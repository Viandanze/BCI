@echo off
REM Week 10 实验一键启动脚本 (Windows)
REM 使用方法：双击运行或在cmd中执行

echo ============================================
echo   BCI Week 10 实验启动器
echo ============================================
echo.

REM 切换到项目目录（根据你的实际路径修改）
cd /d C:\Users\17600\BCI_Projects

echo [1/3] 检查Python环境...
python -c "import torch; print(f'  PyTorch: {torch.__version__}, CUDA: {torch.cuda.is_available()}')" 2>nul
if errorlevel 1 (
    echo   [ERROR] PyTorch not installed!
    pause
    exit /b 1
)

python -c "import mne; print(f'  MNE: {mne.__version__}')" 2>nul
if errorlevel 1 (
    echo   [ERROR] MNE not installed!
    pause
    exit /b 1
)

python -c "import pyriemann; print(f'  pyRiemann: {pyriemann.__version__}')" 2>nul
if errorlevel 1 (
    echo   [ERROR] pyRiemann not installed!
    pause
    exit /b 1
)

echo.
echo [2/3] 检查数据...
if not exist "data\" (
    echo   [WARN] data/ directory not found!
    echo   Run data download first: python -c "from src.data.loader import download_physionet; download_physionet('./data/')"
)

echo.
echo [3/3] 选择实验模式:
echo   1. 运行全部实验 (P0+P1, ~3-4h)
echo   2. 仅运行P0实验 (~2h)
echo   3. 仅集成学习实验 (~1.5h)
echo   4. 仅EEGNet调优 (~2h)
echo   5. 仅高级模型+Pipeline (~0.5h)
echo   6. 预览实验计划 (dry-run)
echo.

set /p choice="输入选择 (1-6): "

if "%choice%"=="1" python scripts/run_week10_experiments.py
if "%choice%"=="2" python scripts/run_week10_experiments.py --only p0
if "%choice%"=="3" python scripts/run_week10_experiments.py --only ensemble
if "%choice%"=="4" python scripts/run_week10_experiments.py --only tuning
if "%choice%"=="5" python scripts/run_week10_experiments.py --only advanced
if "%choice%"=="6" python scripts/run_week10_experiments.py --dry-run

echo.
echo ============================================
echo   实验执行完毕！查看结果:
echo   outputs\week10_experiments\results_summary\
echo ============================================
pause
