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
echo   Starting Hospital Triage System
echo ========================================
echo.
echo TIP: Open heap_state.dot in VS Code with Graphviz Preview
echo      to see the tree structure in real-time!
echo.

hospital.exe

pause
