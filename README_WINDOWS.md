# 🪟 Hospital Triage System - Windows Edition

## ✅ Pure Windows Version - No Ubuntu/WSL Needed!

This is the **Windows-native version** that runs directly on Windows without WSL or Ubuntu.

---

## 📋 Requirements

### 1. Python (Required)
**Download:** https://www.python.org/downloads/

**IMPORTANT:** During installation, check ✅ **"Add Python to PATH"**

### 2. C++ Compiler (Choose ONE)

#### Option A: MinGW-w64 (Recommended - Easiest)
1. **Download:** https://winlibs.com/
2. Click: **Download** → Choose latest version with UCRT runtime
3. **Extract** to `C:\mingw64`
4. **Add to PATH:**
   - Press `Windows Key`, search "Environment Variables"
   - Click "Environment Variables"
   - Under "System variables", find "Path"
   - Click "Edit" → "New"
   - Add: `C:\mingw64\bin`
   - Click OK

#### Option B: Visual Studio
1. **Download:** https://visualstudio.microsoft.com/downloads/
2. Install **Community Edition** (free)
3. Select: **"Desktop development with C++"**

---

## 🚀 Quick Start (3 Steps)

### Method 1: Automatic Setup (Easiest)

**Just double-click:**
```
SETUP_WINDOWS.bat
```
This will:
- Check Python installation
- Install Python packages (numpy, scikit-learn)
- Check C++ compiler
- Build the project automatically

Then run:
```
RUN_WINDOWS.bat
```

### Method 2: Manual Setup

**Step 1: Install Python packages**
```cmd
pip install numpy scikit-learn
```

**Step 2: Build the project**
Double-click: `BUILD_WINDOWS.bat`

**Step 3: Run the project**
Double-click: `RUN_WINDOWS.bat`

---

## 📁 Windows Files

- `HospitalTriageSystem_Windows.cpp` - Windows-native C++ code
- `SETUP_WINDOWS.bat` - Automatic setup
- `BUILD_WINDOWS.bat` - Build the project
- `RUN_WINDOWS.bat` - Run the system
- `FibonacciHeap.h` - Fibonacci Heap implementation (works on Windows)
- `triage_ml_model.py` - ML model (works on Windows)
- `patients_data.csv` - Patient data
- `doctors_data.csv` - Doctor data

---

## 🎯 What You'll See

```
========================================================================
              SMART HOSPITAL TRIAGE SYSTEM
          Real-time ML-Based Patient Allocation
          Using Fibonacci Heap Data Structure
========================================================================

FIBONACCI HEAP PROPERTIES:
  - Insert Operation:      O(1) amortized
  - Find Maximum:          O(1)
  - Extract Maximum:       O(log n) amortized
  - Increase Key:          O(1) amortized

Loading system data...
Loaded 4 doctors from file
Loaded 30 patients from file

========================================
Starting ML-based Triage Scoring...
========================================

Patient 1 (Rajesh Kumar) - ML Score: 95.3
Patient 2 (Priya Sharma) - ML Score: 82.1
...

========================================
All patients scored and added to queues!
========================================

========================================
ITERATION #1
========================================

DOCTOR STATUS:
----------------------------------------
Dr. Arvind Mehta (Cardiology) - BUSY
  Treating: Rajesh Kumar | Priority: 95.3 | Time Left: 3s
Dr. Sneha Kapoor (Cardiology) - BUSY
  Treating: Sunita Reddy | Priority: 96.8 | Time Left: 3s
...

WAITING QUEUES:
----------------------------------------
Cardiology Department (8 waiting):
  1. Harish Kumar (Priority: 97.5)
  2. Radha Krishnan (Priority: 96.2)
  ...

STATISTICS:
----------------------------------------
Total Patients: 30
Patients Treated: 0
Patients Waiting: 28
Critical Cases: 8
High Priority Cases: 10

[Updates every 2 seconds until all patients treated]

========================================
  ALL PATIENTS TREATED SUCCESSFULLY!
========================================
```

---

## 🔧 Troubleshooting

### "Python is not recognized"
**Fix:**
1. Uninstall Python
2. Reinstall from https://www.python.org/
3. ✅ **CHECK "Add Python to PATH"**
4. Restart Command Prompt

### "g++ is not recognized" or "No compiler found"
**Fix:**
1. Download MinGW from: https://winlibs.com/
2. Extract to `C:\mingw64`
3. Add `C:\mingw64\bin` to PATH (see instructions above)
4. **Restart Command Prompt**

### "numpy not found" or "sklearn not found"
**Fix:**
```cmd
pip install numpy scikit-learn
```

If that doesn't work:
```cmd
python -m pip install numpy scikit-learn
```

### "Cannot open file 'patients_data.csv'"
**Fix:**
- Make sure you're in the correct folder
- Right-click `RUN_WINDOWS.bat` → Edit
- Check the file paths

### Build errors
**Fix:**
1. Make sure you have a C++ compiler installed
2. Try running from **Command Prompt as Administrator**
3. Check that all `.h` files are in the same folder

---

## 🎨 Features

✅ **Fibonacci Heap** - Advanced priority queue
✅ **Machine Learning** - Random Forest for triage scoring
✅ **Color-coded output** - Red (Critical), Yellow (High), etc.
✅ **Real-time simulation** - Updates every 2 seconds
✅ **Dynamic allocation** - Highest priority first
✅ **File-based data** - Easy to modify patients/doctors

---

## 📊 Key Differences from Linux Version

| Feature | Linux Version | Windows Version |
|---------|--------------|-----------------|
| Compiler | g++ (native) | g++ (MinGW) or MSVC |
| Colors | ANSI codes | Windows Console API |
| Python call | `popen()` | `system()` + temp file |
| Clear screen | `\033[2J` | `system("cls")` |
| Sleep | `sleep()` | `Sleep()` |

**Everything else is the same!**

---

## 💡 Testing Individual Components

### Test Python ML Model:
```cmd
python triage_ml_model.py 70 1 4 1 190 270 35.0 175 1
```
**Expected output:** ~100 (Critical case)

```cmd
python triage_ml_model.py 25 1 0 0 120 150 23.0 130 0
```
**Expected output:** ~33 (Low priority)

### Test Compilation:
```cmd
g++ --version
```
Should show compiler version

---

## 🎓 For Your Presentation

**Show your guide:**

1. **Files:** Point out `HospitalTriageSystem_Windows.cpp` (Windows-native)
2. **Fibonacci Heap:** Explain O(1) insert, O(log n) extract-max
3. **ML Integration:** Show how Python model scores each patient
4. **Run the demo:** Double-click `RUN_WINDOWS.bat`
5. **Watch it run:** Real-time allocation with color-coded priorities

**Key points:**
- ✅ "I implemented Fibonacci Heap for optimal priority scheduling"
- ✅ "ML model predicts urgency from 9 medical parameters"
- ✅ "Dynamic visualization shows real-time doctor allocation"
- ✅ "Built pure Windows version - no Ubuntu needed"

---

## 📞 Quick Commands

### One-Time Setup:
```cmd
SETUP_WINDOWS.bat
```

### Build:
```cmd
BUILD_WINDOWS.bat
```

### Run:
```cmd
RUN_WINDOWS.bat
```

### Install Python packages manually:
```cmd
pip install numpy scikit-learn
```

---

## ✨ What Makes This Special

1. **Pure Windows** - No WSL, no Ubuntu, no MSYS2 required
2. **Fibonacci Heap** - Advanced DSA, better than binary heap
3. **Machine Learning** - Real Random Forest model
4. **Dynamic Visualization** - Not a menu system!
5. **Production Quality** - 1900+ lines of code

---

## 🎯 Final Checklist

Before your presentation:

- [ ] Python installed (check: `python --version`)
- [ ] Python packages installed (check: `pip list | find "numpy"`)
- [ ] Compiler installed (check: `g++ --version` or `cl`)
- [ ] Project builds successfully (`BUILD_WINDOWS.bat`)
- [ ] Program runs successfully (`RUN_WINDOWS.bat`)
- [ ] Reviewed code to explain Fibonacci Heap operations
- [ ] Tested ML model separately

---

## 🚀 You're Ready!

Your Windows-native Hospital Triage System with Fibonacci Heap and ML is ready to impress your guide!

**Good luck with your presentation!** 🎉
