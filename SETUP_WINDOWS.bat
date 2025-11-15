@echo off
echo ========================================
echo Hospital Triage System - Windows Setup
echo ========================================
echo.

echo Step 1: Checking Python...
where python >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] Python found
    python --version
) else (
    echo [ERROR] Python not found!
    echo.
    echo Please download and install Python:
    echo https://www.python.org/downloads/
    echo.
    echo IMPORTANT: Check "Add Python to PATH" during installation!
    echo.
    pause
    exit /b 1
)

echo.
echo Step 2: Installing Python dependencies...
python -m pip install --upgrade pip
python -m pip install numpy scikit-learn
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to install Python packages
    pause
    exit /b 1
) else (
    echo [OK] Python packages installed
)

echo.
echo Step 3: Checking C++ compiler...
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] g++ compiler found
    g++ --version
    goto :build
)

where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] Visual Studio compiler found
    goto :build
)

echo [ERROR] No C++ compiler found!
echo.
echo Please install MinGW-w64:
echo 1. Download from: https://winlibs.com/
echo 2. Extract to C:\mingw64
echo 3. Add C:\mingw64\bin to PATH
echo.
echo OR install Visual Studio:
echo https://visualstudio.microsoft.com/
echo.
pause
exit /b 1

:build
echo.
echo Step 4: Building project...
call BUILD_WINDOWS.bat

echo.
echo ========================================
echo SETUP COMPLETE!
echo ========================================
echo.
echo To run the system: RUN_WINDOWS.bat
echo.
pause
