# Swimming Mechanics Blueprint - UE5 Implementation

## Project: EssentialUE5BPs Enhancement
**Author**: BruceZeng  
**Date**: 2026-05-06  
**Status**: Ready for PR

---

## Overview

This contribution adds a fully-featured **Swimming Mechanics** blueprint system to the EssentialUE5BPs project, as outlined in the planned features. The implementation supports both First-Person and Third-Person perspectives with configurable physics and animations.

---

## Features

### Core Swimming Mechanics

1. **Automatic Water Detection**
   - Uses line traces to detect water surface
   - Automatic state transition between walking and swimming
   - Configurable detection height threshold

2. **Movement Physics**
   - Underwater movement with reduced gravity
   - Configurable swim speed (default: 300 units/s)
   - Sprint capability while swimming
   - Depth-based movement resistance

3. **Breathing System**
   - Configurable air/oxygen duration
   - Visual HUD indicator for remaining air
   - Damage when air depleted (optional)
   - Surface-to-breath indicator

4. **Camera System**
   - Smooth underwater camera effect
   - Auto-surface camera look adjustment
   - Support for both First-Person and Third-Person

5. **Animation Integration**
   - Swimming idle animation
   - Directional swimming animations
   - Surface swimming animations
   - Transition animations between states

---

## Blueprint Structure

```
Content/
├── Blueprints/
│   ├── BP_SwimmingComponent.uasset    # Core swimming logic component
│   ├── BP_SwimmingCharacter.uasset   # Character with swimming integrated
│   └── BP_WaterVolume.uasset         # Custom water volume for advanced effects
├── UI/
│   └── WBP_AirIndicator.uasset       # Air/oxygen HUD widget
└── Animations/
    ├── AB_Swimming.uasset            # Animation blueprint
    └── AnimNotify_Surface.uasset      # Animation notify for breathing
```

---

## Technical Implementation

### BP_SwimmingComponent

**Properties**:
| Property | Type | Default | Description |
|----------|------|---------|-------------|
| SwimSpeed | float | 300.0 | Base swimming speed |
| SprintSwimSpeed | float | 450.0 | Sprint swimming speed |
| GravityScaleSwimming | float | 0.3 | Reduced gravity underwater |
| MaxAirDuration | float | 30.0 | Seconds of air available |
| bDamageWhenOutOfAir | bool | false | Enable drowning damage |
| DamagePerSecond | float | 10.0 | Damage when out of air |
| SurfaceTraceDistance | float | 100.0 | Distance to trace for water surface |
| BreathingDepthThreshold | float | 50.0 | Min depth to breathe at surface |

**Functions**:
- `EnterWater()` - Called when character enters water
- `ExitWater()` - Called when character leaves water
- `UpdateSwimming(float DeltaTime)` - Main swimming logic tick
- `UpdateAir(float DeltaTime)` - Breathing system update
- `TryToBreathe()` - Attempt to breathe at surface
- `CanBreatheAtSurface()` - Check if character can breathe
- `OnAirDepleted()` - Event when air runs out
- `OnAirRestored()` - Event when character breathes

**Events**:
- `OnEnterWater` - Dispatched when entering water
- `OnExitWater` - Dispatched when leaving water
- `OnAirLow` - Dispatched when air below 25%
- `OnAirDepleted` - Dispatched when air reaches 0

### BP_WaterVolume (Optional Enhancement)

**Features**:
- Custom water physics (lava, toxic, etc.)
- Particle effects for water surface
- Sound effects for entering/exiting
- Underwater fog/visibility effects

### WBP_AirIndicator

**Design**:
- Circular progress indicator
- Color transitions: Blue → Yellow → Red
- Pulse animation when air critical (<10%)
- Smooth fill/drain animation

---

## Installation Guide

### For EssentialUE5BPs Integration

1. **Copy Files**
   ```
   Copy Blueprints/ folder to your project's Content/
   Copy UI/ folder if using air indicator
   ```

2. **Character Integration**
   - Open your character blueprint
   - Add `BP_SwimmingComponent` as a subobject
   - Connect `BeginPlay` to initialize swimming system
   - Override movement mode callbacks

3. **Input Setup**
   ```cpp
   // Enhanced Input Actions needed:
   // - IA_SwimUp    (Space/Jump key while swimming)
   // - IA_SwimDown  (Crouch key while swimming)
   ```

4. **Animation Setup**
   - Create Animation Blueprint based on `AB_Swimming`
   - Connect swimming blend space to character movement state
   - Add surface breathing notify

### Configuration

Edit `BP_SwimmingComponent` properties for your game's needs:

```ini
[/Script/SwimmingExtension.SwimmingComponent]
SwimSpeed=350.0
MaxAirDuration=45.0
bDamageWhenOutOfAir=true
DamagePerSecond=5.0
```

---

## Code Implementation (C++ Alternative)

For developers preferring C++, here's the header:

```cpp
// SwimmingComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwimmingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAirLow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAirDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBreathe);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SWIMMING_API USwimmingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USwimmingComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swimming")
    float SwimSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swimming")
    float SprintSwimSpeed = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swimming")
    float GravityScaleSwimming = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breathing")
    float MaxAirDuration = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breathing")
    bool bDamageWhenOutOfAir = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breathing")
    float DamagePerSecond = 10.0f;

    UPROPERTY(BlueprintAssignable)
    FOnAirLow OnAirLow;

    UPROPERTY(BlueprintAssignable)
    FOnAirDepleted OnAirDepleted;

    UPROPERTY(BlueprintAssignable)
    FOnBreathe OnBreathe;

    UFUNCTION(BlueprintCallable)
    void EnterWater();

    UFUNCTION(BlueprintCallable)
    void ExitWater();

    UFUNCTION(BlueprintPure)
    float GetCurrentAirPercent() const { return CurrentAir / MaxAirDuration; }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float CurrentAir = 30.0f;
    bool bIsSwimming = false;
    bool bCanBreathe = false;

    void UpdateSwimming(float DeltaTime);
    void UpdateBreathing(float DeltaTime);
    bool CanBreatheAtSurface() const;
    void TryToBreathe();
};
```

---

## Testing Checklist

- [ ] Character enters water smoothly
- [ ] Character exits water properly
- [ ] Swim speed feels responsive
- [ ] Air depletes correctly
- [ ] Air restores at surface
- [ ] HUD indicator updates smoothly
- [ ] First-person camera underwater
- [ ] Third-person camera underwater
- [ ] Sprint works while swimming
- [ ] Depth affects breathing difficulty
- [ ] Drowning damage triggers correctly
- [ ] Animations blend properly

---

## Contribution Notes

This implementation follows the project's existing architecture and coding standards. Key principles:

1. **Modularity**: Can be added to any character without modifying base classes
2. **Configurability**: All values exposed for designers
3. **Performance**: Efficient checks, no per-frame allocations
4. **Accessibility**: Full Blueprint support with C++ for optimization

---

## Future Enhancements

Possible additions for future PRs:
1. **Diving Bell/Lock Mechanism** - For submarine mechanics
2. **Underwater Combat** - Swimming attack animations
3. **Swimming AI** - For NPCs that swim
4. **Water Current System** - Push/pull effects
5. **Swimming Tutorial** - Example map with swimming test area

---

## License

Same as EssentialUE5BPs project (MIT).
