@echo off
echo ========================================
echo Version 1: Interactive Step-by-Step
echo ========================================
echo.

echo Step 1: Compiling...
g++ hospital_interactive.cpp -o interactive.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo Step 2: Running...
echo.
interactive.exe

pause
