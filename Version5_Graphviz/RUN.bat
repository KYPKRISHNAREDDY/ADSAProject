@echo off
echo ========================================
echo   Compiling Hospital Triage System...
echo ========================================

g++ hospital_graphviz.cpp -o hospital.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Compilation failed! Make sure g++ is installed.
    pause
    exit /b 1
)

echo Compilation successful!
echo.
echo ========================================
echo   Starting System
echo ========================================
echo.
echo Step 1: Starting browser viewer...
start "" python viewer.py
echo.
echo Waiting for browser to open...
timeout /t 3 /nobreak >nul
echo.
echo Step 2: Running hospital system...
echo.
hospital.exe

pause
