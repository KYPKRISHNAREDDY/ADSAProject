# Hospital Triage System using Fibonacci Heap

A real-time hospital emergency room triage system that uses **Fibonacci Heap** data structure for patient priority queue management and **Machine Learning** for intelligent priority scoring.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Fibonacci Heap Data Structure](#fibonacci-heap-data-structure)
3. [Machine Learning Model](#machine-learning-model)
4. [System Architecture](#system-architecture)
5. [Installation & Setup](#installation--setup)
6. [Usage Guide](#usage-guide)
7. [File Structure](#file-structure)
8. [Time Complexity Analysis](#time-complexity-analysis)

---

## Project Overview

### Problem Statement
In a hospital emergency room, patients arrive with varying levels of urgency. The challenge is to:
- Efficiently manage patient queue based on medical priority
- Handle emergency cases that need immediate attention (preemption)
- Assign appropriate doctors based on department matching
- Visualize the queue state in real-time

### Solution
We use a **Fibonacci Heap (Max-Heap)** to manage the patient priority queue because:
- O(1) insertion time for new patients
- O(1) access to highest priority patient
- Efficient handling of emergency insertions
- Better amortized performance than binary heaps

---

## Fibonacci Heap Data Structure

### What is a Fibonacci Heap?

A Fibonacci Heap is a collection of trees satisfying the **min-heap or max-heap property**. Unlike binary heaps, Fibonacci heaps have a more relaxed structure that allows for better amortized time complexity.

```
FIBONACCI HEAP STRUCTURE (Max-Heap):

    Root List (Circular Doubly Linked List)
    ┌───────────────────────────────────────────┐
    │                                           │
    ▼                                           │
┌───────┐     ┌───────┐     ┌───────┐          │
│  93   │◄───►│  90   │◄───►│  87   │◄─────────┘
│(max)  │     │       │     │       │
└───┬───┘     └───┬───┘     └───────┘
    │             │
    ▼             ▼
┌───────┐     ┌───────┐
│  82   │     │  83   │
└───┬───┘     └───────┘
    │
    ▼
┌───────┐
│  74   │
└───────┘
```

### Key Properties

| Property | Description |
|----------|-------------|
| **Root List** | Circular doubly linked list of tree roots |
| **Max Pointer** | Points to the node with maximum priority |
| **Lazy Operations** | Consolidation only happens during extract-max |
| **Marked Nodes** | Used for cascading cut during decrease-key |

### Operations Implemented

#### 1. INSERT - O(1)
```cpp
void insert(Patient p) {
    Node* node = new Node(p);
    // Add to root list (circular doubly linked list)
    node->left = maxNode;
    node->right = maxNode->right;
    maxNode->right->left = node;
    maxNode->right = node;
    // Update max if needed
    if (node->data.priority > maxNode->data.priority) {
        maxNode = node;
    }
    count++;
}
```

**How it works:**
1. Create a new node with patient data
2. Add node to the root list (just pointer manipulation)
3. Update maxNode if new patient has higher priority
4. No restructuring needed!

#### 2. EXTRACT-MAX - O(log n) amortized
```cpp
Patient extractMax() {
    Node* z = maxNode;

    // Step 1: Add all children of max to root list
    if (z->child != NULL) {
        // Move each child to root list
        // Set parent pointers to NULL
    }

    // Step 2: Remove max from root list
    z->left->right = z->right;
    z->right->left = z->left;

    // Step 3: Consolidate (merge trees of same degree)
    consolidate();

    return z->data;
}
```

**How it works:**
1. Remove the maximum node
2. Add all its children to the root list
3. **Consolidate**: Merge trees with same degree until all trees have unique degrees

#### 3. CONSOLIDATE - The Key Operation
```cpp
void consolidate() {
    // Array A[0..D] where D = log(n)
    vector<Node*> A(maxDegree, NULL);

    for each root node x:
        d = x->degree
        while (A[d] != NULL):
            y = A[d]
            if (x->priority < y->priority):
                swap(x, y)
            link(y, x)  // Make y child of x
            A[d] = NULL
            d++
        A[d] = x

    // Rebuild root list from array A
}
```

**Visual Example of Consolidation:**
```
Before Consolidate (after extract-max):
Root List: [82°0] - [74°0] - [69°0] - [83°1] - [87°0]
           (degree shown as superscript)

Step 1: Process 82°0
        A[0] = 82

Step 2: Process 74°0, A[0] exists!
        Link 74 under 82, degree becomes 1
        A[0] = NULL, A[1] = 82°1

Step 3: Process 69°0
        A[0] = 69

Step 4: Process 83°1, A[1] exists!
        Link 82 under 83, degree becomes 2
        A[1] = NULL, A[2] = 83°2

After Consolidate:
Root List: [87°0] - [69°0] - [83°2]
                              │
                           [82°1]
                              │
                           [74°0]
```

### Why Fibonacci Heap for Hospital Triage?

| Operation | Binary Heap | Fibonacci Heap | Hospital Use Case |
|-----------|-------------|----------------|-------------------|
| Insert | O(log n) | **O(1)** | New patient arrives |
| Find-Max | O(1) | O(1) | Check next patient |
| Extract-Max | O(log n) | O(log n)* | Treat highest priority |
| Increase-Key | O(log n) | **O(1)** | Patient condition worsens |

*Amortized time complexity

**Real-world benefit:** When 100 emergency patients arrive rapidly, Fibonacci Heap handles all insertions in O(100) time vs O(100 × log n) for binary heap.

---

## Machine Learning Model

### Model: Random Forest Classifier

We use **Random Forest** - an ensemble learning method that builds multiple decision trees and combines their predictions.

```
RANDOM FOREST ARCHITECTURE:

Input Features                    Decision Trees                 Output
┌─────────────┐                  ┌──────────┐
│ Age         │───────────────►  │  Tree 1  │────┐
├─────────────┤                  └──────────┘    │
│ Chest Pain  │───────────────►  ┌──────────┐   │    ┌─────────┐
├─────────────┤                  │  Tree 2  │───┼───►│ VOTING  │──► Priority
│ Blood Press │───────────────►  └──────────┘   │    │ (Avg)   │    Score
├─────────────┤                       ...       │    └─────────┘
│ Heart Rate  │───────────────►  ┌──────────┐   │
└─────────────┘                  │ Tree 100 │───┘
                                 └──────────┘
```

### Why Random Forest?

| Advantage | Description |
|-----------|-------------|
| **No Overfitting** | Multiple trees reduce variance |
| **Feature Importance** | Shows which vitals matter most |
| **Handles Non-linear** | Can capture complex relationships |
| **Robust** | Works well with medical data |

### Training Pipeline

```python
# 1. Load Data (10,000 real patient records)
data = load_csv('datasets/heart_disease_train.csv')

# 2. Select Features
features = ['age', 'cp', 'trestbps', 'thalach']
# cp = chest pain type (0-4)
# trestbps = resting blood pressure
# thalach = maximum heart rate

# 3. Split Data
X_train, X_test = train_test_split(data, test_size=0.2)

# 4. Normalize Features
scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)

# 5. Train Model
model = RandomForestClassifier(
    n_estimators=100,    # 100 decision trees
    max_depth=10,        # Prevent overfitting
    random_state=42      # Reproducibility
)
model.fit(X_train_scaled, y_train)

# 6. Save Model
pickle.dump({'model': model, 'scaler': scaler}, 'trained_model.pkl')
```

### Dataset

**Source:** Kaggle Cardiovascular Disease Dataset (70,000 records)
**Used:** 10,000 processed records with realistic patient profiles

| Column | Description | Range |
|--------|-------------|-------|
| age | Patient age | 20-90 years |
| cp | Chest pain type | 0-4 (higher = worse) |
| trestbps | Blood pressure | 90-200 mmHg |
| thalach | Heart rate | 60-200 bpm |
| target | Heart disease | 0 (no) / 1 (yes) |

### Model Performance

```
============================================================
  MODEL EVALUATION RESULTS
============================================================
Accuracy: 83.05%

Confusion Matrix:
                 Predicted
              Low Risk  High Risk
Actual Low     847        173      (True Neg / False Pos)
Actual High    166        814      (False Neg / True Pos)

Feature Importance:
  thalach (Heart Rate) : 41.1% ████████████████
  cp (Chest Pain)      : 34.3% █████████████
  trestbps (BP)        : 14.9% █████
  age                  :  9.8% ███
============================================================
```

### Priority Score Calculation

```python
def predict_priority(age, chest_pain, bp, heart_rate):
    # Get ML probability of high risk
    prob = model.predict_proba([age, cp, bp, hr])[0][1]

    # Base score from ML (0-60 points)
    base_score = prob * 60

    # Additional clinical factors (0-40 points)
    age_factor = min(age / 100 * 15, 15)      # Max 15 pts
    bp_factor = min((bp - 120) / 80 * 12, 12)  # Max 12 pts
    hr_factor = min((hr - 80) / 100 * 8, 8)    # Max 8 pts
    cp_factor = chest_pain * 1.5               # Max 6 pts

    # Final score (25-100)
    priority = base_score + age_factor + bp_factor + hr_factor + cp_factor
    return max(25, min(100, priority))
```

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    HOSPITAL TRIAGE SYSTEM                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────────┐  │
│  │ patients.csv │───►│   ML Model   │───►│ Priority Scores  │  │
│  │ (12 patients)│    │ (Random      │    │ (25-100)         │  │
│  └──────────────┘    │  Forest)     │    └────────┬─────────┘  │
│                      └──────────────┘             │             │
│                                                   ▼             │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                   FIBONACCI HEAP                          │  │
│  │  ┌─────┐   ┌─────┐   ┌─────┐   ┌─────┐                   │  │
│  │  │ 93  │◄─►│ 90  │◄─►│ 87  │◄─►│ 83  │ ...              │  │
│  │  │(max)│   │     │   │     │   │     │                   │  │
│  │  └──┬──┘   └──┬──┘   └─────┘   └─────┘                   │  │
│  │     │         │                                           │  │
│  │     ▼         ▼                                           │  │
│  │  ┌─────┐   ┌─────┐                                       │  │
│  │  │ 82  │   │ 74  │                                       │  │
│  │  └─────┘   └─────┘                                       │  │
│  └──────────────────────────────────────────────────────────┘  │
│                          │                                      │
│                          ▼ Extract-Max                          │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │                 DOCTOR ASSIGNMENT                         │  │
│  │                                                           │  │
│  │  Patient: Sunita (Priority 93, Cardiology)               │  │
│  │                    │                                      │  │
│  │         ┌──────────┴──────────┐                          │  │
│  │         ▼                     ▼                          │  │
│  │  ┌─────────────┐      ┌─────────────┐                    │  │
│  │  │ Free Doctor │      │ Preemption  │                    │  │
│  │  │   Found?    │      │ (if higher  │                    │  │
│  │  │     YES     │      │  priority)  │                    │  │
│  │  └──────┬──────┘      └─────────────┘                    │  │
│  │         ▼                                                 │  │
│  │  Assigned to Dr. Smith (Cardiology)                      │  │
│  └──────────────────────────────────────────────────────────┘  │
│                          │                                      │
│                          ▼                                      │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │              GRAPHVIZ VISUALIZATION                       │  │
│  │                                                           │  │
│  │  heap.dot ──► dot command ──► heap.png ──► viewer.html   │  │
│  │                                                           │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## Installation & Setup

### Prerequisites

| Software | Version | Purpose |
|----------|---------|---------|
| G++ | 5.0+ | Compile C++ code |
| Python | 3.7+ | ML model training |
| Graphviz | 2.40+ | Visualization |
| scikit-learn | 0.24+ | Random Forest model |

### Installation Steps

**Windows:**
```cmd
# 1. Install Python (from python.org)
# 2. Install Graphviz (from graphviz.org, add to PATH)
# 3. Install Python packages
pip install numpy scikit-learn
```

**Linux:**
```bash
# 1. Install packages
sudo apt-get install g++ python3 python3-pip graphviz

# 2. Install Python packages
pip3 install numpy scikit-learn
```

---

## Usage Guide

### Quick Start
```cmd
cd Version6_Simple
run.bat          # Windows
./run.sh         # Linux
```

### Manual Execution

**Step 1: Train ML Model (one-time)**
```cmd
python train_model.py
```

**Step 2: Compile C++ Program**
```cmd
g++ -o hospital_triage hospital_triage.cpp
```

**Step 3: Open Visualization**
- Open `viewer.html` in web browser

**Step 4: Run Program**
```cmd
hospital_triage.exe   # Windows
./hospital_triage     # Linux
```

**Step 5: Follow Interactive Menu**
1. Press Enter → ML calculates priority scores
2. Press Enter → Build Fibonacci Heap
3. Press Enter → Start simulation
4. Watch real-time visualization!

---

## File Structure

```
Version6_Simple/
├── hospital_triage.cpp      # Main C++ program
├── triage.py                # ML prediction script
├── train_model.py           # ML training script
├── viewer.html              # Real-time visualization viewer
├── run.bat                  # Windows batch runner
│
├── patients_data.csv        # 12 patients for demo
├── doctors_data.csv         # 6 doctors (2 per department)
├── trained_model.pkl        # Saved ML model
│
├── datasets/
│   ├── heart_disease_train.csv    # 10,000 training records
│   ├── cardio_raw.csv             # Raw 70K dataset
│   └── process_cardio_data.py     # Data processing script
│
└── README.md                # This documentation
```

---

## Time Complexity Analysis

### Fibonacci Heap Operations

| Operation | Time Complexity | Description |
|-----------|-----------------|-------------|
| Insert | O(1) | Add patient to root list |
| Find-Max | O(1) | Return maxNode pointer |
| Extract-Max | O(log n)* | Remove max + consolidate |
| Union | O(1) | Merge two heaps |
| Increase-Key | O(1)* | Update priority |

*Amortized time complexity

### Overall System Complexity

| Phase | Operations | Complexity |
|-------|------------|------------|
| Load Patients | Read CSV | O(n) |
| ML Scoring | Predict each | O(n × m) where m = features |
| Build Heap | n inserts | O(n) |
| Process All | n extracts | O(n log n) |

### Comparison with Other Data Structures

| Data Structure | Insert | Extract-Max | Use Case |
|----------------|--------|-------------|----------|
| Unsorted Array | O(1) | O(n) | Few extracts |
| Sorted Array | O(n) | O(1) | Few inserts |
| Binary Heap | O(log n) | O(log n) | General purpose |
| **Fibonacci Heap** | **O(1)** | O(log n) | Many inserts, priority updates |

---

## Features Demonstrated

1. **Fibonacci Heap Operations**
   - O(1) Insert
   - O(log n) Extract-Max with Consolidation
   - Circular doubly linked list management

2. **Machine Learning Integration**
   - Random Forest Classifier
   - Real cardiovascular dataset (10K records)
   - 83% accuracy in risk prediction

3. **Real-time Visualization**
   - Graphviz DOT format
   - Auto-refreshing HTML viewer
   - Color-coded priority levels

4. **Hospital Management**
   - Department-based doctor assignment
   - Preemption for critical patients
   - Treatment time simulation

---

## Authors

- **Student:** [Your Name]
- **Course:** Advanced Data Structures and Algorithms
- **Institution:** [Your Institution]

---

## References

1. Fredman, M. L., & Tarjan, R. E. (1987). Fibonacci heaps and their uses in improved network optimization algorithms.
2. Cormen, T. H., et al. Introduction to Algorithms (CLRS), Chapter 19: Fibonacci Heaps.
3. Kaggle Cardiovascular Disease Dataset: https://www.kaggle.com/datasets/sulianova/cardiovascular-disease-dataset
4. scikit-learn Random Forest Documentation: https://scikit-learn.org/stable/modules/ensemble.html#random-forests
