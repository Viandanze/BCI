#!/usr/bin/env python3
"""
Week 10 一站式实验运行脚本
自动执行全部P0+P1实验，收集结果，生成汇总报告

用法：
    python scripts/run_week10_experiments.py                    # 运行全部实验
    python scripts/run_week10_experiments.py --only p0           # 仅运行P0实验
    python scripts/run_week10_experiments.py --only ensemble     # 仅运行集成学习实验
    python scripts/run_week10_experiments.py --dry-run           # 预览实验计划不执行

实验清单：
    P0-1: 集成学习 - Voting (soft) EEGNet+pyRiemann
    P0-2: 集成学习 - Stacking (Logistic meta-learner)
    P0-3: 集成学习 - Weighted (validation-based)
    P0-4: EEGNet调优 - 数据增强组合
    P0-5: EEGNet调优 - 超参搜索
    P0-6: EEGNet调优 - 架构改进 (BatchNorm+LabelSmoothing)
    P1-1: Conformer baseline
    P1-2: TCN baseline
    P1-3: 实时Pipeline测试
"""

import argparse
import json
import logging
import os
import subprocess
import sys
import time
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('./outputs/week10_experiments/experiment_run.log'),
    ]
)
logger = logging.getLogger(__name__)

PROJECT_ROOT = Path(__file__).parent.parent
SCRIPTS_DIR = PROJECT_ROOT / "scripts"
OUTPUT_BASE = PROJECT_ROOT / "outputs" / "week10_experiments"


# ============================================================================
# Experiment Definitions
# ============================================================================

EXPERIMENTS = {
    # === P0 Experiments ===
    "p0-1_ensemble_voting": {
        "priority": "P0",
        "name": "集成学习 - Voting (soft) EEGNet+pyRiemann",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "train_ensemble.py"),
            "--strategy", "voting",
            "--models", "eegnet", "riemann",
            "--voting_mode", "soft",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "ensemble" / "voting_soft"),
        ],
        "timeout": 1800,  # 30 min
        "success_metric": "accuracy >= 0.70",
    },
    "p0-2_ensemble_stacking": {
        "priority": "P0",
        "name": "集成学习 - Stacking (Logistic meta-learner)",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "train_ensemble.py"),
            "--strategy", "stacking",
            "--models", "eegnet", "csp", "riemann",
            "--meta_learner", "logistic",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "ensemble" / "stacking_logistic"),
        ],
        "timeout": 2400,  # 40 min (CV is slower)
        "success_metric": "accuracy >= 0.70",
    },
    "p0-3_ensemble_weighted": {
        "priority": "P0",
        "name": "集成学习 - Weighted (validation-based)",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "train_ensemble.py"),
            "--strategy", "weighted",
            "--models", "eegnet", "csp", "riemann",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "ensemble" / "weighted"),
        ],
        "timeout": 1800,
        "success_metric": "accuracy >= 0.70",
    },
    "p0-4_tuning_augmentation": {
        "priority": "P0",
        "name": "EEGNet调优 - 数据增强组合 (Strategy A)",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "tune_eegnet.py"),
            "--strategy", "A",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "tuning" / "augmentation"),
        ],
        "timeout": 3600,  # 60 min
        "success_metric": "best_accuracy >= 0.70",
    },
    "p0-5_tuning_hyperparam": {
        "priority": "P0",
        "name": "EEGNet调优 - 超参搜索 (Strategy B)",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "tune_eegnet.py"),
            "--strategy", "B",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "tuning" / "hyperparam"),
        ],
        "timeout": 3600,
        "success_metric": "best_accuracy >= 0.70",
    },
    "p0-6_tuning_architecture": {
        "priority": "P0",
        "name": "EEGNet调优 - 架构改进 (BatchNorm+LabelSmoothing)",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "tune_eegnet.py"),
            "--strategy", "C",
            "--improvements", "baseline", "batchnorm", "label_smoothing", "se_attention", "combined",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "tuning" / "architecture"),
        ],
        "timeout": 1800,
        "success_metric": "accuracy >= 0.70",
    },
    # === P1 Experiments ===
    "p1-1_conformer": {
        "priority": "P1",
        "name": "Conformer baseline",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "train_advanced.py"),
            "--model", "conformer",
            "--epochs", "150",
            "--batch_size", "32",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "advanced" / "conformer"),
        ],
        "timeout": 2400,
        "success_metric": "accuracy >= 0.65",
    },
    "p1-2_tcn": {
        "priority": "P1",
        "name": "TCN baseline",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "train_advanced.py"),
            "--model", "tcn",
            "--epochs", "150",
            "--batch_size", "32",
            "--subjects", "1", "2", "3", "4", "5", "6", "7", "8",
            "--output_dir", str(OUTPUT_BASE / "advanced" / "tcn"),
        ],
        "timeout": 2400,
        "success_metric": "accuracy >= 0.65",
    },
    "p1-3_pipeline_test": {
        "priority": "P1",
        "name": "实时Pipeline测试",
        "cmd": [
            sys.executable, str(SCRIPTS_DIR / "realtime_demo.py"),
            "--mock",
            "--duration", "120",
            "--output_dir", str(OUTPUT_BASE / "pipeline"),
        ],
        "timeout": 300,
        "success_metric": "pipeline runs without error",
    },
}


# ============================================================================
# Runner
# ============================================================================

class ExperimentRunner:
    """Run experiments sequentially with logging and result collection."""
    
    def __init__(self, dry_run: bool = False):
        self.dry_run = dry_run
        self.results: Dict[str, Dict] = {}
        self.start_time = None
    
    def run_single(self, exp_id: str, exp_config: Dict) -> Dict:
        """Run a single experiment."""
        result = {
            "id": exp_id,
            "name": exp_config["name"],
            "priority": exp_config["priority"],
            "status": "pending",
            "start_time": None,
            "end_time": None,
            "duration_sec": 0,
            "output": "",
            "error": "",
            "return_code": None,
        }
        
        logger.info(f"\n{'='*70}")
        logger.info(f"[{exp_config['priority']}] {exp_config['name']}")
        logger.info(f"Command: {' '.join(exp_config['cmd'])}")
        logger.info(f"{'='*70}")
        
        if self.dry_run:
            result["status"] = "dry_run"
            logger.info("[DRY RUN] Skipped execution")
            return result
        
        result["start_time"] = datetime.now().isoformat()
        start = time.time()
        
        try:
            proc = subprocess.run(
                exp_config["cmd"],
                capture_output=False,
                text=True,
                timeout=exp_config.get("timeout", 3600),
                cwd=str(PROJECT_ROOT),
            )
            
            result["return_code"] = proc.returncode
            result["output"] = proc.stdout[-5000:] if len(proc.stdout) > 5000 else proc.stdout
            result["error"] = proc.stderr[-3000:] if len(proc.stderr) > 3000 else proc.stderr
            result["status"] = "success" if proc.returncode == 0 else "failed"
            
        except subprocess.TimeoutExpired:
            result["status"] = "timeout"
            result["error"] = f"Experiment timed out after {exp_config.get('timeout', 3600)}s"
        except Exception as e:
            result["status"] = "error"
            result["error"] = str(e)
        
        result["end_time"] = datetime.now().isoformat()
        result["duration_sec"] = round(time.time() - start, 1)
        
        status_icon = "[OK]" if result["status"] == "success" else "[FAIL]"
        logger.info(f"{status_icon} {exp_id}: {result['status']} ({result['duration_sec']}s)")
        
        return result
    
    def run_all(self, experiments: Dict[str, Dict]) -> Dict:
        """Run all specified experiments sequentially."""
        self.start_time = time.time()
        
        # Sort: P0 first, then P1
        sorted_exps = sorted(
            experiments.items(),
            key=lambda x: (0 if x[1]["priority"] == "P0" else 1, x[0])
        )
        
        for exp_id, exp_config in sorted_exps:
            self.results[exp_id] = self.run_single(exp_id, exp_config)
        
        # Save results
        self._save_results()
        
        return self.results
    
    def _save_results(self):
        """Save experiment results to JSON."""
        results_path = OUTPUT_BASE / "results_summary" / "week10_results.json"
        results_path.parent.mkdir(parents=True, exist_ok=True)
        
        summary = {
            "run_time": datetime.now().isoformat(),
            "total_duration_sec": round(time.time() - self.start_time, 1),
            "total_experiments": len(self.results),
            "successful": sum(1 for r in self.results.values() if r["status"] == "success"),
            "failed": sum(1 for r in self.results.values() if r["status"] != "success"),
            "experiments": self.results,
        }
        
        with open(results_path, 'w') as f:
            json.dump(summary, f, indent=2, ensure_ascii=False)
        
        logger.info(f"\nResults saved to {results_path}")
        self._print_summary(summary)
    
    def _print_summary(self, summary: Dict):
        """Print experiment summary."""
        print("\n" + "=" * 70)
        print("  WEEK 10 EXPERIMENT RESULTS SUMMARY")
        print("=" * 70)
        print(f"  Total: {summary['total_experiments']} | "
              f"[OK] Success: {summary['successful']} | "
              f"[FAIL] Failed: {summary['failed']}")
        print(f"  Total Duration: {summary['total_duration_sec']:.0f}s "
              f"({summary['total_duration_sec']/60:.1f}min)")
        print("-" * 70)
        
        for exp_id, result in self.results.items():
            icon = "[OK]" if result["status"] == "success" else "[FAIL]"
            print(f"  {icon} [{result['priority']}] {result['name']}")
            print(f"     Status: {result['status']} | Duration: {result['duration_sec']}s")
        
        print("=" * 70)


# ============================================================================
# Main
# ============================================================================

def main():
    parser = argparse.ArgumentParser(description="Week 10 Experiment Runner")
    parser.add_argument("--only", type=str, default=None,
                       choices=["p0", "p1", "ensemble", "tuning", "advanced", "pipeline"],
                       help="Only run specific experiment group")
    parser.add_argument("--dry-run", action="store_true",
                       help="Preview experiments without running")
    args = parser.parse_args()
    
    # Filter experiments
    exps = EXPERIMENTS.copy()
    
    if args.only == "p0":
        exps = {k: v for k, v in exps.items() if v["priority"] == "P0"}
    elif args.only == "p1":
        exps = {k: v for k, v in exps.items() if v["priority"] == "P1"}
    elif args.only == "ensemble":
        exps = {k: v for k, v in exps.items() if "ensemble" in k}
    elif args.only == "tuning":
        exps = {k: v for k, v in exps.items() if "tuning" in k}
    elif args.only == "advanced":
        exps = {k: v for k, v in exps.items() if k.startswith("p1-1") or k.startswith("p1-2")}
    elif args.only == "pipeline":
        exps = {k: v for k, v in exps.items() if "pipeline" in k}
    
    logger.info(f"Week 10 Experiment Runner")
    logger.info(f"Experiments to run: {len(exps)}")
    logger.info(f"Dry run: {args.dry_run}")
    
    if args.dry_run:
        for exp_id, exp_config in exps.items():
            print(f"  [{exp_config['priority']}] {exp_config['name']}")
            print(f"    Command: {' '.join(exp_config['cmd'])}")
            print(f"    Timeout: {exp_config.get('timeout', 3600)}s")
            print()
        return
    
    runner = ExperimentRunner(dry_run=args.dry_run)
    results = runner.run_all(exps)
    
    # Return non-zero if any P0 experiment failed
    p0_failed = any(
        r["status"] != "success"
        for k, r in results.items()
        if EXPERIMENTS[k]["priority"] == "P0"
    )
    
    if p0_failed:
        logger.warning("Some P0 experiments failed! Check logs for details.")
        sys.exit(1)
    else:
        logger.info("All experiments completed successfully! 🎉")
        sys.exit(0)


if __name__ == "__main__":
    main()
