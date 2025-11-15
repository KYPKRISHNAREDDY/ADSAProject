@echo off
REM Run script for Windows

echo ========================================
echo Hospital Triage System
echo ========================================
echo.

REM Check if executable exists
if not exist hospital_triage.exe (
    echo ERROR: hospital_triage.exe not found!
    echo Please run build_windows.bat first
    pause
    exit /b 1
)

REM Check Python dependencies
echo Checking Python dependencies...
python -c "import numpy, sklearn" 2>nul
if %ERRORLEVEL% NEQ 0 (
    python3 -c "import numpy, sklearn" 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo Installing Python dependencies...
        pip install numpy scikit-learn
        if %ERRORLEVEL% NEQ 0 (
            pip3 install numpy scikit-learn
        )
    )
)

echo.
echo Running Hospital Triage System...
echo.

hospital_triage.exe

pause
