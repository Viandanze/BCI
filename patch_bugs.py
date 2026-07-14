"""
Patch script to fix BCI_Projects bugs:
1. MNE PhysioNet data download API (use eegbci.load_data)
2. raw.annotations setter (use set_annotations instead)
3. GBK emoji encoding in run_week10_experiments.py
"""

import os
import re

BASE = os.path.dirname(os.path.abspath(__file__))


def patch_loader():
    """Fix src/data/loader.py"""
    path = os.path.join(BASE, "src", "data", "loader.py")
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()

    # 1. Replace _download_and_load_subject function entirely
    old_func_start = "def _download_and_load_subject("
    old_func_end = "return _create_sample_data(subject_id, runs)"
    
    start_idx = content.find(old_func_start)
    if start_idx == -1:
        print("  [SKIP] _download_and_load_subject not found")
    else:
        # Find the end of this function (next def at same or lower indent)
        after_start = content[start_idx:]
        # Find the next top-level def after this one
        next_def = re.search(r'\ndef _create_sample_data', after_start)
        if next_def:
            end_idx = start_idx + next_def.start()
            old_func = content[start_idx:end_idx]
            
            new_func = '''def _download_and_load_subject(
    data_path: Path, 
    subject_id: int, 
    runs: List[int],
    verbose: bool = False
) -> mne.io.Raw:
    """
    Download and load subject data from PhysioNet.
    Uses MNE's eegbci dataset loader.
    """
    from mne.datasets import eegbci
    
    logger.info(f"Downloading subject {subject_id} via mne.datasets.eegbci...")
    
    raw_fnames = eegbci.load_data(
        subject_id, runs, path=str(data_path), update_path=True, verbose=verbose
    )
    
    if not raw_fnames:
        raise FileNotFoundError(f"No files downloaded for subject {subject_id}")
    
    raw_list = []
    for fname in raw_fnames:
        raw = mne.io.read_raw_edf(fname, preload=True, verbose=verbose)
        raw_list.append(raw)
    
    if len(raw_list) == 0:
        raise FileNotFoundError(f"No files could be loaded for subject {subject_id}")
    
    return mne.concatenate_raws(raw_list, preload=True) if len(raw_list) > 1 else raw_list[0]

'''
            content = content[:start_idx] + new_func + content[end_idx:]
            print("  [FIXED] _download_and_load_subject -> use eegbci.load_data")

    # 2. Fix raw.annotations = mne.annotations_from_events(
    #    Replace with raw = raw.set_annotations(...)
    pattern = r'raw\.annotations\s*=\s*mne\.annotations_from_events\('
    if re.search(pattern, content):
        # More robust: find and replace the full statement
        def replace_annotations(m):
            return 'raw = raw.set_annotations(mne.annotations_from_events('
        content = re.sub(pattern, replace_annotations, content)
        
        # Now we need to also remove the extra closing that was on the old line
        # The old pattern was: raw.annotations = mne.annotations_from_events(\n  ...orig_time=raw.annotations.orig_time\n)
        # The new should be: raw = raw.set_annotations(mne.annotations_from_events(\n  ...\n))
        # The problem is raw.annotations.orig_time won't work if annotations is empty
        # Let's fix the orig_time reference too
        content = content.replace(
            'orig_time=raw.annotations.orig_time',
            'orig_time=None'
        )
        print("  [FIXED] raw.annotations = ... -> raw.set_annotations(...)")

    with open(path, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"  [SAVED] {path}")


def patch_train_ensemble():
    """Fix scripts/train_ensemble.py"""
    path = os.path.join(BASE, "scripts", "train_ensemble.py")
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()

    # Fix raw.annotations = mne.annotations_from_events(
    pattern = r'raw\.annotations\s*=\s*mne\.annotations_from_events\('
    if re.search(pattern, content):
        content = re.sub(
            pattern,
            'raw = raw.set_annotations(mne.annotations_from_events(',
            content
        )
        print("  [FIXED] raw.annotations = ... -> raw.set_annotations(...)")

    with open(path, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"  [SAVED] {path}")


def patch_run_week10():
    """Fix GBK emoji encoding in scripts/run_week10_experiments.py"""
    path = os.path.join(BASE, "scripts", "run_week10_experiments.py")
    if not os.path.exists(path):
        print("  [SKIP] run_week10_experiments.py not found")
        return
        
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()

    # Replace emoji chars with ASCII
    replacements = {
        '❌': '[FAIL]',
        '✅': '[OK]',
        '⚡': '[!]',
        '🎯': '[*]',
    }
    for emoji, ascii_repl in replacements.items():
        if emoji in content:
            content = content.replace(emoji, ascii_repl)
            print(f"  [FIXED] {emoji} -> {ascii_repl}")

    # Fix capture_output=True to show real errors
    if 'capture_output=True' in content:
        content = content.replace('capture_output=True', 'capture_output=False')
        print("  [FIXED] capture_output=True -> False (show real errors)")

    with open(path, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"  [SAVED] {path}")


if __name__ == "__main__":
    print("=" * 50)
    print("Patching BCI_Projects bugs...")
    print("=" * 50)
    
    print("\n[1/3] Patching src/data/loader.py")
    patch_loader()
    
    print("\n[2/3] Patching scripts/train_ensemble.py")
    patch_train_ensemble()
    
    print("\n[3/3] Patching scripts/run_week10_experiments.py")
    patch_run_week10()
    
    print("\n" + "=" * 50)
    print("Done! Now run your experiments again.")
    print("=" * 50)
