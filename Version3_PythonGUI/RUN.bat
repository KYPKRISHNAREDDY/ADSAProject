@echo off
echo ========================================
echo Version 3: Python GUI Window
echo ========================================
echo.

echo Step 1: Compiling...
g++ hospital_gui_data.cpp -o gui_data.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo Step 2: Generating data...
gui_data.exe

echo.
echo Step 3: Launching GUI...
python gui_visualizer.py

pause
