@echo off
echo ========================================
echo Building Hospital Triage System (Windows)
echo ========================================
echo.

REM Check for g++ (MinGW)
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using MinGW g++ compiler...
    g++ -std=c++17 -O2 HospitalTriageSystem_Windows.cpp -o hospital_triage_windows.exe
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo BUILD SUCCESSFUL!
        echo Executable: hospital_triage_windows.exe
        echo.
        echo Run with: RUN_WINDOWS.bat
        echo.
        goto :end
    ) else (
        echo Build failed with g++
        goto :error
    )
)

REM Check for cl (Visual Studio)
where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using Visual Studio compiler...
    cl /std:c++17 /EHsc /O2 HospitalTriageSystem_Windows.cpp /Fe:hospital_triage_windows.exe
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo BUILD SUCCESSFUL!
        echo Executable: hospital_triage_windows.exe
        echo.
        echo Run with: RUN_WINDOWS.bat
        echo.
        goto :end
    ) else (
        echo Build failed with cl
        goto :error
    )
)

:error
echo.
echo ========================================
echo ERROR: No C++ compiler found!
echo ========================================
echo.
echo Please install one of the following:
echo.
echo Option 1 - MinGW-w64 (Recommended):
echo   Download from: https://winlibs.com/
echo   Extract and add bin folder to PATH
echo.
echo Option 2 - Visual Studio:
echo   Download from: https://visualstudio.microsoft.com/
echo   Install "Desktop development with C++"
echo.
echo Option 3 - TDM-GCC:
echo   Download from: https://jmeubank.github.io/tdm-gcc/
echo.
pause
exit /b 1

:end
pause
