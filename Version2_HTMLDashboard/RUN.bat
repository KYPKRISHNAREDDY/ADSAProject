@echo off
echo ========================================
echo Version 2: HTML Dashboard
echo ========================================
echo.

echo Step 1: Compiling...
g++ hospital_html.cpp -o html.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo Step 2: Generating data...
html.exe

echo.
echo Step 3: Creating dashboard...
python create_dashboard.py

echo.
echo Done! Dashboard should open in browser.
echo If not, open: dashboard.html
pause
