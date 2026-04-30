# BCI-Demo

从零手搓非侵入式脑机接口，目标BCI+LLM融合领域国内Top 10。

这项目是干嘛的
---

老实说，之前那个README确实太敷衍了，就几行"This is my first BCI project"。现在补上正经的。

我是计算机科学专业的，主攻C++游戏程序开发和数字孪生，2026年开始折腾BCI。这项目就是我的脑机接口研究平台，从零开始搭，包括：

- 三大经典范式：运动想象(MI)、P300、SSVEP
- EEG信号处理和特征提取
- 深度学习模型（EEGNet等）
- 最终目标：把EEG特征解码成意图，喂给LLM，实现人意识和大模型"意识"相连

听起来很遥远？确实。但不试试怎么知道。

当前进展
---

**模型准确率（PhysioNet数据集）**：

| 方法 | 准确率 |
|------|--------|
| pyRiemann基准 | 73.63% |
| EEGNet多受试者 | 68.10% |
| EEGNet单受试者 | 61.54% |

当前受试者表现：S1: 70.77%, S2: 70.77%, S3: 67.69%, S4: 64.62%, S5: 66.67%

还有很长的路要走，先把准确率拉到80%以上再说。

一年计划（2026.4 - 2027.4）
---

**Q1（4-6月）：打地基**
三大范式pipeline搭起来，运动想象准确率先过70%

**Q2（7-9月）：深度学习进阶**
EEGNet调优、跨受试者迁移学习、Transformer实验

**Q3（10-12月）：BCI+LLM融合**
EEG特征 → 意图解码 → LLM → 文本输出，这条链路要跑通

**Q4（2027.1-4月）：突破与展示**
多模态、实时闭环、开源v1.0

技术栈
---

```python
# 信号处理
MNE-Python          # EEG预处理必备
pyRiemann           # 黎曼几何分类，效果不错
BrainFlow           # 统一设备接口，支持50+种BCI硬件
MetaBCI             # 国产BCI全栈平台，可以研究下

# 深度学习
PyTorch             # 主框架
braindecode         # EEG专用深度学习库
EEGNet              # 经典轻量模型

# 基础
numpy, scipy, scikit-learn, matplotlib
```

怎么跑起来
---

```bash
# 创建环境
conda create -n bci python=3.9
conda activate bci

# 安装依赖
pip install -r requirements.txt

# 下载PhysioNet数据集
python -c "import mne; mne.datasets.eegbci.load_data(subject=1, runs=[6,10])"

# 跑运动想象demo
python demos/eegnet_multisubject_train.py
```

demos目录下的东西都是可运行的，按需取用。

项目结构
---

```
BCI_Projects/
├── demos/                         # 实验代码
│   ├── eegnet_train.py            # 单受试者训练
│   ├── eegnet_multisubject_train.py
│   ├── pyriemann_motor_imagery_demo.py
│   ├── SSVEP相关（ssvep_demo.py, FBTRCA, FBTDCA, sceTRCA...）
│   ├── P300_demo.py
│   ├── neurolm_inference.py       # LLM融合
│   ├── transfer_learning.py
│   └── dynamic_stopping_demos/     # 动态停止
├── braindecode/                   # 深度学习框架
├── brainflow/                     # 硬件接口
├── MetaBCI/                       # 国产全栈平台
├── 学习笔记/                      # 学习记录
├── 周报/                          # 进度汇报
├── 里程碑计划.md
└── requirements.txt
```

依赖
---

```txt
mne>=1.0.0
pyriemann>=0.3.0
braindecode>=0.8.0
torch>=2.0.0
numpy>=1.21.0
scipy>=1.7.0
scikit-learn>=1.0.0
matplotlib>=3.5.0
```

关于作者
---

曾炜峻（BruceZeng）
- GitHub: https://github.com/Viandanze
- 主攻：C++游戏程序开发、数字孪生、BCI+LLM融合
- INTJ，说话直接，不整虚的

学习交流可以提Issue，别问我"在吗"，直接说事。

License
---

MIT
