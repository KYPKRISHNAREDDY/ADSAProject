@echo off
REM Build script for Windows using MinGW/MSYS2 or WSL

echo ========================================
echo Building Hospital Triage System
echo ========================================
echo.

REM Try to detect compiler
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: g++ not found!
    echo.
    echo Please install one of the following:
    echo 1. WSL: wsl --install
    echo 2. MSYS2: https://www.msys2.org/
    echo 3. MinGW-w64: https://www.mingw-w64.org/
    echo.
    pause
    exit /b 1
)

REM Check Python
where python >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    where python3 >nul 2>&1
    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: Python not found!
        echo Please install Python from: https://www.python.org/
        pause
        exit /b 1
    )
)

echo Compiling C++ code...
g++ -std=c++17 -Wall -Wextra -O2 HospitalTriageSystem.cpp -o hospital_triage.exe
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build successful!
echo.
echo To run: hospital_triage.exe
echo.
pause
