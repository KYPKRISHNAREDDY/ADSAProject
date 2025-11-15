@echo off
echo ========================================
echo Version 4: Professional Graphs
echo ========================================
echo.

echo Step 1: Compiling...
g++ hospital_graphs.cpp -o graphs.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo Step 2: Running simulation...
graphs.exe

echo.
echo Step 3: Generating graphs...
python generate_graphs.py

echo.
echo Step 4: Opening report...
cd graphs
start REPORT.html
cd ..

echo.
echo Done! Check the graphs folder.
pause
