# 🪟 Windows Setup Guide for Hospital Triage System

## Quick Start (Recommended: WSL)

### Method 1: WSL (Windows Subsystem for Linux) - EASIEST

**Step 1: Install WSL**
```powershell
# Open PowerShell as Administrator and run:
wsl --install
```

**Step 2: Restart Computer**

**Step 3: Open Ubuntu**
- Find "Ubuntu" in Start Menu
- First time setup: create username/password

**Step 4: Install Dependencies**
```bash
sudo apt-get update
sudo apt-get install g++ make python3 python3-pip -y
pip3 install numpy scikit-learn
```

**Step 5: Navigate to Project**
```bash
# Windows D: drive is at /mnt/d/
cd /mnt/d/Temppp/ADSAProject-claude-fibonacci-heap-hospital-triage-01VuSXe4Z2xGUXBwpSYFbR5G

# Or C: drive is at /mnt/c/
# cd /mnt/c/Users/YourName/...
```

**Step 6: Build and Run**
```bash
make
./hospital_triage
```

---

## Method 2: MSYS2 (Native Windows)

**Step 1: Install MSYS2**
1. Download: https://www.msys2.org/
2. Run installer
3. Install to `C:\msys64`

**Step 2: Open MSYS2 MINGW64 Terminal**
- Find "MSYS2 MINGW64" in Start Menu

**Step 3: Install Tools**
```bash
pacman -Syu  # Update package database
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-python mingw-w64-x86_64-python-pip make
pip install numpy scikit-learn
```

**Step 4: Navigate to Project**
```bash
# In MSYS2, D: drive is /d/, C: is /c/
cd /d/Temppp/ADSAProject-claude-fibonacci-heap-hospital-triage-01VuSXe4Z2xGUXBwpSYFbR5G
```

**Step 5: Build and Run**
```bash
make
./hospital_triage.exe
```

---

## Method 3: Using Batch Files (After Installing Compiler)

**After installing MSYS2 or MinGW-w64:**

1. **Add to PATH** (Important!)
   - For MSYS2: Add `C:\msys64\mingw64\bin` to Windows PATH
   - For MinGW: Add `C:\mingw64\bin` to Windows PATH

2. **Double-click these files:**
   - `build_windows.bat` - Builds the project
   - `run_windows.bat` - Runs the project

---

## Method 4: Visual Studio (Advanced)

**Step 1: Install Visual Studio 2022**
- Download: https://visualstudio.microsoft.com/
- Select "Desktop development with C++"

**Step 2: Install Python**
- Download: https://www.python.org/
- Check "Add Python to PATH"

**Step 3: Open Developer Command Prompt**
```cmd
cd D:\Temppp\ADSAProject-claude-fibonacci-heap-hospital-triage-01VuSXe4Z2xGUXBwpSYFbR5G

pip install numpy scikit-learn

cl /std:c++17 /EHsc HospitalTriageSystem.cpp /Fe:hospital_triage.exe

hospital_triage.exe
```

---

## Troubleshooting

### "Command not found" or "g++ not recognized"
- ✅ Use WSL (Method 1) - easiest solution
- ✅ Or install MSYS2 and add to PATH

### "Python not found"
```bash
# In WSL:
sudo apt-get install python3 python3-pip

# In Windows:
# Download from https://www.python.org/
```

### "numpy not found" or "sklearn not found"
```bash
# In WSL or MSYS2:
pip3 install numpy scikit-learn

# In Windows PowerShell:
pip install numpy scikit-learn
```

### "Permission denied"
```bash
# In WSL/MSYS2:
chmod +x hospital_triage
./hospital_triage

# Or:
bash hospital_triage
```

### Terminal colors not showing
- Use Windows Terminal (recommended)
- Download from Microsoft Store
- Or use WSL default terminal

---

## Recommended Setup for Presentation

**Best for Demo:**
1. **Install WSL** (5 minutes)
2. **Install dependencies** (2 minutes)
3. **Run project** (instant)

This gives you:
- ✅ Full color support
- ✅ All features working
- ✅ Easy to use
- ✅ Looks professional

---

## Quick Commands Reference

### WSL/MSYS2:
```bash
# Build
make

# Run
./hospital_triage

# Test
./test_system.sh

# Clean
make clean
```

### Windows (after adding to PATH):
```cmd
REM Build
build_windows.bat

REM Run
run_windows.bat
```

---

## What Works on Windows?

✅ All features work if using WSL or MSYS2
✅ Fibonacci Heap implementation
✅ ML model (Python)
✅ Dynamic visualization
✅ Color-coded terminal output
✅ File loading
✅ All algorithms

❌ Pure Windows CMD has limited color support (use Windows Terminal or WSL)

---

## For Your Presentation

**Recommended: Use WSL**
1. Install WSL once (5 min)
2. Works perfectly every time
3. Professional Linux-like terminal
4. Full color support
5. Your guide will be impressed!

**Command to show:**
```bash
cd /mnt/d/Temppp/ADSAProject-claude-fibonacci-heap-hospital-triage-01VuSXe4Z2xGUXBwpSYFbR5G
make
./hospital_triage
```

---

## Need Help?

1. **Quick Start**: Use WSL (Method 1)
2. **Issues**: Check Troubleshooting section
3. **Alternative**: Use MSYS2 (Method 2)

Good luck! 🚀
