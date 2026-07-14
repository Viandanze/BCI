"""Fix BCI_Projects bugs - save this as D:\\BCI_Projects\\fix_bugs.py and run it"""
import os, re

B = os.path.dirname(os.path.abspath(__file__))

# ============================================================
# 1. Fix src/data/loader.py - download API + annotations setter
# ============================================================
p = os.path.join(B, "src", "data", "loader.py")
with open(p, "r", encoding="utf-8") as f:
    c = f.read()

# Replace _download_and_load_subject function
old_start = "def _download_and_load_subject("
old_end_marker = "\ndef _create_sample_data("
s = c.find(old_start)
e = c.find(old_end_marker, s)
if s > 0 and e > 0:
    new_func = '''def _download_and_load_subject(
    data_path: Path,
    subject_id: int,
    runs: List[int],
    verbose: bool = False
) -> mne.io.Raw:
    """Download from PhysioNet via mne.datasets.eegbci."""
    from mne.datasets import eegbci
    logger.info(f"Downloading subject {subject_id} via eegbci...")
    raw_fnames = eegbci.load_data(
        subject_id, runs, path=str(data_path), update_path=True, verbose=verbose
    )
    if not raw_fnames:
        raise FileNotFoundError(f"No files for subject {subject_id}")
    raw_list = [mne.io.read_raw_edf(f, preload=True, verbose=verbose) for f in raw_fnames]
    if not raw_list:
        raise FileNotFoundError(f"No files loaded for subject {subject_id}")
    return mne.concatenate_raws(raw_list, preload=True) if len(raw_list) > 1 else raw_list[0]

'''
    c = c[:s] + new_func + c[e:]
    print("[OK] loader.py: _download_and_load_subject replaced with eegbci.load_data")
else:
    print("[SKIP] loader.py: could not find _download_and_load_subject")

# Fix raw.annotations = ... -> raw = raw.set_annotations(...)
c = re.sub(
    r'raw\.annotations\s*=\s*mne\.annotations_from_events\(',
    'raw = raw.set_annotations(mne.annotations_from_events(',
    c
)
c = c.replace('orig_time=raw.annotations.orig_time', 'orig_time=None')
print("[OK] loader.py: annotations setter fixed")

with open(p, "w", encoding="utf-8") as f:
    f.write(c)

# ============================================================
# 2. Fix scripts/train_ensemble.py - annotations setter
# ============================================================
p = os.path.join(B, "scripts", "train_ensemble.py")
with open(p, "r", encoding="utf-8") as f:
    c = f.read()

c = re.sub(
    r'raw\.annotations\s*=\s*mne\.annotations_from_events\(',
    'raw = raw.set_annotations(mne.annotations_from_events(',
    c
)
print("[OK] train_ensemble.py: annotations setter fixed")

with open(p, "w", encoding="utf-8") as f:
    f.write(c)

# ============================================================
# 3. Fix scripts/run_week10_experiments.py - emoji + capture_output
# ============================================================
p = os.path.join(B, "scripts", "run_week10_experiments.py")
if os.path.exists(p):
    with open(p, "r", encoding="utf-8") as f:
        c = f.read()
    for em, tx in [("\u274c", "[FAIL]"), ("\u2705", "[OK]"), ("\u26a1", "[!]"), ("\U0001f3af", "[*]")]:
        if em in c:
            c = c.replace(em, tx)
            print(f"[OK] run_week10: {em} -> {tx}")
    c = c.replace("capture_output=True", "capture_output=False")
    print("[OK] run_week10: capture_output=True -> False")
    with open(p, "w", encoding="utf-8") as f:
        f.write(c)

print("\nAll patches applied! Now run:")
print("  python scripts/train_ensemble.py --strategy voting --models eegnet riemann --voting_mode soft --subjects 1 2 3 4 5 6 7 8 --output_dir outputs/week10_experiments/ensemble/voting_soft")
