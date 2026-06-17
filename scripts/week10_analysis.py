"""
BCI Week10 实验统计分析与可视化
生成：统计显著性检验 + 对比图表 + 混淆矩阵 + 综合报告
用法：python scripts/week10_analysis.py
"""

import json
import os
import numpy as np
from pathlib import Path
from scipy import stats
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay

# ─── 配置 ───
PROJECT_ROOT = Path(__file__).resolve().parent.parent
OUTPUT_DIR = PROJECT_ROOT / "outputs" / "week10_experiments" / "analysis"
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False
plt.rcParams['figure.dpi'] = 150

# ─── 数据加载 ───
def load_conformer_results():
    path = PROJECT_ROOT / "outputs/week10_experiments/advanced/conformer"
    dirs = sorted(path.glob("conformer_s*/"))
    if not dirs:
        return None
    with open(dirs[-1] / "results.json") as f:
        return json.load(f)

def load_tcn_results():
    path = PROJECT_ROOT / "outputs/week10_experiments/advanced/tcn"
    dirs = sorted(path.glob("tcn_s*/"))
    if not dirs:
        return None
    with open(dirs[-1] / "results.json") as f:
        return json.load(f)

def load_ensemble_results():
    results = {}
    base = PROJECT_ROOT / "outputs/week10_experiments/ensemble"
    # Voting
    voting_dirs = sorted((base / "voting_soft").glob("results_*.json"))
    if voting_dirs:
        with open(voting_dirs[-1]) as f:
            results['voting'] = json.load(f)
    # Stacking
    stacking_dirs = sorted((base / "stacking_tangent").glob("results_*.json"))
    if stacking_dirs:
        with open(stacking_dirs[-1]) as f:
            results['stacking'] = json.load(f)
    # Weighted
    weighted_dirs = sorted((base / "weighted").glob("results_*.json"))
    if weighted_dirs:
        with open(weighted_dirs[-1]) as f:
            results['weighted'] = json.load(f)
    return results if results else None

def load_strategy_results():
    results = {}
    for name in ['A_augmentation', 'B_hyperparams', 'C_architecture']:
        paths = sorted((PROJECT_ROOT / "outputs").glob(f"strategy_{name}_*.json"))
        if paths:
            with open(paths[-1]) as f:
                results[name[0]] = json.load(f)
    return results if results else None

# ─── 统计检验 ───
def statistical_tests(conformer, tcn, strategy, ensemble):
    """对关键对比做统计显著性检验"""
    report_lines = []
    report_lines.append("# BCI Week10 统计显著性检验报告\n")
    report_lines.append(f"生成时间：{__import__('datetime').datetime.now().strftime('%Y-%m-%d %H:%M')}\n\n")

    # 1. Conformer vs TCN 配对检验
    if conformer and tcn:
        conf_accs = [conformer[f'subject_{i}']['mean_accuracy'] for i in range(1, 9)]
        tcn_accs = [tcn[f'subject_{i}']['mean_accuracy'] for i in range(1, 9)]
        
        report_lines.append("## 1. Conformer vs TCN（8受试者配对）\n\n")
        report_lines.append(f"| 指标 | Conformer | TCN |\n|------|-----------|-----|\n")
        report_lines.append(f"| Mean±Std | {np.mean(conf_accs):.4f}±{np.std(conf_accs):.4f} | {np.mean(tcn_accs):.4f}±{np.std(tcn_accs):.4f} |\n\n")

        # Shapiro-Wilk正态性检验
        diff = np.array(conf_accs) - np.array(tcn_accs)
        sw_stat, sw_p = stats.shapiro(diff)
        report_lines.append(f"**差值正态性检验**（Shapiro-Wilk）：W={sw_stat:.4f}, p={sw_p:.4f}\n\n")

        if sw_p > 0.05:
            # 配对t检验
            t_stat, p_val = stats.ttest_rel(conf_accs, tcn_accs)
            report_lines.append(f"**配对t检验**：t={t_stat:.4f}, p={p_val:.4f}\n")
            report_lines.append(f"结论：{'显著差异' if p_val < 0.05 else '无显著差异'}（α=0.05）\n\n")
        else:
            # Wilcoxon符号秩检验
            w_stat, p_val = stats.wilcoxon(conf_accs, tcn_accs)
            report_lines.append(f"**Wilcoxon符号秩检验**：W={w_stat:.4f}, p={p_val:.4f}\n")
            report_lines.append(f"结论：{'显著差异' if p_val < 0.05 else '无显著差异'}（α=0.05）\n\n")

        # 效果量（Cohen's d）
        d = np.mean(diff) / np.std(diff, ddof=1) if np.std(diff, ddof=1) > 0 else 0
        report_lines.append(f"**Cohen's d** = {d:.4f}（{'大' if abs(d)>=0.8 else '中' if abs(d)>=0.5 else '小'}效果量）\n\n")

    # 2. 各受试者准确率 vs 随机基线
    if conformer:
        conf_accs = [conformer[f'subject_{i}']['mean_accuracy'] for i in range(1, 9)]
        n_classes = 3  # Conformer是3分类
        random_baseline = 1.0 / n_classes
        report_lines.append("## 2. Conformer各受试者 vs 随机基线\n\n")
        report_lines.append(f"随机基线（3分类）= {random_baseline:.4f}\n\n")
        
        above_random = sum(1 for a in conf_accs if a > random_baseline)
        t_stat, p_val = stats.ttest_1samp(conf_accs, random_baseline)
        report_lines.append(f"**单样本t检验**：t={t_stat:.4f}, p={p_val:.4f}\n")
        report_lines.append(f"8个受试者中{above_random}个超过随机基线\n\n")

    if tcn:
        tcn_accs = [tcn[f'subject_{i}']['mean_accuracy'] for i in range(1, 9)]
        n_classes = 3
        random_baseline = 1.0 / n_classes
        report_lines.append("## 3. TCN各受试者 vs 随机基线\n\n")
        report_lines.append(f"随机基线（3分类）= {random_baseline:.4f}\n\n")
        
        above_random = sum(1 for a in tcn_accs if a > random_baseline)
        t_stat, p_val = stats.ttest_1samp(tcn_accs, random_baseline)
        report_lines.append(f"**单样本t检验**：t={t_stat:.4f}, p={p_val:.4f}\n")
        report_lines.append(f"8个受试者中{above_random}个超过随机基线\n\n")

    # 3. Strategy B 超参搜索 vs 基线
    if strategy and 'B' in strategy:
        strat_b = strategy['B']
        best_acc = strat_b['best_accuracy']
        baseline_acc = strategy.get('A', {}).get('augmentation_results', {}).get('baseline', {}).get('accuracy', 0.4583)
        improvement = best_acc - baseline_acc
        report_lines.append("## 4. 超参搜索 vs 基线\n\n")
        report_lines.append(f"| 模型 | Accuracy | 提升 |\n|------|----------|------|\n")
        report_lines.append(f"| EEGNet基线 | {baseline_acc:.4f} | — |\n")
        report_lines.append(f"| EEGNet+超参搜索 | {best_acc:.4f} | +{improvement:.4f} ({improvement*100:.2f}pp) |\n\n")
        report_lines.append(f"**注意**：Strategy结果为整体平均，无per-subject数据，无法做配对检验。\n")
        report_lines.append(f"提升幅度{improvement*100:.2f}个百分点，实际显著性需更多被试验证。\n\n")

    # 4. 集成学习效果
    if ensemble:
        report_lines.append("## 5. 集成学习效果\n\n")
        report_lines.append("| 策略 | Accuracy | Kappa | 单模型EEGNet | 单模型Riemann |\n|------|----------|-------|-------------|--------------|\n")
        for name, data in ensemble.items():
            acc = data.get('ensemble_accuracy', 0)
            kappa = data.get('ensemble_kappa', 0)
            eegnet = data.get('single_model_results', {}).get('eegnet', 0)
            riemann = data.get('single_model_results', {}).get('riemann', 0)
            report_lines.append(f"| {name} | {acc:.4f} | {kappa:.4f} | {eegnet:.4f} | {riemann:.4f} |\n")
        report_lines.append(f"\n**注意**：集成实验使用留出法，仅一个测试集精度，无多次重复，无法做统计检验。\n")
        report_lines.append(f"Stacking 82.14% vs EEGNet单模型77.86%，提升4.28pp，但需交叉验证确认稳定性。\n\n")

    return ''.join(report_lines)


# ─── 可视化 ───
def plot_accuracy_comparison(conformer, tcn, strategy, ensemble):
    """图1：所有方法准确率对比条形图"""
    methods = []
    accuracies = []
    errors = []
    colors = []

    # EEGNet基线
    if strategy and 'A' in strategy:
        baseline_acc = strategy['A']['augmentation_results']['baseline']['accuracy']
        methods.append('EEGNet\n基线')
        accuracies.append(baseline_acc * 100)
        errors.append(0)
        colors.append('#95a5a6')

    # Strategy A最佳
    if strategy and 'A' in strategy:
        best_a = strategy['A']['best_accuracy']
        methods.append('EEGNet\n+Mixup')
        accuracies.append(best_a * 100)
        errors.append(0)
        colors.append('#3498db')

    # Strategy B最佳
    if strategy and 'B' in strategy:
        best_b = strategy['B']['best_accuracy']
        methods.append('EEGNet\n+超参搜索')
        accuracies.append(best_b * 100)
        errors.append(0)
        colors.append('#2ecc71')

    # Conformer
    if conformer:
        accs = [conformer[f'subject_{i}']['mean_accuracy'] for i in range(1, 9)]
        methods.append('Conformer')
        accuracies.append(np.mean(accs) * 100)
        errors.append(np.std(accs) * 100)
        colors.append('#e74c3c')

    # TCN
    if tcn:
        accs = [tcn[f'subject_{i}']['mean_accuracy'] for i in range(1, 9)]
        methods.append('TCN')
        accuracies.append(np.mean(accs) * 100)
        errors.append(np.std(accs) * 100)
        colors.append('#e67e22')

    # Ensemble
    if ensemble:
        for name, color in [('Voting', '#9b59b6'), ('Stacking', '#1abc9c'), ('Weighted', '#f39c12')]:
            if name.lower() in ensemble:
                acc = ensemble[name.lower()]['ensemble_accuracy']
                methods.append(f'{name}\n集成')
                accuracies.append(acc * 100)
                errors.append(0)
                colors.append(color)

    fig, ax = plt.subplots(figsize=(14, 6))
    x = np.arange(len(methods))
    bars = ax.bar(x, accuracies, yerr=errors, capsize=5, color=colors, edgecolor='white', linewidth=1.5, width=0.6)

    # 数值标注
    for bar, acc, err in zip(bars, accuracies, errors):
        ax.text(bar.get_x() + bar.get_width() / 2., bar.get_height() + err + 1,
                f'{acc:.1f}%', ha='center', va='bottom', fontsize=11, fontweight='bold')

    # 随机基线
    ax.axhline(y=33.3, color='red', linestyle='--', alpha=0.5, label='3类随机基线 (33.3%)')
    ax.axhline(y=50.0, color='blue', linestyle='--', alpha=0.3, label='2类随机基线 (50.0%)')

    ax.set_ylabel('Accuracy (%)', fontsize=13)
    ax.set_title('BCI Week10 全部方法准确率对比', fontsize=15, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(methods, fontsize=10)
    ax.set_ylim(0, 95)
    ax.legend(loc='upper left', fontsize=10)
    ax.grid(axis='y', alpha=0.3)
    
    plt.tight_layout()
    path = OUTPUT_DIR / "accuracy_comparison.png"
    plt.savefig(path, bbox_inches='tight')
    plt.close()
    print(f"✅ 保存: {path}")
    return path


def plot_subject_heatmap(conformer, tcn):
    """图2：8受试者×多方法准确率热力图"""
    if not conformer and not tcn:
        return None

    methods = []
    data = []
    
    if conformer:
        methods.append('Conformer')
        data.append([conformer[f'subject_{i}']['mean_accuracy'] * 100 for i in range(1, 9)])
    
    if tcn:
        methods.append('TCN')
        data.append([tcn[f'subject_{i}']['mean_accuracy'] * 100 for i in range(1, 9)])

    data = np.array(data)
    subjects = [f'S{i}' for i in range(1, 9)]

    fig, ax = plt.subplots(figsize=(12, 4))
    im = ax.imshow(data, cmap='RdYlGn', aspect='auto', vmin=20, vmax=60)
    
    # 数值标注
    for i in range(len(methods)):
        for j in range(len(subjects)):
            val = data[i, j]
            color = 'white' if val < 30 or val > 50 else 'black'
            ax.text(j, i, f'{val:.1f}%', ha='center', va='center', fontsize=11, fontweight='bold', color=color)

    ax.set_xticks(np.arange(len(subjects)))
    ax.set_xticklabels(subjects, fontsize=11)
    ax.set_yticks(np.arange(len(methods)))
    ax.set_yticklabels(methods, fontsize=11)
    ax.set_title('Conformer / TCN 各受试者准确率热力图', fontsize=14, fontweight='bold')
    
    cbar = plt.colorbar(im, ax=ax, shrink=0.8)
    cbar.set_label('Accuracy (%)', fontsize=11)
    
    plt.tight_layout()
    path = OUTPUT_DIR / "subject_heatmap.png"
    plt.savefig(path, bbox_inches='tight')
    plt.close()
    print(f"✅ 保存: {path}")
    return path


def plot_confusion_matrices(conformer, tcn):
    """图3：Conformer和TCN的混淆矩阵"""
    results = []
    for name, data in [('Conformer', conformer), ('TCN', tcn)]:
        if data is None:
            continue
        # 聚合所有受试者的y_true/y_pred
        all_true = []
        all_pred = []
        for i in range(1, 9):
            subj = data[f'subject_{i}']
            all_true.extend(subj['y_true'])
            all_pred.extend(subj['y_pred'])
        results.append((name, all_true, all_pred))

    if not results:
        return None

    fig, axes = plt.subplots(1, len(results), figsize=(6 * len(results), 5))
    if len(results) == 1:
        axes = [axes]

    # 3类标签
    class_names = ['左拳', '右拳', '双脚']

    for ax, (name, y_true, y_pred) in zip(axes, results):
        n_classes = len(set(y_true))
        if n_classes == 3:
            labels = class_names
        else:
            labels = [f'Class {i}' for i in range(n_classes)]
        
        cm = confusion_matrix(y_true, y_pred, labels=list(range(n_classes)))
        cm_pct = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis] * 100

        sns.heatmap(cm_pct, annot=True, fmt='.1f', cmap='Blues', ax=ax,
                    xticklabels=labels, yticklabels=labels,
                    cbar_kws={'label': 'Percentage (%)'})
        ax.set_ylabel('真实标签', fontsize=12)
        ax.set_xlabel('预测标签', fontsize=12)
        ax.set_title(f'{name} 混淆矩阵（聚合8受试者）', fontsize=13, fontweight='bold')

    plt.tight_layout()
    path = OUTPUT_DIR / "confusion_matrices.png"
    plt.savefig(path, bbox_inches='tight')
    plt.close()
    print(f"✅ 保存: {path}")
    return path


def plot_subject_bar_chart(conformer, tcn):
    """图4：各受试者Conformer vs TCN分组条形图"""
    if not conformer or not tcn:
        return None

    subjects = [f'S{i}' for i in range(1, 9)]
    conf_accs = [conformer[f'subject_{i}']['mean_accuracy'] * 100 for i in range(1, 9)]
    tcn_accs = [tcn[f'subject_{i}']['mean_accuracy'] * 100 for i in range(1, 9)]

    x = np.arange(len(subjects))
    width = 0.35

    fig, ax = plt.subplots(figsize=(12, 5))
    bars1 = ax.bar(x - width/2, conf_accs, width, label='Conformer', color='#e74c3c', alpha=0.85)
    bars2 = ax.bar(x + width/2, tcn_accs, width, label='TCN', color='#e67e22', alpha=0.85)

    # 数值标注
    for bar in bars1:
        ax.text(bar.get_x() + bar.get_width()/2., bar.get_height() + 0.5,
                f'{bar.get_height():.1f}', ha='center', va='bottom', fontsize=9)
    for bar in bars2:
        ax.text(bar.get_x() + bar.get_width()/2., bar.get_height() + 0.5,
                f'{bar.get_height():.1f}', ha='center', va='bottom', fontsize=9)

    ax.axhline(y=33.3, color='red', linestyle='--', alpha=0.4, label='3类随机基线')
    ax.set_ylabel('Accuracy (%)', fontsize=12)
    ax.set_xlabel('受试者', fontsize=12)
    ax.set_title('Conformer vs TCN 各受试者准确率对比', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(subjects, fontsize=11)
    ax.legend(fontsize=10)
    ax.set_ylim(0, 65)
    ax.grid(axis='y', alpha=0.3)

    plt.tight_layout()
    path = OUTPUT_DIR / "subject_comparison.png"
    plt.savefig(path, bbox_inches='tight')
    plt.close()
    print(f"✅ 保存: {path}")
    return path


def plot_ensemble_comparison(ensemble):
    """图5：集成学习策略对比"""
    if not ensemble:
        return None

    fig, ax = plt.subplots(figsize=(8, 5))
    
    names = []
    ensemble_accs = []
    eegnet_accs = []
    riemann_accs = []
    
    for key, label in [('voting', 'Voting'), ('stacking', 'Stacking'), ('weighted', 'Weighted')]:
        if key in ensemble:
            d = ensemble[key]
            names.append(label)
            ensemble_accs.append(d['ensemble_accuracy'] * 100)
            eegnet_accs.append(d.get('single_model_results', {}).get('eegnet', 0) * 100)
            riemann_accs.append(d.get('single_model_results', {}).get('riemann', 0) * 100)

    x = np.arange(len(names))
    width = 0.25

    bars1 = ax.bar(x - width, eegnet_accs, width, label='EEGNet单模型', color='#3498db')
    bars2 = ax.bar(x, riemann_accs, width, label='Riemann单模型', color='#e67e22')
    bars3 = ax.bar(x + width, ensemble_accs, width, label='集成', color='#1abc9c')

    for bars in [bars1, bars2, bars3]:
        for bar in bars:
            ax.text(bar.get_x() + bar.get_width()/2., bar.get_height() + 0.5,
                    f'{bar.get_height():.1f}%', ha='center', va='bottom', fontsize=9)

    ax.set_ylabel('Accuracy (%)', fontsize=12)
    ax.set_title('集成学习策略对比', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(names, fontsize=11)
    ax.legend(fontsize=10)
    ax.set_ylim(0, 100)
    ax.grid(axis='y', alpha=0.3)

    plt.tight_layout()
    path = OUTPUT_DIR / "ensemble_comparison.png"
    plt.savefig(path, bbox_inches='tight')
    plt.close()
    print(f"✅ 保存: {path}")
    return path


# ─── 主函数 ───
def main():
    print("=" * 60)
    print("BCI Week10 统计分析与可视化")
    print("=" * 60)

    # 加载数据
    print("\n📂 加载实验结果...")
    conformer = load_conformer_results()
    tcn = load_tcn_results()
    ensemble = load_ensemble_results()
    strategy = load_strategy_results()

    loaded = []
    if conformer: loaded.append("Conformer")
    if tcn: loaded.append("TCN")
    if ensemble: loaded.append(f"集成({len(ensemble)}组)")
    if strategy: loaded.append(f"Strategy({','.join(strategy.keys())})")
    print(f"   已加载: {', '.join(loaded)}")

    if not any([conformer, tcn, ensemble, strategy]):
        print("❌ 未找到任何实验结果，请确认outputs目录存在")
        return

    # 统计检验
    print("\n📊 执行统计显著性检验...")
    report = statistical_tests(conformer, tcn, strategy, ensemble)
    report_path = OUTPUT_DIR / "statistical_report.md"
    with open(report_path, 'w', encoding='utf-8') as f:
        f.write(report)
    print(f"✅ 统计报告: {report_path}")

    # 可视化
    print("\n📈 生成可视化图表...")
    plots = {}
    
    p = plot_accuracy_comparison(conformer, tcn, strategy, ensemble)
    if p: plots['accuracy_comparison'] = p

    p = plot_subject_heatmap(conformer, tcn)
    if p: plots['subject_heatmap'] = p

    p = plot_confusion_matrices(conformer, tcn)
    if p: plots['confusion_matrices'] = p

    p = plot_subject_bar_chart(conformer, tcn)
    if p: plots['subject_comparison'] = p

    p = plot_ensemble_comparison(ensemble)
    if p: plots['ensemble_comparison'] = p

    # 汇总
    print(f"\n{'=' * 60}")
    print(f"✅ 全部完成！输出目录: {OUTPUT_DIR}")
    print(f"{'=' * 60}")
    print(f"\n生成文件:")
    print(f"  📄 统计报告: statistical_report.md")
    for name, path in plots.items():
        print(f"  🖼️  {name}: {path.name}")


if __name__ == "__main__":
    main()
