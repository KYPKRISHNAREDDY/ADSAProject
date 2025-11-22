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
echo   Starting Live Viewer + Program
echo ========================================
echo.

REM Start the viewer in a separate window
start "Heap Viewer" python viewer.py

REM Wait a moment for viewer to open
timeout /t 2 /nobreak >nul

REM Run the main program
hospital.exe

pause
