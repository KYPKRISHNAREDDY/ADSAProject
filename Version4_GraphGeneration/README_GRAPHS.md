# 📊 Version 4: Graph/Chart Generation System

## ✨ Overview

This version runs the simulation and **generates professional graphs and charts** for analysis and presentation!

Perfect for:
- Creating visual reports for your guide
- Including graphs in your presentation/report
- Statistical analysis of the system
- Professional documentation

---

## 🚀 How to Run

### Step 1: Compile and run the C++ program
```cmd
g++ hospital_graphs.cpp -o graphs.exe
graphs.exe
```

This will:
- Run the full simulation
- Save all data to `graph_data/` folder (CSV files)
- Complete in ~1 minute

### Step 2: Generate graphs
```cmd
pip install matplotlib pandas seaborn
python generate_graphs.py
```

This will:
- Read the CSV data
- Create 6 professional graphs
- Generate an HTML report
- Save everything to `graphs/` folder

### Step 3: View results
```cmd
cd graphs
start REPORT.html
```

Opens a beautiful HTML report in your browser with all graphs!

---

## 📈 Graphs Generated

### 1. **Queue Timeline** (`1_queue_timeline.png`)
**Line chart showing:**
- Total patients waiting over time
- Queue size by department
- How queue shrinks as patients are treated

**Key insight:** Shows Fibonacci Heap efficiency in managing dynamic queue

---

### 2. **Priority Distribution** (`2_priority_distribution.png`)
**Pie chart + Bar chart showing:**
- How many patients in each category
  - Critical (90-100)
  - High (75-89)
  - Medium (60-74)
  - Low (<60)

**Key insight:** ML model distribution across priority levels

---

### 3. **Doctor Utilization** (`3_doctor_utilization.png`)
**Gantt chart showing:**
- Timeline of each doctor's activity
- Which patients they treated
- When they were busy vs. available
- Color-coded by department

**Key insight:** Resource utilization and load balancing

---

###4. **Top Patients by Priority** (`4_allocation_order.png`)
**Horizontal bar chart showing:**
- Top 20 highest priority patients
- Their ML triage scores
- Color-coded by urgency level

**Key insight:** Fibonacci Heap extract-max ensures highest priority first

---

### 5. **ML Score Analysis** (`5_ml_score_analysis.png`)
**Histogram + Box plot showing:**
- Distribution of ML scores
- Mean and median scores
- Score distribution by department
- Statistical analysis

**Key insight:** Random Forest model performance and patterns

---

### 6. **Summary Dashboard** (`6_summary_dashboard.png`)
**Combined dashboard with:**
- Cumulative patients treated
- Priority category breakdown
- Department distribution
- Statistics table with key metrics

**Key insight:** Complete overview of system performance

---

## 📄 HTML Report

The `REPORT.html` file includes:
- ✅ All 6 graphs embedded
- ✅ Explanations for each graph
- ✅ Key statistics
- ✅ Professional formatting
- ✅ Easy to share or present

**Perfect for:** Opening during your presentation or submitting with your project!

---

## 📁 File Structure

```
Version4_GraphGeneration/
├── hospital_graphs.cpp          # C++ program
├── generate_graphs.py           # Python graph generator
├── README_GRAPHS.md             # This file
│
├── graph_data/                  # Generated data (CSV)
│   ├── allocation_timeline.csv
│   ├── queue_sizes.csv
│   ├── priority_distribution.csv
│   ├── doctor_utilization.csv
│   └── ml_scores.csv
│
└── graphs/                      # Generated visualizations
    ├── 1_queue_timeline.png
    ├── 2_priority_distribution.png
    ├── 3_doctor_utilization.png
    ├── 4_allocation_order.png
    ├── 5_ml_score_analysis.png
    ├── 6_summary_dashboard.png
    └── REPORT.html              # Main report
```

---

## 🎯 Use Cases

### For Presentation:
1. Run the program before your demo
2. Open `REPORT.html` in browser
3. Show graphs while explaining concepts
4. Professional and impressive!

### For Documentation:
1. Include graphs in your project report
2. High-quality PNG files (300 DPI)
3. Ready to print or submit

### For Analysis:
1. See which priorities are most common
2. Analyze doctor utilization
3. Understand queue behavior
4. Validate ML model performance

---

## 💡 Key Features

### ✅ Professional Quality
- 300 DPI high-resolution images
- Publication-ready graphs
- Clean, modern design

### ✅ Comprehensive Analysis
- 6 different visualization types
- Statistical summaries
- Multiple perspectives

### ✅ Easy to Use
- Just run 2 commands
- Automatic graph generation
- HTML report ready to open

### ✅ Customizable
- Edit `generate_graphs.py` to change colors, styles
- Modify graph types
- Add your own analyses

---

## 🎨 Graph Customization

Want different colors or styles? Edit `generate_graphs.py`:

```python
# Change color schemes
colors = ['#YourColor1', '#YourColor2', '#YourColor3', '#YourColor4']

# Change figure sizes
plt.rcParams['figure.figsize'] = (16, 10)  # Larger graphs

# Change style
sns.set_style("darkgrid")  # Different background
```

---

## 📊 Sample Statistics Shown

```
Total Patients:     30
Patients Treated:   30
Average ML Score:   68.5
Median ML Score:    72.0
Highest Priority:   100.0
Lowest Priority:    28.3
Total Iterations:   45
Critical Cases:     8
High Priority:      10
Medium Priority:    8
Low Priority:       4
```

---

## 🔧 Requirements

```cmd
pip install matplotlib pandas seaborn
```

**Note:** These are standard data science libraries, very stable and well-supported.

---

## ✨ Sample Graph Previews

### Queue Timeline:
- Shows decreasing queue as system processes patients
- Demonstrates system efficiency

### Doctor Gantt Chart:
- Visual timeline of doctor activities
- Shows load distribution
- Identifies busy periods

### ML Score Distribution:
- Shows realistic medical triage distribution
- Most patients in medium range
- Critical cases clearly identified

---

## 🎓 Perfect for Your Guide!

Show these graphs to demonstrate:

1. **Data Structures (Fibonacci Heap)**
   - Queue timeline shows efficient priority management
   - Allocation order proves extract-max works correctly

2. **Machine Learning Integration**
   - ML score distribution shows realistic triage
   - Priority categories based on medical parameters

3. **System Performance**
   - Doctor utilization shows balanced load
   - Statistics prove all patients treated

4. **Professional Quality**
   - High-quality visualizations
   - Complete documentation
   - Ready for submission

---

## 🚀 Quick Start Summary

```cmd
# Step 1: Compile and run
g++ hospital_graphs.cpp -o graphs.exe
graphs.exe

# Step 2: Generate graphs
python generate_graphs.py

# Step 3: View results
cd graphs
start REPORT.html
```

**Done!** Beautiful graphs ready for your presentation! 🎉

---

## 📝 Notes

- Graphs are saved as PNG (portable, high quality)
- HTML report works offline (no internet needed)
- Data files can be opened in Excel for further analysis
- All graphs use consistent color schemes
- Professional typography and styling

---

## 🌟 Why Use This Version?

- ✅ Creates **professional documentation** automatically
- ✅ Shows **statistical analysis** of the system
- ✅ Provides **visual evidence** of algorithms working
- ✅ Generates **presentation-ready** materials
- ✅ Combines with **Version 1** for complete demo!

**Recommended:** Use **Version 1** for live demo, then show **Version 4 graphs** for analysis! 🎯
