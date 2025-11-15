# 🏥 Smart Hospital Triage System with Fibonacci Heap

## Project Overview

A **Data Structures and Algorithms (DSA)** project implementing an intelligent hospital emergency department triage system using **Fibonacci Heap** and **Machine Learning** for dynamic patient prioritization and doctor allocation.

---

## 🎯 Problem Statement

Modern hospitals face critical challenges:
- ❌ **Manual triaging** → slow, error-prone, inconsistent
- ❌ **Long wait times** → inefficient resource utilization
- ❌ **Unbalanced doctor workload** → burnout and delays
- ❌ **No dynamic priority updates** → critical patients wait

### Our Solution ✅

1. **ML-Based Triage Scoring** (Python)
   - Predicts urgency score (0-100) from medical parameters
   - Random Forest Regressor trained on emergency scenarios

2. **Fibonacci Heap Priority Queue** (C++)
   - O(1) insertion and find-max operations
   - O(log n) extract-max for highest priority patient
   - Optimal for dynamic priority-based scheduling

3. **Dynamic Doctor Allocation** (C++)
   - Real-time assignment based on priority
   - Department-specific allocation
   - Automatic load balancing

4. **Real-time Visualization** (Terminal-based)
   - Live doctor status updates
   - Dynamic queue visualization
   - Priority-based color coding
   - Statistical analytics

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   Patient Arrival                       │
│         (Data loaded from patients_data.csv)            │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│              Python ML Model                            │
│         (Random Forest Regressor)                       │
│  Input: age, gender, chest_pain, diabetes, BP,          │
│         cholesterol, BMI, heart_rate, hypertension      │
│  Output: Priority Score (0-100)                         │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│          Fibonacci Heap (Max-Heap)                      │
│     Department-wise Priority Queues                     │
│  • Cardiology Queue                                     │
│  • Emergency Queue                                      │
│  Highest priority patient always at top                 │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│         Doctor Allocation Engine                        │
│  • Check available doctors by department                │
│  • Extract max priority patient from heap               │
│  • Assign to available doctor                           │
│  • Track treatment time                                 │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│       Real-time Visualization System                    │
│  • Doctor status (Busy/Available)                       │
│  • Current patient being treated                        │
│  • Waiting queue (Fibonacci Heap contents)              │
│  • Statistics and analytics                             │
│  • Color-coded priority levels                          │
└─────────────────────────────────────────────────────────┘
```

---

## 📊 Data Structures Used

### 1. Fibonacci Heap (Primary DSA Component)

**Why Fibonacci Heap?**
- Superior performance for priority queue operations
- Ideal for dynamic priority systems
- Better amortized complexity than binary/binomial heaps

**Operations & Complexity:**
| Operation | Time Complexity |
|-----------|----------------|
| Insert | O(1) amortized |
| Find Maximum | O(1) |
| Extract Maximum | O(log n) amortized |
| Increase Key | O(1) amortized |
| Delete | O(log n) amortized |
| Merge | O(1) |

**Implementation Details:**
- Max-heap variant (highest priority first)
- Supports lazy consolidation
- Cascading cuts for structural balance
- Department-wise separate heaps

### 2. Hash Map (std::map)
- Quick doctor lookup by department
- O(log n) access time

### 3. Vector (std::vector)
- Doctor storage
- Patient data loading
- Efficient dynamic arrays

---

## 🤖 Machine Learning Model

### Algorithm: Random Forest Regressor

**Input Features (9 parameters):**
1. Age (years)
2. Gender (0=Female, 1=Male)
3. Chest Pain Level (0-4)
4. Diabetes (0=No, 1=Yes)
5. Blood Pressure (systolic)
6. Cholesterol (mg/dL)
7. BMI (Body Mass Index)
8. Maximum Heart Rate
9. Hypertension (0=No, 1=Yes)

**Output:**
- Triage Priority Score (0-100)
  - 90-100: **CRITICAL** (Red)
  - 75-89: **HIGH** (Orange/Red)
  - 60-74: **MEDIUM** (Yellow)
  - 40-59: **LOW** (Cyan)
  - 0-39: **ROUTINE** (Green)

**Model Characteristics:**
- Trained on synthetic emergency medical data
- 24+ training samples covering all priority ranges
- Rule-based adjustments for critical scenarios
- Handles edge cases (extreme vitals, age factors)

---

## 📁 File Structure

```
ADSAProject/
├── FibonacciHeap.h              # Fibonacci Heap implementation
├── Visualization.h              # Real-time terminal visualization
├── HospitalTriageSystem.cpp     # Main system logic
├── triage_ml_model.py           # ML model for priority scoring
├── patients_data.csv            # Sample patient data (30 patients)
├── doctors_data.csv             # Doctor information (4 doctors)
├── Makefile                     # Build system
├── README.md                    # Documentation (this file)
├── triage_log.txt              # Generated: Event log
├── priority_data.csv           # Generated: Stats for graphing
└── plot_triage.gnu             # Generated: Gnuplot script
```

---

## 🚀 How to Build and Run

### Prerequisites

```bash
# C++ Compiler (g++)
sudo apt-get update
sudo apt-get install g++ make

# Python 3 and dependencies
sudo apt-get install python3 python3-pip
pip3 install numpy scikit-learn

# Optional: For graph generation
sudo apt-get install gnuplot
```

### Build & Run

```bash
# Clone or navigate to project directory
cd ADSAProject

# Build the project
make

# Run the system
make run

# Or manually:
g++ -std=c++17 -Wall -O2 HospitalTriageSystem.cpp -o hospital_triage
./hospital_triage
```

### Generate Visualization Graph

```bash
# After running the system:
make plot

# Or manually:
gnuplot plot_triage.gnu
# Output: triage_visualization.png
```

---

## 🎬 Demo: How It Works

### Step 1: System Initialization
```
═══════════════════════════════════════════
  Loading doctors from doctors_data.csv
  Loaded 4 doctors
  Loading patients from patients_data.csv
  Loaded 30 patients
═══════════════════════════════════════════
```

### Step 2: ML-Based Priority Scoring
```
► Patient 1 (Rajesh Kumar) - ML Score: 95.3
► Patient 2 (Priya Sharma) - ML Score: 82.1
► Patient 3 (Amit Patel) - ML Score: 58.4
...
All 30 patients scored and added to Fibonacci Heap!
```

### Step 3: Dynamic Real-time Allocation

**Iteration #1:**
```
═══ DOCTOR STATUS ═══

┌─────────────────────────────────────────┐
│ Dr. Arvind Mehta (Cardiology)           │
│ Status: BUSY │ Time Remaining: 3s       │
│ Treating: Rajesh Kumar                  │
│ Priority: 95.3 (CRITICAL)               │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│ Dr. Sneha Kapoor (Cardiology)           │
│ Status: BUSY │ Time Remaining: 3s       │
│ Treating: Sunita Reddy                  │
│ Priority: 96.8 (CRITICAL)               │
└─────────────────────────────────────────┘

WAITING QUEUE - Cardiology (Fibonacci Heap)
┌──────┬─────────────────────┬──────┬──────────┐
│ Pos  │ Name                │ Age  │ Priority │
├──────┼─────────────────────┼──────┼──────────┤
│  1   │ Harish Kumar        │  75  │ 97.5     │
│  2   │ Radha Krishnan      │  65  │ 96.2     │
│  3   │ Lakshmi Iyer        │  55  │ 85.0     │
└──────┴─────────────────────┴──────┴──────────┘

STATISTICS:
  Total Patients: 30
  Treated: 0
  Waiting: 28
  Critical Cases: 8
  High Priority: 10
```

**Iteration #5:**
```
STATISTICS:
  Total Patients: 30
  Treated: 8
  Waiting: 18
  ...
```

**Final:**
```
🎉 ALL PATIENTS TREATED SUCCESSFULLY! 🎉

  Total Patients: 30
  Treated: 30
  Critical Cases: 8
  High Priority: 10
```

---

## 📈 Key Features Demonstrated

### 1. ✅ Fibonacci Heap Operations
- **Insert**: All 30 patients inserted with O(1) complexity
- **Extract-Max**: Highest priority patient always selected first
- **Dynamic updates**: Real-time heap state visualization

### 2. ✅ ML Integration
- Each patient scored using Random Forest model
- Realistic medical parameter-based scoring
- Handles 9 input features per patient

### 3. ✅ Real-time Visualization
- Color-coded priority levels
- Live doctor status updates every 2 seconds
- Dynamic queue visualization
- Progress bars and statistics

### 4. ✅ File-based Data Loading
- Patients loaded from CSV file
- Doctors loaded from CSV file
- Easy to modify and extend data

### 5. ✅ Graph Generation
- Export priority distribution data
- Gnuplot script for visualization
- PNG graph output

---

## 🎓 DSA Concepts Demonstrated

1. **Advanced Heap Structure**
   - Fibonacci Heap with lazy consolidation
   - Cascading cuts and marking nodes
   - Min-degree property maintenance

2. **Priority Queue Applications**
   - Dynamic priority scheduling
   - Real-time resource allocation
   - Multi-queue management

3. **Algorithm Analysis**
   - Amortized time complexity
   - Space-time tradeoffs
   - Performance optimization

4. **Data Structure Integration**
   - Combining heaps, maps, vectors
   - Efficient data organization
   - System design patterns

---

## 🔧 Customization

### Add More Patients
Edit `patients_data.csv`:
```csv
31,New Patient,45,1,Cardiology,3,0,165,220,28.0,155,1,Symptoms,10:35:00
```

### Add More Doctors
Edit `doctors_data.csv`:
```csv
105,Dr. New Doctor,Emergency,Critical Care,7
```

### Modify Treatment Time
In `HospitalTriageSystem.cpp`, function `calculateTreatmentTime()`:
```cpp
int calculateTreatmentTime(double priority) {
    if (priority >= 90) return 5;  // Increase critical treatment time
    ...
}
```

### Adjust ML Model
In `triage_ml_model.py`, modify `_train_model()` to add more training samples.

---

## 📊 Testing the ML Model

Test the ML model separately:
```bash
# Syntax: python3 triage_ml_model.py <age> <gender> <chest_pain> <diabetes> <bp> <cholesterol> <bmi> <heart_rate> <hypertension>

# Critical case
python3 triage_ml_model.py 70 1 4 1 190 270 35.0 175 1
# Output: ~98.0

# Low priority case
python3 triage_ml_model.py 25 1 0 0 120 150 23.0 130 0
# Output: ~35.0
```

---

## 🐛 Troubleshooting

### Python ML Model Not Found
```bash
# Ensure Python 3 is installed
python3 --version

# Install dependencies
pip3 install numpy scikit-learn
```

### Compilation Errors
```bash
# Update g++
sudo apt-get install g++

# Check C++17 support
g++ --version  # Should be >= 7.0
```

### Visualization Issues
- Ensure terminal supports ANSI color codes
- Use a modern terminal (bash, zsh)
- Windows: Use WSL or Git Bash

---

## 📚 Learning Outcomes

This project demonstrates:
- ✅ Implementation of advanced heap data structure
- ✅ Integration of ML with DSA
- ✅ Real-world application of algorithms
- ✅ Dynamic resource allocation strategies
- ✅ File I/O and data parsing
- ✅ System design and architecture
- ✅ Real-time visualization techniques
- ✅ Performance optimization

---

## 🎯 Advantages Over Basic Heaps

| Feature | Binary Heap | Fibonacci Heap |
|---------|-------------|----------------|
| Insert | O(log n) | **O(1)** ✅ |
| Find Max | O(1) | **O(1)** ✅ |
| Extract Max | O(log n) | **O(log n)** ✅ |
| Decrease Key | O(log n) | **O(1)** ✅ |
| Merge | O(n) | **O(1)** ✅ |

**Result**: Fibonacci Heap provides superior performance for priority-based dynamic scheduling!

---

## 👨‍💻 Author & Acknowledgments

**Project Type**: DSA Course Project

**Technologies Used**:
- C++17 (Data Structures)
- Python 3 (Machine Learning)
- scikit-learn (Random Forest)
- NumPy (Numerical Computing)
- Gnuplot (Visualization)

**References**:
- "Introduction to Algorithms" - Cormen, Leiserson, Rivest, Stein
- Fibonacci Heap Paper - Fredman & Tarjan (1987)
- Emergency Triage Systems Research

---

## 📄 License

This project is for educational purposes only.

---

## 🎉 Conclusion

This project successfully demonstrates:
1. ✅ **Fibonacci Heap** implementation and practical usage
2. ✅ **Machine Learning** integration for intelligent scoring
3. ✅ **Real-time dynamic visualization** (not just menu-based!)
4. ✅ **File-based data loading** for scalability
5. ✅ **Complete hospital triage workflow** simulation

**No more basic menu demos!** This is a complete, production-quality DSA project with advanced algorithms, ML integration, and dynamic visualization! 🚀

---

## 🌟 Future Enhancements

- Web-based dashboard with real-time graphs
- Database integration (MySQL/PostgreSQL)
- Multi-hospital network support
- Patient history tracking
- Advanced ML models (Neural Networks)
- Mobile app integration
- IoT device integration (vital monitors)

---

**Happy Coding!** 💻🏥
