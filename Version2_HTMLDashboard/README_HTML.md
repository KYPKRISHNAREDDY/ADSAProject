# 🌐 Version 2: HTML Dashboard (Browser UI)

## ✨ Overview

This version creates a **beautiful interactive web dashboard** that opens in your browser!

Features:
- 🎨 Modern, professional UI
- 🎮 Interactive controls (Next, Previous, Auto-play)
- 📊 Real-time statistics
- 👨‍⚕️ Visual doctor status cards
- ⏳ Dynamic queue visualization
- ⌨️ Keyboard shortcuts
- 📱 Responsive design

**Perfect for:** Presentations, demos, and impressing your guide with a modern UI!

---

## 🚀 How to Run

### Step 1: Compile and run C++ program
```cmd
g++ hospital_html.cpp -o html.exe
html.exe
```

This generates `dashboard_data.json` with all iteration data.

### Step 2: Create HTML dashboard
```cmd
python create_dashboard.py
```

This creates `dashboard.html` and **automatically opens** it in your browser!

### Step 3: Interact!
Use the buttons or keyboard shortcuts:
- **→ Arrow Right**: Next iteration
- **← Arrow Left**: Previous iteration
- **Spacebar**: Auto-play/pause
- **Buttons**: Click to navigate

---

## 🎨 What You'll See

### Dashboard Layout:

```
┌─────────────────────────────────────────────────────────────┐
│  🏥 Smart Hospital Triage System                            │
│     Real-time ML-Based Patient Allocation                   │
├─────────────────────────────────────────────────────────────┤
│  [▶ Next] [◀ Previous] [⏯ Auto Play] [🔄 Reset] [⏭ End]   │
├─────────────────────────────────────────────────────────────┤
│  Iteration: 5 / 45                                          │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  📊 STATISTICS                                              │
│  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐            │
│  │  30  │ │  12  │ │  18  │ │  8   │ │  10  │            │
│  │Total │ │Treat │ │Wait  │ │Crit  │ │High  │            │
│  └──────┘ └──────┘ └──────┘ └──────┘ └──────┘            │
│                                                              │
│  Progress: [████████░░░░░░░░░] 40%                        │
│                                                              │
│  🔷 Fibonacci Heap: O(1) Insert | O(log n) Extract-Max    │
│                                                              │
│  👨‍⚕️ DOCTOR STATUS                                          │
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │ Dr. Arvind Mehta │  │ Dr. Sneha Kapoor │               │
│  │    🔴 BUSY       │  │   🟢 AVAILABLE   │               │
│  │ Treating:        │  │ Cardiology       │               │
│  │ Rajesh Kumar     │  │                  │               │
│  │ Priority: 95.3   │  │                  │               │
│  │ Time: 3s         │  │                  │               │
│  └──────────────────┘  └──────────────────┘               │
│                                                              │
│  ⏳ WAITING QUEUE                                           │
│  Cardiology Department (8 waiting)                         │
│  ┌───┬───────────────┬─────┬──────────┬──────────┐       │
│  │ # │ Name          │ Age │ Priority │ Status   │       │
│  ├───┼───────────────┼─────┼──────────┼──────────┤       │
│  │ 1 │ Harish Kumar  │ 75  │ 97.5     │ CRITICAL │       │
│  │ 2 │ Radha K.      │ 65  │ 96.2     │ CRITICAL │       │
│  │ 3 │ Lakshmi Iyer  │ 55  │ 85.0     │ HIGH     │       │
│  └───┴───────────────┴─────┴──────────┴──────────┘       │
└─────────────────────────────────────────────────────────────┘
```

---

## 🎮 Interactive Features

### Controls:
- **▶ Next Iteration** - Advance one step
- **◀ Previous** - Go back one step
- **⏯ Auto Play** - Automatically advance every 2 seconds
- **🔄 Reset** - Go back to iteration 1
- **⏭ Jump to End** - See final state

### Keyboard Shortcuts:
- **Right Arrow (→)** - Next
- **Left Arrow (←)** - Previous
- **Spacebar** - Toggle auto-play

### Visual Indicators:
- 🔴 **Red cards** - Doctor busy
- 🟢 **Green cards** - Doctor available
- **Color-coded priorities**:
  - Red badge = Critical (90-100)
  - Orange badge = High (75-89)
  - Yellow badge = Medium (60-74)
  - Blue badge = Low (<60)

---

## 🎯 Perfect For:

### Live Demonstration:
1. Project on screen during presentation
2. Click through iterations while explaining
3. Point to statistics in real-time
4. Show doctor allocation happening

### Remote Presentation:
1. Share screen
2. Navigate smoothly with buttons
3. Professional appearance
4. No command line needed

### Documentation:
1. Take screenshots of dashboard
2. Include in report
3. Show system states
4. Professional visual aids

---

## 💡 How It Works

### Data Flow:
```
C++ Program (hospital_html.cpp)
    ↓
Saves each iteration to dashboard_data.json
    ↓
Python Script (create_dashboard.py)
    ↓
Embeds data into HTML template
    ↓
dashboard.html (Interactive webpage)
    ↓
JavaScript handles interactivity
```

### Technology Stack:
- **C++**: Backend simulation
- **Python**: HTML generation
- **HTML/CSS**: Beautiful UI
- **JavaScript**: Interactivity
- **JSON**: Data storage

---

## 🎨 Customization

### Change Colors:
Edit `create_dashboard.py` and modify CSS:
```css
.header {
    background: linear-gradient(135deg, #YOUR_COLOR1, #YOUR_COLOR2);
}
```

### Change Auto-play Speed:
In the HTML JavaScript section:
```javascript
autoPlayInterval = setInterval(() => {
    ...
}, 2000);  // Change 2000 to desired milliseconds
```

### Add More Statistics:
Modify the stats section in `create_dashboard.py`

---

## ✅ Advantages

### vs Console Version:
- ✅ Much more visual
- ✅ Easy to navigate back/forward
- ✅ Looks professional
- ✅ No scrolling issues
- ✅ Can pause at any point

### vs GUI Version:
- ✅ No installation needed (works in browser)
- ✅ Easy to share (just send HTML file)
- ✅ Cross-platform (works everywhere)
- ✅ Modern, responsive design

---

## 📊 Statistics Displayed

The dashboard shows:
- **Total Patients** - Total in system
- **Treated** - Completed treatments
- **Waiting** - Currently in queue
- **Critical Cases** - Priority ≥ 90
- **High Priority** - Priority ≥ 75
- **Progress Bar** - Visual treatment progress
- **Fibonacci Heap Info** - Algorithm complexity

---

## 🎓 For Your Presentation

### Opening Line:
*"Let me show you our interactive web dashboard that visualizes the real-time triage system..."*

### Navigation Demo:
1. Click "Next Iteration"
2. Explain what's happening
3. Point to doctor status changing
4. Show queue shrinking
5. Highlight priority scores
6. Use auto-play for continuous demo

### Key Points to Mention:
- "Notice how the highest priority patient (97.5) is always extracted first from the Fibonacci Heap"
- "The dashboard updates in real-time showing doctor allocation"
- "We can navigate back and forth to explain any iteration"
- "The color coding makes it easy to identify critical cases"

---

## 🔧 Requirements

```cmd
# No special Python packages needed!
# Uses only standard library
```

**Browser:** Any modern browser (Chrome, Firefox, Edge, Safari)

---

## 📁 File Structure

```
Version2_HTMLDashboard/
├── hospital_html.cpp          # C++ backend
├── create_dashboard.py        # Dashboard generator
├── README_HTML.md             # This file
│
├── dashboard_data.json        # Generated data
└── dashboard.html             # Generated dashboard (opens in browser)
```

---

## 🎬 Quick Demo Script

**For Your Guide:**

1. **Open dashboard.html**
   - "Here's our interactive web-based visualization"

2. **Click Next Iteration**
   - "Each iteration shows real-time state changes"

3. **Point to Fibonacci Heap Section**
   - "Notice the O(1) insert and O(log n) extract-max complexity"

4. **Show Doctor Cards**
   - "Red indicates busy, green shows available doctors"

5. **Scroll to Queue Table**
   - "Highest priority patient is always at the top"

6. **Click Auto-Play**
   - "We can run the entire simulation automatically"

7. **Show Statistics**
   - "All 30 patients treated, 8 were critical cases"

---

## 🌟 Why Use This Version?

- ✅ **Most Visual** - Beautiful, modern UI
- ✅ **Interactive** - Full control over navigation
- ✅ **Professional** - Impresses guides and reviewers
- ✅ **Shareable** - Can email just the HTML file
- ✅ **Easy to Use** - No command line during demo
- ✅ **Keyboard Friendly** - Quick navigation with arrows

**Highly Recommended for Presentations!** 🎯

---

## 🚀 Quick Start Summary

```cmd
# Step 1: Generate data
g++ hospital_html.cpp -o html.exe
html.exe

# Step 2: Create dashboard
python create_dashboard.py

# Step 3: Opens automatically in browser!
# Or manually: start dashboard.html
```

**That's it!** Professional dashboard ready in 3 commands! 🎉
