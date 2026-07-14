# BCI_Projects - Motor Imagery Brain-Computer Interface Toolkit

A comprehensive toolkit for EEG-based Motor Imagery classification, implementing state-of-the-art deep learning and classical machine learning approaches.

## 📋 Project Overview

This project provides complete implementations of:
- **EEGNet v2** - Compact CNN for EEG classification (Lawhern et al. 2018)
- **CSP (Common Spatial Pattern)** - Classical BCI approach
- **Riemannian MDM** - Covariance-based classification using Riemannian geometry

### Current Performance Targets

| Model | Multi-Subject | Single-Subject | Target |
|-------|---------------|----------------|--------|
| EEGNet (baseline) | 68.10% | 61.54% | 70%+ |
| CSP | ~65% | ~60% | 70%+ |
| Riemannian MDM | - | 73.63% | 75%+ |

## 📁 Project Structure

```
BCI_Projects/
├── src/                          # Core package
│   ├── data/                      # Data loading and preprocessing
│   │   ├── loader.py             # PhysioNet dataset loader
│   │   └── preprocessing.py      # EEG preprocessing pipeline
│   ├── models/                    # ML models
│   │   ├── eegnet.py             # EEGNet v2 implementation
│   │   ├── csp.py                # CSP classifier
│   │   └── riemann_mdm.py        # Riemannian MDM classifier
│   ├── training/                 # Training utilities
│   │   ├── trainer.py            # Unified training loop
│   │   └── augment.py             # Data augmentation
│   ├── evaluation/               # Metrics
│   │   └── metrics.py            # Comprehensive evaluation
│   └── utils/                    # Utilities
│       └── config.py             # Configuration management
├── scripts/                      # Executable scripts
│   ├── train_eegnet.py           # EEGNet training
│   ├── train_csp.py              # CSP training
│   ├── train_riemann.py          # Riemannian training
│   ├── compare_models.py         # Model comparison
│   └── tune_eegnet.py            # Hyperparameter tuning
├── configs/                      # Configuration files
│   └── default.yaml              # Default configuration
├── outputs/                      # Results and checkpoints
├── README.md                     # This file
└── requirements.txt              # Python dependencies
```

## 🔧 Installation

### Prerequisites

```bash
# Create conda environment (if not already done)
conda create -n bci_dev python=3.10
conda activate bci_dev

# Install PyTorch
pip install torch>=2.0.0

# Install MNE and dependencies
pip install mne>=1.0.0 scipy>=1.7.0 numpy>=1.21.0

# Install scikit-learn
pip install scikit-learn>=1.0.0

# Install pyRiemann (for Riemannian classifiers)
pip install pyriemann>=0.3.0

# Install Braindecode (optional, for additional models)
pip install braindecode>=0.8.0

# Install other utilities
pip install pyyaml matplotlib tqdm
```

### Quick Install

```bash
cd BCI_Projects
pip install -r requirements.txt
```

## 🚀 Quick Start

### 1. Download PhysioNet Dataset

The first time you run a script, MNE will attempt to download the PhysioNet Motor Movement/Imagery dataset. This requires internet access.

```bash
python scripts/train_eegnet.py --subjects 1 2 3
```

### 2. Train EEGNet

```bash
# Basic training
python scripts/train_eegnet.py --subjects 1 2 3 --epochs 100

# With data augmentation
python scripts/train_eegnet.py --subjects 1 2 --augment --epochs 100

# With cross-validation
python scripts/train_eegnet.py --subjects 1 --cv_folds 5
```

### 3. Train CSP

```bash
python scripts/train_csp.py --subjects 1 2 3 --n_components 4
```

### 4. Train Riemannian MDM

```bash
python scripts/train_riemann.py --subjects 1 2 --metric riemann
```

### 5. Compare Models

```bash
python scripts/compare_models.py --subjects 1 2 3 --quick
```

### 6. Hyperparameter Tuning

```bash
# Run all strategies
python scripts/tune_eegnet.py --all --subjects 1 2

# Run specific strategy
python scripts/tune_eegnet.py --strategy A --augmentations gaussian_noise mixup
python scripts/tune_eegnet.py --strategy B --full_search
python scripts/tune_eegnet.py --strategy C --improvements batchnorm
```

## 📊 EEGNet Tuning Strategies

### Strategy A: Data Augmentation
Systematically test augmentation methods:
- Gaussian Noise
- Temporal Masking
- Channel Masking
- Time Shifting
- Band Perturbation
- Mixup

### Strategy B: Hyperparameter Grid Search
Search over key parameters:
- F1 (temporal filters): [4, 8, 16]
- D (depth multiplier): [1, 2, 4]
- Dropout: [0.3, 0.5, 0.7]
- Kernel length: [32, 64, 128]

### Strategy C: Architecture Improvements
Test architectural modifications:
- Batch Normalization
- Label Smoothing
- SE Attention
- Combined approaches

## 🔬 Key Features

### Data Augmentation (6+ Methods)
```python
from src.training.augment import EEGAugmentor, AugmentationConfig

config = AugmentationConfig(
    enabled=True,
    temporal_mask={'enabled': True, 'prob': 0.3},
    channel_mask={'enabled': True, 'prob': 0.2},
    gaussian_noise={'enabled': True, 'prob': 0.3, 'snr_db': 10},
    time_shift={'enabled': True, 'prob': 0.2},
    band_perturbation={'enabled': True, 'prob': 0.2},
    mixup={'enabled': True, 'prob': 0.3, 'alpha': 0.2},
)

augmentor = EEGAugmentor(config, sfreq=128)
X_aug = augmentor.augment(X)
```

### Preprocessing Pipeline
```python
from src.data.preprocessing import PreprocessingPipeline, PreprocessingConfig

config = PreprocessingConfig(
    bandpass_low=4,
    bandpass_high=38,
    tmin=-1.0,
    tmax=4.0,
    baseline=(-1.0, 0.0),
    normalize=True,
    resample_freq=128,
)

pipeline = PreprocessingPipeline(config)
epochs = pipeline.process_raw(raw)
```

## 📈 Experiment Results Template

### Multi-Subject (LOSO) Results

| Subject | EEGNet | CSP | Riemannian | Notes |
|---------|--------|-----|------------|-------|
| S001 | XX.XX% | XX.XX% | XX.XX% | |
| S002 | XX.XX% | XX.XX% | XX.XX% | |
| ... | ... | ... | ... | |
| **Mean** | **XX.XX%** | **XX.XX%** | **XX.XX%** | |

### Hyperparameter Tuning Results

| Strategy | Best Accuracy | Best Config |
|----------|---------------|-------------|
| A (Augmentation) | XX.XX% | method=XXX |
| B (Grid Search) | XX.XX% | F1=X, D=X, dropout=X.X |
| C (Architecture) | XX.XX% | improvement=XXX |

## 🛠️ Configuration

### YAML Configuration

```yaml
# configs/default.yaml
data:
  dataset_path: "./BCI_Projects/data/"
  subjects: [1, 2, 3, 4, 5, 6, 7, 8]
  runs: [4, 5, 6]

preprocessing:
  bandpass_low: 4
  bandpass_high: 38
  normalize: true

eegnet:
  F1: 8
  D: 2
  kernel_length: 64
  dropout_rate: 0.5
  epochs: 100
  batch_size: 64
  learning_rate: 0.001

augmentation:
  enabled: true
  probability: 0.5
  temporal_mask:
    enabled: true
    prob: 0.3
```

## 📝 Tips for Breaking 70% Barrier

1. **Data Augmentation**: Start with Gaussian noise and temporal masking
2. **Hyperparameter Search**: Focus on F1 and D parameters
3. **Subject-Specific Tuning**: Different subjects may need different configurations
4. **Ensemble Methods**: Combine predictions from multiple models
5. **More Data**: Use all 109 subjects if possible

## 🐛 Troubleshooting

### Dataset Download Issues
If the PhysioNet dataset fails to download:
```bash
# Check your internet connection
# Try setting a proxy if behind firewall
# Use synthetic data for testing: scripts will auto-generate if download fails
```

### Out of Memory
```bash
# Reduce batch size
python scripts/train_eegnet.py --subjects 1 --batch_size 32

# Use CPU if GPU memory is limited
python scripts/train_eegnet.py --subjects 1 --device cpu
```

### Import Errors
```bash
# Make sure you're in the project root
cd BCI_Projects
export PYTHONPATH="${PYTHONPATH}:$(pwd)"

# Or run scripts directly
python scripts/train_eegnet.py
```

## 📚 References

1. Lawhern, V. J., et al. (2018). EEGNet: A compact convolutional neural network for EEG-based brain-computer interfaces. *Journal of Neural Engineering*.

2. Blankertz, B., et al. (2008). The BCI competition III: Validating alternative approaches to actual EEG problems. *IEEE TNSRE*.

3. Barachant, A., et al. (2012). Classification of covariance matrices using a Riemannian-based kernel for BCI applications. *NeuroImage*.

## 📄 License

This project is for educational and research purposes.

## 🤝 Contributing

Contributions welcome! Please submit issues and pull requests.

---

**Author**: UE5AssetAnalyzer  
**Last Updated**: 2024
