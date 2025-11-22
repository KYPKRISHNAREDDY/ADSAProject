@echo off
echo ========================================
echo   Hospital Triage System - Version 6
echo ========================================
echo.

echo Step 1: Compiling...
g++ hospital_triage.cpp -o hospital_triage.exe
if %errorlevel% neq 0 (
    echo Compilation FAILED!
    pause
    exit /b 1
)
echo Compilation successful!
echo.

echo Step 2: Opening viewer in browser...
start viewer.html
timeout /t 2 >nul

echo Step 3: Running program...
echo.
hospital_triage.exe

pause
