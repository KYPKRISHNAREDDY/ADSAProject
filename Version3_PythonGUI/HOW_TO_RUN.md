# 🚀 How to Run Version 3 (GUI Window)

## ✅ Everything You Need is in THIS Folder!

No need to copy files - all required files are already here:
- ✅ `hospital_gui_data.cpp`
- ✅ `gui_visualizer.py`
- ✅ `patients_data.csv`
- ✅ `doctors_data.csv`
- ✅ `triage_ml_model.py`

---

## 🎯 EASIEST WAY: Just Double-Click!

**Simply double-click:** `RUN.bat`

The program will:
1. Compile automatically
2. Generate data
3. Launch GUI window! 🎉

---

## 💻 OR: Manual Method

### Step 1: Compile
```cmd
g++ hospital_gui_data.cpp -o gui_data.exe
```

### Step 2: Generate data
```cmd
gui_data.exe
```

### Step 3: Launch GUI
```cmd
python gui_visualizer.py
```

A colorful window pops up!

---

## 🎮 How to Use

**Buttons in the window:**
- **"▶ Next Patient"** - Process one patient
- **"⏩ Next Iteration"** - Run 5 steps
- **"⏭ Auto Play"** - Full automatic simulation
- **"🔄 Reset"** - Start over

---

## ⏱️ Demo Duration

- Manual (Next Patient): Your choice
- Auto-play: ~2-4 minutes

---

## ✨ Perfect For

- ✅ Popup window demos
- ✅ Desktop application feel
- ✅ Colorful visual display
- ✅ Interactive showcase

---

## 🆘 Troubleshooting

**Problem:** "tkinter not found"
```cmd
pip install tk
```

**Problem:** "g++ is not recognized"
- Install MinGW from: https://winlibs.com/

---

**That's it! You're ready!** 🎉
