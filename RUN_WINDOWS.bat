@echo off
echo ========================================
echo Hospital Triage System (Windows)
echo ========================================
echo.

REM Check if executable exists
if not exist hospital_triage_windows.exe (
    echo ERROR: hospital_triage_windows.exe not found!
    echo Please run BUILD_WINDOWS.bat first
    echo.
    pause
    exit /b 1
)

REM Check Python
where python >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Python not found!
    echo Please install Python from: https://www.python.org/
    echo Make sure to check "Add Python to PATH" during installation
    echo.
    pause
    exit /b 1
)

REM Check Python dependencies
echo Checking Python dependencies...
python -c "import numpy, sklearn" 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Installing Python dependencies...
    python -m pip install numpy scikit-learn
    if %ERRORLEVEL% NEQ 0 (
        echo Failed to install dependencies
        echo Please run: pip install numpy scikit-learn
        pause
        exit /b 1
    )
)

REM Run the program
echo.
echo Starting Hospital Triage System...
echo.
hospital_triage_windows.exe

echo.
pause
