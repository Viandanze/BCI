# LeetCode 45: 跳跃游戏 II

## 题目描述

给定一个非负整数数组 `nums`，你最初位于数组的第一个位置。数组中的每个元素代表你在该位置可以跳跃的最大长度。你的目标是使用**最少的跳跃次数**到达数组的最后一个位置。

**注意**：假设你总是可以到达数组的最后一个位置。

### 示例

```
输入: nums = [2,3,1,1,4]
输出: 2
解释: 从下标0跳1步到下标1，然后跳3步到最后位置。
```

---

## 解题思路

### 核心思想：贪心算法

**关键洞察**：每次跳跃都选择"下一跳能到达最远位置"的落脚点，局部最优即全局最优。

### 算法步骤

1. 维护两个变量：
   - `current_end`：当前跳跃的边界（本次跳跃能到达的最远位置）
   - `farthest`：在当前跳跃范围内，下一次跳跃能到达的最远位置

2. 遍历数组（到倒数第二个元素）：
   - 更新 `farthest = max(farthest, i + nums[i])`
   - 当遍历到当前边界时，执行一次跳跃，更新边界为 `farthest`，跳跃次数 +1

3. 最终返回跳跃次数

### 游戏开发类比

想象你在设计一个平台跳跃游戏：
- 每个平台有一个"弹跳力"值（对应 `nums[i]`）
- 玩家需要用最少的跳跃次数到达终点
- 贪心策略：站在当前位置时，先"环顾四周"，选择能让你下一次跳得最远的中转站

---

## C++ 实现

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * LeetCode 45: 跳跃游戏 II
 * 
 * 贪心算法 O(n) 时间复杂度
 * 
 * 游戏开发应用：平台跳跃类游戏的关卡设计验证
 */

class JumpGameII {
public:
    /**
     * 贪心算法：维护当前跳跃边界，在边界处进行跳跃
     * 
     * @param nums 跳跃能力数组
     * @return 最少跳跃次数
     */
    static int jump(vector<int>& nums) {
        int n = nums.size();
        if (n <= 1) return 0;
        
        int jumps = 0;           // 跳跃次数
        int current_end = 0;     // 当前跳跃的边界
        int farthest = 0;        // 下一跳能到达的最远位置
        
        // 遍历到倒数第二个元素（不需要处理最后一位，因为已经到达终点）
        for (int i = 0; i < n - 1; i++) {
            // 更新最远可达位置
            farthest = max(farthest, i + nums[i]);
            
            // 到达当前边界，执行跳跃
            if (i == current_end) {
                jumps++;
                current_end = farthest;
                
                // 提前终止优化：如果当前边界已覆盖终点
                if (current_end >= n - 1) {
                    break;
                }
            }
        }
        
        return jumps;
    }
    
    /**
     * 详细版本：记录每一步的决策过程（用于调试/学习）
     */
    static int jumpVerbose(vector<int>& nums) {
        int n = nums.size();
        if (n <= 1) return 0;
        
        int jumps = 0;
        int current_end = 0;
        int farthest = 0;
        int step_start = 0;
        
        cout << "跳跃过程追踪:" << endl;
        cout << "========================================" << endl;
        
        for (int i = 0; i < n - 1; i++) {
            farthest = max(farthest, i + nums[i]);
            
            if (i == current_end) {
                jumps++;
                cout << "第 " << jumps << " 跳: 从位置 " << step_start 
                     << " 开始，可达范围 [0, " << farthest << "]" << endl;
                
                step_start = current_end + 1;
                current_end = farthest;
                
                if (current_end >= n - 1) {
                    cout << "已达到终点！" << endl;
                    break;
                }
            }
        }
        
        cout << "========================================" << endl;
        return jumps;
    }
};

/**
 * 辅助函数：验证解法正确性
 */
void runTests() {
    vector<vector<int>> testCases = {
        {2, 3, 1, 1, 4},      // 标准案例: 2
        {2, 3, 0, 1, 4},      // 含0的案例: 2
        {1, 1, 1, 1, 1},      // 最坏情况: 4
        {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},  // 大跨度: 1
        {0},                   // 单元素: 0
        {1, 2},                // 简单: 1
    };
    
    for (size_t i = 0; i < testCases.size(); i++) {
        vector<int> nums = testCases[i];
        int result = JumpGameII::jump(nums);
        
        cout << "测试用例 " << (i + 1) << ": ";
        for (int num : nums) cout << num << " ";
        cout << "=> 最少跳跃次数: " << result << endl;
    }
}

int main() {
    cout << "===== LeetCode 45: 跳跃游戏 II =====" << endl << endl;
    
    // 运行测试
    runTests();
    cout << endl;
    
    // 详细演示
    vector<int> nums = {2, 3, 1, 1, 4};
    cout << "详细分析 nums = {2, 3, 1, 1, 4}:" << endl;
    JumpGameII::jumpVerbose(nums);
    
    return 0;
}
```

---

## 复杂度分析

### 时间复杂度：O(n)

- 只遍历数组一次
- 每次遍历只做常数时间操作（max比较）

### 空间复杂度：O(1)

- 只使用常数个额外变量
- 不依赖输入规模增长

### 对比分析

| 算法 | 时间复杂度 | 空间复杂度 | 说明 |
|------|----------|----------|------|
| 暴力 BFS | O(n²) | O(n) | 展开所有可达位置 |
| 贪心优化 | **O(n)** | **O(1)** | 当前最优选择 |

---

## 优化方向

1. **提前终止**：当 `current_end >= n - 1` 时提前退出
2. **剪枝策略**：如果 `farthest == current_end` 且不等于终点，说明陷入死路（但题目保证可达）
3. **二分优化**：对于固定跳跃次数，找到最小可达边界（二分搜索）

---

## 游戏开发实际应用

### 场景1：关卡可通关性验证

```cpp
class LevelValidator {
public:
    bool isLevelCompletable(vector<int>& jumpCapabilities) {
        // 使用跳跃游戏算法验证关卡
        return JumpGameII::jump(jumpCapabilities) > 0;
    }
};
```

### 场景2：AI寻路决策

```cpp
class AINavigation {
public:
    // 在网格世界中，nums[i] 表示位置 i 可以移动到的最大步数
    int findMinJumps(vector<int>& grid) {
        return JumpGameII::jump(grid);
    }
};
```

### 场景3：资源预算分配

```cpp
class ResourceAllocator {
public:
    // 将资源分配问题转化为跳跃游戏问题
    // nums[i] = 可从资源包 i 获取的资源数量
    int minAllocationSteps(vector<int>& resources) {
        return JumpGameII::jump(resources);
    }
};
```

---

## 总结

| 维度 | 内容 |
|-----|------|
| **题目** | LeetCode 45 - 跳跃游戏 II |
| **难度** | 中等 (Medium) |
| **核心算法** | 贪心 + 双边界 |
| **代码模式** | 数组遍历 + 边界更新 |
| **面试高频** | ⭐⭐⭐⭐⭐ |
| **游戏开发场景** | 平台跳跃、寻路、关卡验证 |

**核心要点**：维护当前跳跃边界，在遍历过程中贪心选择最优落脚点，局部最优即全局最优。
