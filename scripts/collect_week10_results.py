#!/usr/bin/env python3
"""
Week 10 实验结果汇总脚本
解析实验输出，生成对比表格和结论报告

用法：
    python scripts/collect_week10_results.py
"""

import json
import os
import re
from pathlib import Path
from datetime import datetime

PROJECT_ROOT = Path(__file__).parent.parent
RESULTS_DIR = PROJECT_ROOT / "outputs" / "week10_experiments"
OUTPUT_DIR = RESULTS_DIR / "results_summary"


def parse_accuracy_from_text(text: str) -> float:
    """Try to extract accuracy from text output."""
    patterns = [
        r"Accuracy[:\s]+([0-9.]+)",
        r"accuracy[:\s]+([0-9.]+)",
        r"Test Accuracy[:\s]+([0-9.]+)",
        r"Best accuracy[:\s]+([0-9.]+)",
        r"Val accuracy[:\s]+([0-9.]+)",
    ]
    for pattern in patterns:
        match = re.search(pattern, text, re.IGNORECASE)
        if match:
            return float(match.group(1))
    return -1.0


def load_experiment_results() -> dict:
    """Load results from week10_results.json."""
    results_file = OUTPUT_DIR / "week10_results.json"
    if results_file.exists():
        with open(results_file, 'r') as f:
            return json.load(f)
    return {}


def scan_output_dirs() -> list:
    """Scan all experiment output directories for results."""
    findings = []
    
    for subdir in ["ensemble", "tuning", "advanced", "pipeline"]:
        dir_path = RESULTS_DIR / subdir
        if not dir_path.exists():
            continue
        
        for exp_dir in dir_path.rglob("*"):
            if not exp_dir.is_dir():
                continue
            
            # Look for result files
            for result_file in exp_dir.glob("*.json"):
                try:
                    with open(result_file, 'r') as f:
                        data = json.load(f)
                    findings.append({
                        "path": str(result_file),
                        "experiment": exp_dir.name,
                        "category": subdir,
                        "data": data,
                    })
                except:
                    pass
            
            # Look for log files with accuracy
            for log_file in list(exp_dir.glob("*.log")) + list(exp_dir.glob("*.txt")):
                try:
                    text = log_file.read_text()
                    acc = parse_accuracy_from_text(text)
                    if acc > 0:
                        findings.append({
                            "path": str(log_file),
                            "experiment": exp_dir.name,
                            "category": subdir,
                            "accuracy": acc,
                        })
                except:
                    pass
    
    return findings


def generate_report(results_json: dict, scan_findings: list) -> str:
    """Generate summary report."""
    lines = []
    lines.append("# Week 10 实验结果汇总报告")
    lines.append(f"\n**生成时间：** {datetime.now().strftime('%Y-%m-%d %H:%M')}")
    lines.append("")
    
    # Overall summary
    if results_json:
        lines.append("## 执行概览")
        lines.append(f"- 总实验数: {results_json.get('total_experiments', 'N/A')}")
        lines.append(f"- 成功: {results_json.get('successful', 'N/A')}")
        lines.append(f"- 失败: {results_json.get('failed', 'N/A')}")
        lines.append(f"- 总耗时: {results_json.get('total_duration_sec', 0)/60:.1f}min")
        lines.append("")
    
    # Experiment results table
    lines.append("## 实验结果对比")
    lines.append("")
    lines.append("| # | 实验ID | 方法 | 状态 | 耗时 | 备注 |")
    lines.append("|---|--------|------|------|------|------|")
    
    experiments = results_json.get("experiments", {})
    for i, (exp_id, result) in enumerate(experiments.items(), 1):
        status_icon = "✅" if result.get("status") == "success" else "❌"
        duration = result.get("duration_sec", 0)
        lines.append(
            f"| {i} | {exp_id} | {result.get('name', 'N/A')} | "
            f"{status_icon} {result.get('status', 'N/A')} | "
            f"{duration:.0f}s | |"
        )
    
    # Scan findings
    if scan_findings:
        lines.append("")
        lines.append("## 扫描发现")
        lines.append("")
        for finding in scan_findings:
            if "accuracy" in finding:
                lines.append(f"- **{finding['experiment']}**: accuracy={finding['accuracy']:.4f}")
            elif "data" in finding:
                acc = finding["data"].get("accuracy", finding["data"].get("test_accuracy", "N/A"))
                lines.append(f"- **{finding['experiment']}**: accuracy={acc}")
    
    # Key conclusions
    lines.append("")
    lines.append("## 关键结论")
    lines.append("")
    lines.append("_（待实验完成后填写）_")
    lines.append("")
    lines.append("### 是否达成70%目标？")
    lines.append("- [ ] 是 — 方法: ___")
    lines.append("- [ ] 否 — 差距: ___，下一步: ___")
    lines.append("")
    lines.append("### 最优方法")
    lines.append("- 方法: ___")
    lines.append("- 准确率: ___%")
    lines.append("- 配置: ___")
    
    return "\n".join(lines)


def main():
    results_json = load_experiment_results()
    scan_findings = scan_output_dirs()
    
    report = generate_report(results_json, scan_findings)
    
    # Save report
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    report_path = OUTPUT_DIR / "week10_summary_report.md"
    with open(report_path, 'w', encoding='utf-8') as f:
        f.write(report)
    
    print(report)
    print(f"\n报告已保存到: {report_path}")


if __name__ == "__main__":
    main()
