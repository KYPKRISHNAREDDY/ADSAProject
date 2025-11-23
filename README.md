# Hospital Triage System using Fibonacci Heap

A real-time hospital emergency room triage system that uses **Fibonacci Heap** data structure for patient priority queue management and **Machine Learning (Random Forest)** for intelligent priority scoring.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Fibonacci Heap - In Depth](#fibonacci-heap---in-depth)
3. [Machine Learning Model - In Depth](#machine-learning-model---in-depth)
4. [System Architecture](#system-architecture)
5. [Installation & Setup](#installation--setup)
6. [Usage Guide](#usage-guide)
7. [Time Complexity Analysis](#time-complexity-analysis)

---

## Project Overview

### Problem Statement
In a hospital emergency room, patients arrive with varying levels of urgency. The challenge is to:
- Efficiently manage patient queue based on medical priority
- Handle emergency cases that need immediate attention (preemption)
- Assign appropriate doctors based on department matching
- Visualize the queue state in real-time

### Why Fibonacci Heap?
| Scenario | Binary Heap | Fibonacci Heap |
|----------|-------------|----------------|
| 100 patients arrive rapidly | O(100 × log n) inserts | **O(100)** inserts |
| Find most critical patient | O(1) | O(1) |
| Patient condition worsens | O(log n) update | **O(1)** update |

---

# Fibonacci Heap - In Depth

## What is a Heap?

A **heap** is a tree-based data structure that satisfies the **heap property**:
- **Max-Heap**: Parent node is always ≥ children (we use this)
- **Min-Heap**: Parent node is always ≤ children

```
MAX-HEAP EXAMPLE:
        93 (root = maximum)
       /  \
      87   90
     /  \
    74   82
```

## What Makes Fibonacci Heap Special?

Unlike a Binary Heap (single tree), a **Fibonacci Heap** is a **collection of trees** stored in a circular doubly linked list.

```
FIBONACCI HEAP STRUCTURE:

     ┌──────────────────────────────────────────────────────────┐
     │                    ROOT LIST                              │
     │         (Circular Doubly Linked List)                     │
     └──────────────────────────────────────────────────────────┘
                              │
         ┌────────────────────┼────────────────────┐
         │                    │                    │
         ▼                    ▼                    ▼
     ┌───────┐            ┌───────┐            ┌───────┐
     │  93   │◄──────────►│  87   │◄──────────►│  69   │
     │ (MAX) │            │       │            │       │
     └───┬───┘            └───┬───┘            └───────┘
         │                    │                  Tree 3
         │                    │                (degree 0)
         ▼                    ▼
     ┌───────┐            ┌───────┐
     │  82   │            │  74   │
     └───┬───┘            └───────┘
         │                 Tree 2
         ▼               (degree 1)
     ┌───────┐
     │  41   │
     └───────┘
       Tree 1
     (degree 2)

     maxNode pointer ──────► points to node with value 93
```

### Key Components

| Component | Description |
|-----------|-------------|
| **Root List** | Circular doubly linked list containing root of each tree |
| **maxNode** | Pointer to the node with maximum priority |
| **Degree** | Number of children a node has |
| **Mark** | Boolean flag used for cascading cuts |

### Node Structure

```cpp
struct Node {
    Patient data;        // Patient information
    int degree;          // Number of children
    Node* parent;        // Parent node (NULL for roots)
    Node* child;         // Pointer to one child
    Node* left;          // Left sibling (circular)
    Node* right;         // Right sibling (circular)
    bool mark;           // Has this node lost a child?
};
```

---

## Fibonacci Heap Operations Explained

### 1. INSERT Operation - O(1)

**Goal:** Add a new patient to the heap.

**How it works:**
1. Create a new node
2. Add it to the root list (just update 4 pointers!)
3. Update maxNode if needed

```
BEFORE INSERT (Patient: Amit, Priority: 85):

Root List: [93] ◄──► [87] ◄──► [69]
            ▲                    │
            └────────────────────┘
maxNode = 93


AFTER INSERT:

Root List: [93] ◄──► [85] ◄──► [87] ◄──► [69]
            ▲         NEW              │
            └──────────────────────────┘
maxNode = 93 (unchanged, 93 > 85)
```

**Code:**
```cpp
void insert(Patient p) {
    Node* node = new Node(p);

    if (maxNode == NULL) {
        // First node - points to itself
        maxNode = node;
        node->left = node;
        node->right = node;
    } else {
        // Add to root list (between maxNode and maxNode->right)
        node->left = maxNode;
        node->right = maxNode->right;
        maxNode->right->left = node;
        maxNode->right = node;

        // Update max if new node has higher priority
        if (node->data.priority > maxNode->data.priority) {
            maxNode = node;
        }
    }
    count++;
}
```

**Why O(1)?** We're just updating 4 pointers - no traversal, no restructuring!

---

### 2. FIND-MAX Operation - O(1)

**Goal:** Get the highest priority patient.

```cpp
Patient getMax() {
    return maxNode->data;  // Just return what maxNode points to!
}
```

**Why O(1)?** We always maintain a pointer to the maximum.

---

### 3. EXTRACT-MAX Operation - O(log n) amortized

**Goal:** Remove and return the highest priority patient.

This is where the "magic" happens! It has 3 phases:

#### Phase 1: Remove maxNode and add its children to root list

```
BEFORE (extracting 93):

Root List: [93] ◄──► [87] ◄──► [69]
            │
            ▼
          [82]
            │
            ▼
          [41]

AFTER Phase 1:

Root List: [82] ◄──► [41] ◄──► [87] ◄──► [69]
           (was       (was
           child)     grandchild)

93 is removed and returned
```

#### Phase 2: CONSOLIDATE - Merge trees of same degree

This is the KEY operation that makes Fibonacci Heap efficient!

**Rule:** After consolidation, no two trees should have the same degree.

```
CONSOLIDATE STEP-BY-STEP:

Initial Root List (after removing 93):
[82°⁰] ◄──► [41°⁰] ◄──► [87°¹] ◄──► [69°⁰]
                          │
                        [74]

Step 1: Create degree array A[]
        A[0] = NULL, A[1] = NULL, A[2] = NULL

Step 2: Process node 82 (degree 0)
        A[0] is empty → A[0] = 82

Step 3: Process node 41 (degree 0)
        A[0] has 82 → LINK! (82 > 41, so 41 becomes child of 82)

        82°¹               A[0] = NULL
         │                 A[1] = 82
        41

Step 4: Process node 87 (degree 1)
        A[1] has 82 → LINK! (87 > 82, so 82 becomes child of 87)

        87°²               A[1] = NULL
         │                 A[2] = 87
        82°¹
         │
        41

Step 5: Process node 69 (degree 0)
        A[0] is empty → A[0] = 69

FINAL Root List:
[87°²] ◄──► [69°⁰]
  │
 82°¹
  │
 41

Trees reduced from 4 to 2!
maxNode updated to 87
```

**Consolidate Code:**
```cpp
void consolidate() {
    // Array to store trees by degree
    int maxDegree = log2(count) + 2;
    vector<Node*> A(maxDegree, NULL);

    // Collect all root nodes
    vector<Node*> roots;
    Node* curr = maxNode;
    do {
        roots.push_back(curr);
        curr = curr->right;
    } while (curr != maxNode);

    // Process each root
    for (Node* x : roots) {
        int d = x->degree;

        // Keep linking while there's a tree with same degree
        while (A[d] != NULL) {
            Node* y = A[d];

            // x should have higher priority
            if (x->data.priority < y->data.priority) {
                swap(x, y);
            }

            // Make y a child of x
            link(y, x);
            A[d] = NULL;
            d++;  // x's degree increased
        }
        A[d] = x;
    }

    // Rebuild root list and find new max
    maxNode = NULL;
    for (Node* node : A) {
        if (node != NULL) {
            // Add to root list and update max
            ...
        }
    }
}
```

#### Phase 3: Update maxNode

After consolidation, scan the root list to find the new maximum.

---

### 4. LINK Operation - O(1)

**Goal:** Make one tree a subtree of another.

```cpp
void link(Node* y, Node* x) {
    // Remove y from root list
    y->left->right = y->right;
    y->right->left = y->left;

    // Make y a child of x
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->left = y;
        y->right = y;
    } else {
        // Add y to x's child list
        y->left = x->child;
        y->right = x->child->right;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = false;
}
```

---

## Why "Fibonacci" Heap?

The name comes from the **Fibonacci sequence**: 1, 1, 2, 3, 5, 8, 13, 21...

**Key Property:** A node of degree k has at least F(k+2) descendants, where F(n) is the nth Fibonacci number.

This guarantees that the maximum degree is O(log n), which ensures O(log n) consolidation time.

```
Degree 0: At least F(2) = 1 node   ✓
Degree 1: At least F(3) = 2 nodes  ✓
Degree 2: At least F(4) = 3 nodes  ✓
Degree 3: At least F(5) = 5 nodes  ✓
...
```

---

## Visual Walkthrough: Hospital Scenario

```
SCENARIO: 5 patients arrive, then we treat the most critical

Step 1: INSERT Rajesh (Priority 90)
        Root: [90]
        Max: 90

Step 2: INSERT Priya (Priority 82)
        Root: [90] ◄──► [82]
        Max: 90

Step 3: INSERT Amit (Priority 69)
        Root: [90] ◄──► [82] ◄──► [69]
        Max: 90

Step 4: INSERT Sunita (Priority 93)
        Root: [90] ◄──► [93] ◄──► [82] ◄──► [69]
        Max: 93 (updated!)

Step 5: INSERT Vikram (Priority 44)
        Root: [90] ◄──► [93] ◄──► [44] ◄──► [82] ◄──► [69]
        Max: 93

Step 6: EXTRACT-MAX (Treat Sunita)
        - Remove 93
        - Consolidate remaining trees

        After Consolidate:
        Root: [90°²] ◄──► [69°⁰]
               │
              82°¹
               │
              44

        Max: 90 (Rajesh is next!)
```

---

# Machine Learning Model - In Depth

## What is Machine Learning?

Machine Learning is teaching computers to **learn patterns from data** instead of explicitly programming rules.

```
TRADITIONAL PROGRAMMING:
    Data + Rules → Program → Output

MACHINE LEARNING:
    Data + Output → ML Algorithm → Rules (Model)
```

## Why ML for Hospital Triage?

**Without ML (Rule-based):**
```python
if age > 60 and chest_pain == 1:
    priority = "HIGH"
elif bp > 180:
    priority = "HIGH"
# ... 100s of rules needed!
```

**With ML:**
```python
# Model learns patterns automatically from 10,000 patient records!
priority = model.predict([age, chest_pain, bp, heart_rate])
```

---

## Random Forest Classifier

We use **Random Forest** - an ensemble of Decision Trees.

### What is a Decision Tree?

A decision tree makes decisions by asking yes/no questions:

```
                    ┌─────────────────┐
                    │ Heart Rate > 150?│
                    └────────┬────────┘
                      YES    │    NO
                    ┌────────┴────────┐
                    ▼                 ▼
            ┌───────────────┐  ┌───────────────┐
            │ Chest Pain > 2?│  │   Age > 60?   │
            └───────┬───────┘  └───────┬───────┘
             YES    │   NO      YES    │   NO
            ┌───────┴───────┐  ┌───────┴───────┐
            ▼               ▼  ▼               ▼
        ┌───────┐       ┌───────┐         ┌───────┐
        │ HIGH  │       │MEDIUM │         │  LOW  │
        │ RISK  │       │ RISK  │         │ RISK  │
        └───────┘       └───────┘         └───────┘
```

### What is Random Forest?

**Problem with single tree:** Can overfit (memorize training data)

**Solution:** Build MANY trees, each slightly different, and VOTE!

```
RANDOM FOREST (100 Trees):

Patient Data: age=65, chest_pain=3, bp=180, hr=160

    Tree 1        Tree 2        Tree 3    ...   Tree 100
       │             │             │               │
       ▼             ▼             ▼               ▼
    HIGH          HIGH         MEDIUM           HIGH
    RISK          RISK          RISK            RISK
       │             │             │               │
       └─────────────┴──────┬──────┴───────────────┘
                            │
                      VOTING (Average)
                            │
                            ▼
                    FINAL: HIGH RISK
                    (85 trees said HIGH, 15 said MEDIUM)
```

### Why "Random"?

Each tree is trained on:
1. **Random subset of data** (some patients excluded)
2. **Random subset of features** (not all vitals used per split)

This **diversity** makes the forest robust!

---

## Our Training Pipeline

### Step 1: Load Dataset

```python
# 10,000 real cardiovascular patient records
# Source: Kaggle Cardiovascular Disease Dataset
data = load_csv('datasets/heart_disease_train.csv')
```

**Dataset Sample:**
| patient_id | name | age | cp | trestbps | thalach | target |
|------------|------|-----|----|---------:|--------:|-------:|
| 1 | John Smith | 63 | 3 | 145 | 150 | 1 (High Risk) |
| 2 | Mary Johnson | 37 | 2 | 130 | 187 | 0 (Low Risk) |
| 3 | Robert Williams | 56 | 1 | 120 | 178 | 0 (Low Risk) |

**Column Meanings:**
| Column | Full Name | Description | Range |
|--------|-----------|-------------|-------|
| age | Age | Patient age in years | 20-90 |
| cp | Chest Pain Type | 0=None, 1=Mild, 2=Moderate, 3=Severe, 4=Critical | 0-4 |
| trestbps | Resting Blood Pressure | Systolic BP in mmHg | 90-200 |
| thalach | Maximum Heart Rate | Achieved heart rate in bpm | 60-200 |
| target | Heart Disease | 0=No disease, 1=Has disease | 0/1 |

### Step 2: Feature Selection

We use 4 key features that medical research shows are most predictive:

```python
features = ['age', 'cp', 'trestbps', 'thalach']
target = 'target'

X = data[features]  # Input: Patient vitals
y = data[target]    # Output: Disease risk
```

### Step 3: Split Data

```python
# 80% for training, 20% for testing
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# Training: 8,000 patients
# Testing: 2,000 patients (model never sees these during training!)
```

```
FULL DATASET (10,000 patients)
┌────────────────────────────────────────────────────┐
│                                                    │
│  ┌──────────────────────────────┐ ┌────────────┐  │
│  │     TRAINING SET             │ │  TEST SET  │  │
│  │      (8,000)                 │ │  (2,000)   │  │
│  │                              │ │            │  │
│  │  Model learns from this      │ │ Evaluate   │  │
│  │                              │ │ accuracy   │  │
│  └──────────────────────────────┘ └────────────┘  │
│           80%                         20%          │
└────────────────────────────────────────────────────┘
```

### Step 4: Feature Scaling (Normalization)

**Problem:** Features have different scales
- Age: 20-90 (range of 70)
- Blood Pressure: 90-200 (range of 110)
- Heart Rate: 60-200 (range of 140)

**Solution:** StandardScaler normalizes all features to similar scale

```python
from sklearn.preprocessing import StandardScaler

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)
```

```
BEFORE SCALING:
  age=65     bp=180      hr=150

AFTER SCALING (mean=0, std=1):
  age=0.82   bp=1.45     hr=0.67
```

### Step 5: Train the Model

```python
from sklearn.ensemble import RandomForestClassifier

model = RandomForestClassifier(
    n_estimators=100,    # 100 decision trees
    max_depth=10,        # Maximum tree depth (prevents overfitting)
    random_state=42      # For reproducibility
)

model.fit(X_train_scaled, y_train)
```

**Training Process:**
```
Building Tree 1: Using random 6,000 patients, features [age, cp, hr]
Building Tree 2: Using random 6,000 patients, features [age, bp, hr]
Building Tree 3: Using random 6,000 patients, features [cp, bp, hr]
...
Building Tree 100: Using random 6,000 patients, features [age, cp, bp]

Training Complete!
```

### Step 6: Evaluate Performance

```python
predictions = model.predict(X_test_scaled)
accuracy = accuracy_score(y_test, predictions)
print(f"Accuracy: {accuracy * 100:.2f}%")  # 83.05%
```

**Confusion Matrix:**
```
                    PREDICTED
                 Low Risk   High Risk
              ┌───────────┬───────────┐
    Low Risk  │    847    │    173    │  → 83% correct
ACTUAL        ├───────────┼───────────┤
    High Risk │    166    │    814    │  → 83% correct
              └───────────┴───────────┘

True Positives (TP):  814 - Correctly identified high risk
True Negatives (TN):  847 - Correctly identified low risk
False Positives (FP): 173 - Wrongly flagged as high risk
False Negatives (FN): 166 - Missed high risk patients
```

### Step 7: Feature Importance

Random Forest tells us which features matter most:

```python
importances = model.feature_importances_
```

```
FEATURE IMPORTANCE:

thalach (Heart Rate) : 41.1%  ████████████████████░░░░░░░░░░
cp (Chest Pain)      : 34.3%  █████████████████░░░░░░░░░░░░░
trestbps (BP)        : 14.9%  ███████░░░░░░░░░░░░░░░░░░░░░░░
age                  :  9.8%  █████░░░░░░░░░░░░░░░░░░░░░░░░░

Key Insight: Heart rate and chest pain are the strongest
predictors of cardiovascular disease!
```

### Step 8: Save Model

```python
import pickle

model_data = {
    'model': model,
    'scaler': scaler
}
with open('trained_model.pkl', 'wb') as f:
    pickle.dump(model_data, f)
```

---

## How Priority Score is Calculated

When a new patient arrives:

```python
def predict_priority(age, chest_pain, bp, heart_rate):
    # Load trained model
    with open('trained_model.pkl', 'rb') as f:
        model_data = pickle.load(f)

    model = model_data['model']
    scaler = model_data['scaler']

    # Prepare input
    patient = [[age, chest_pain, bp, heart_rate]]
    patient_scaled = scaler.transform(patient)

    # Get probability of high risk
    probabilities = model.predict_proba(patient_scaled)[0]
    # probabilities = [P(low risk), P(high risk)]
    # Example: [0.15, 0.85] = 15% low risk, 85% high risk

    high_risk_prob = probabilities[1]

    # Convert to priority score (25-100)
    base_score = high_risk_prob * 60  # ML contributes 0-60 points

    # Clinical adjustments (0-40 points)
    age_factor = min(age / 100 * 15, 15)           # Max 15
    bp_factor = min((bp - 120) / 80 * 12, 12)      # Max 12
    hr_factor = min((heart_rate - 80) / 100 * 8, 8) # Max 8
    cp_factor = chest_pain * 1.5                    # Max 6

    priority = int(base_score + age_factor + bp_factor + hr_factor + cp_factor)

    return max(25, min(100, priority))  # Clamp to 25-100
```

**Example Calculation:**
```
Patient: age=70, chest_pain=4, bp=185, heart_rate=165

ML Prediction: P(high risk) = 0.92 (92%)

Base Score:    0.92 × 60 = 55.2
Age Factor:    70/100 × 15 = 10.5
BP Factor:     (185-120)/80 × 12 = 9.75
HR Factor:     (165-80)/100 × 8 = 6.8
CP Factor:     4 × 1.5 = 6.0

Total: 55.2 + 10.5 + 9.75 + 6.8 + 6.0 = 88.25

Final Priority Score: 88 (HIGH - treat soon!)
```

---

## ML Model Summary

```
┌─────────────────────────────────────────────────────────────┐
│                    ML TRAINING PIPELINE                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐                                            │
│  │ 10,000      │                                            │
│  │ Patient     │──┐                                         │
│  │ Records     │  │                                         │
│  └─────────────┘  │                                         │
│                   ▼                                         │
│  ┌─────────────────────────────────────┐                   │
│  │         FEATURE EXTRACTION          │                   │
│  │  age, chest_pain, bp, heart_rate    │                   │
│  └──────────────────┬──────────────────┘                   │
│                     ▼                                       │
│  ┌─────────────────────────────────────┐                   │
│  │         STANDARD SCALER             │                   │
│  │      Normalize to mean=0, std=1     │                   │
│  └──────────────────┬──────────────────┘                   │
│                     ▼                                       │
│  ┌─────────────────────────────────────┐                   │
│  │       RANDOM FOREST TRAINING        │                   │
│  │         100 Decision Trees          │                   │
│  │          Max Depth = 10             │                   │
│  └──────────────────┬──────────────────┘                   │
│                     ▼                                       │
│  ┌─────────────────────────────────────┐                   │
│  │         trained_model.pkl           │                   │
│  │      Accuracy: 83.05%               │                   │
│  └─────────────────────────────────────┘                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    ML PREDICTION FLOW                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  New Patient                                                │
│  ┌─────────────────┐                                        │
│  │ age=70          │                                        │
│  │ chest_pain=4    │─────┐                                  │
│  │ bp=185          │     │                                  │
│  │ heart_rate=165  │     │                                  │
│  └─────────────────┘     │                                  │
│                          ▼                                  │
│                 ┌─────────────────┐                         │
│                 │ trained_model   │                         │
│                 │    .pkl         │                         │
│                 └────────┬────────┘                         │
│                          │                                  │
│            ┌─────────────┴─────────────┐                    │
│            ▼                           ▼                    │
│    ┌──────────────┐           ┌──────────────┐             │
│    │ ML Probability│           │  Clinical    │             │
│    │ 92% high risk │           │  Adjustments │             │
│    │ = 55.2 pts    │           │  = 33 pts    │             │
│    └──────────────┘           └──────────────┘             │
│            │                           │                    │
│            └───────────┬───────────────┘                    │
│                        ▼                                    │
│               ┌──────────────┐                              │
│               │ Priority: 88 │                              │
│               │  (HIGH RISK) │                              │
│               └──────────────┘                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
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
│  │  Patient: Sunita (Priority 93, Cardiology)               │  │
│  │         ┌──────────┴──────────┐                          │  │
│  │         ▼                     ▼                          │  │
│  │  ┌─────────────┐      ┌─────────────┐                    │  │
│  │  │ Free Doctor │      │ Preemption  │                    │  │
│  │  │   Found     │      │ (if higher  │                    │  │
│  │  └──────┬──────┘      │  priority)  │                    │  │
│  │         ▼             └─────────────┘                    │  │
│  │  Assigned to Dr. Smith                                   │  │
│  └──────────────────────────────────────────────────────────┘  │
│                          │                                      │
│                          ▼                                      │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │              GRAPHVIZ VISUALIZATION                       │  │
│  │  heap.dot ──► dot command ──► heap.png ──► viewer.html   │  │
│  └──────────────────────────────────────────────────────────┘  │
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

### Installation

**Windows:**
```cmd
pip install numpy scikit-learn
# Install Graphviz from graphviz.org and add to PATH
```

**Linux:**
```bash
sudo apt-get install g++ python3 python3-pip graphviz
pip3 install numpy scikit-learn
```

---

## Usage Guide

### Quick Start
```cmd
cd Version6_Simple
run.bat
```

### Manual Steps
```cmd
# 1. Train ML Model
python train_model.py

# 2. Compile
g++ -o hospital_triage hospital_triage.cpp

# 3. Open viewer.html in browser

# 4. Run
./hospital_triage
```

---

## Time Complexity Analysis

### Fibonacci Heap Operations

| Operation | Time | Description |
|-----------|------|-------------|
| Insert | O(1) | Add to root list |
| Find-Max | O(1) | Return maxNode |
| Extract-Max | O(log n)* | Remove + consolidate |
| Decrease-Key | O(1)* | Update priority |

*Amortized

### Comparison

| Structure | Insert | Extract-Max | Best For |
|-----------|--------|-------------|----------|
| Array | O(1) | O(n) | Few extracts |
| Binary Heap | O(log n) | O(log n) | General |
| **Fibonacci Heap** | **O(1)** | O(log n) | Many inserts |

---

## File Structure

```
├── README.md              # This documentation
├── hospital_triage.cpp    # Main C++ program
├── train_model.py         # ML training script
├── triage.py              # ML prediction script
├── viewer.html            # Real-time visualization
├── run.bat                # Windows runner
├── patients_data.csv      # 12 demo patients
├── doctors_data.csv       # 6 doctors
├── trained_model.pkl      # Saved ML model
└── datasets/
    ├── heart_disease_train.csv   # 10K training data
    └── cardio_raw.csv            # Raw 70K dataset
```

---

## References

1. Fredman & Tarjan (1987) - Fibonacci Heaps and Their Uses in Improved Network Optimization Algorithms
2. CLRS - Introduction to Algorithms, Chapter 19
3. Kaggle Cardiovascular Disease Dataset
4. scikit-learn Random Forest Documentation
