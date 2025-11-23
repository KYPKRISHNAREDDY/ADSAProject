@echo off
echo ========================================
echo   Hospital Triage System - Version 6
echo   With ML Model Training
echo ========================================
echo.

echo Step 1: Checking if ML model exists...
if not exist trained_model.pkl (
    echo    Model not found. Training now...
    echo.
    python train_model.py
    echo.
    if not exist trained_model.pkl (
        echo ERROR: Model training failed!
        pause
        exit /b 1
    )
    echo Model training complete!
    echo.
) else (
    echo    Model found: trained_model.pkl
)
echo.

echo Step 2: Compiling C++ code...
g++ hospital_triage.cpp -o hospital_triage.exe
if %errorlevel% neq 0 (
    echo Compilation FAILED!
    pause
    exit /b 1
)
echo Compilation successful!
echo.

echo Step 3: Opening viewer in browser...
start viewer.html
timeout /t 2 >nul

echo Step 4: Running program...
echo.
hospital_triage.exe

pause
