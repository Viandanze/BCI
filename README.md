# BCI - EEG Motor Imagery Classification

A deep learning pipeline for EEG-based motor imagery brain-computer interfaces, built on PhysioNet Motor Movement/Imagery Dataset. Implements EEGNet, Conformer, TCN, and stacking ensemble with statistical analysis.

## Results

| Model | Accuracy | Notes |
|-------|----------|-------|
| EEGNet baseline | 45.83% | Default hyperparameters |
| EEGNet + HPO | 54.32% | +8.49pp via grid search |
| Conformer | 38.43% | Small-sample underfitting |
| TCN | 40.35% | Small-sample underfitting |
| Soft Voting | 79.29% | EEGNet + Riemannian MDM |
| Weighted Voting | 80.00% | Optimized weights |
| **Stacking (Logistic)** | **82.14%** | Best - EEGNet + CSP + Riemannian MDM |

> Dataset: PhysioNet EEGMMIDB, 8 subjects, 3-class (left fist / right fist / rest), leave-one-subject-out evaluation.

### Statistical Analysis

- TCN significantly above chance level (33.3%): t(7)=2.40, p=0.048
- Conformer vs TCN: no significant difference (p=0.632)
- Stacking ensemble achieves 82.14% with Kappa=0.54, +4.28pp over best single model
- Full report: outputs/experiments/analysis/statistical_report.md

## Project Structure

```
BCI/
├── src/                          # Core library
│   ├── data/                     # Data loading & preprocessing
│   ├── models/                   # EEGNet, CSP, Riemannian MDM, Conformer, TCN
│   ├── training/                 # Training loop & data augmentation
│   ├── evaluation/               # Metrics & analysis
│   ├── inference/                # Inference utilities
│   ├── visualization/            # Plotting helpers
│   └── utils/                    # Config management
├── scripts/                      # Executable experiments
│   ├── train_eegnet.py           # EEGNet training
│   ├── train_ensemble.py         # Ensemble experiments
│   ├── train_advanced.py         # Conformer & TCN
│   ├── week10_analysis.py        # Statistical tests & visualization
│   └── ...
├── configs/
│   └── default.yaml
├── outputs/
│   └── experiments/              # Results, figures, analysis
│       ├── advanced/             # Conformer & TCN results
│       ├── ensemble/             # Stacking & voting results
│       ├── analysis/             # Statistical report & plots
│       ├── tuning/               # Hyperparameter search logs
│       └── results_summary/      # Aggregated JSON
└── requirements.txt
```

## Quick Start

```bash
# Setup
conda create -n bci python=3.10 -y && conda activate bci
pip install torch>=2.0.0 mne>=1.0.0 scikit-learn>=1.0.0 pyriemann>=0.3.0
pip install -r requirements.txt

# Train EEGNet (auto-downloads PhysioNet dataset)
python scripts/train_eegnet.py --subjects 1 2 3 --epochs 100

# Run ensemble
python scripts/train_ensemble.py

# Statistical analysis & plots
python scripts/week10_analysis.py
```

## Key Design Decisions

- **EEGNet v2** as backbone: compact, interpretable, works well on small EEG datasets
- **6 augmentation methods**: Gaussian noise, temporal/channel masking, time shift, band perturbation, Mixup
- **Leave-one-subject-out (LOSO)** evaluation for generalization assessment
- **Stacking ensemble** bridges deep learning (EEGNet) and classical (CSP, Riemannian) paradigms

## References

1. Lawhern et al. (2018). EEGNet: A compact convolutional network for EEG-based BCIs. *J Neural Eng*.
2. Barachant et al. (2012). Classification of covariance matrices using Riemannian kernel. *NeuroImage*.
3. Blankertz et al. (2008). BCI Competition III. *IEEE TNSRE*.

## License

MIT
