# Version 5: Graphviz Visualization

This version shows the **actual Fibonacci Heap tree structure** using Graphviz!

## Prerequisites

### 1. Install Graphviz
- Download from: https://graphviz.org/download/
- **Important:** During install, check "Add Graphviz to system PATH"

### 2. VS Code Extension (for real-time preview)
- Open VS Code
- Go to Extensions (Ctrl+Shift+X)
- Search: "Graphviz Preview" or "Graphviz (dot) language support"
- Install it

## How to Run

### Step 1: Compile and Run
```cmd
cd Version5_Graphviz
g++ hospital_graphviz.cpp -o hospital.exe
hospital.exe
```

Or just double-click `RUN.bat`

### Step 2: View the Tree Structure

**In VS Code:**
1. Open the `Version5_Graphviz` folder in VS Code
2. Open `heap_state.dot` file
3. Press `Ctrl+Shift+V` to open preview (or right-click -> "Open Preview")
4. You'll see the tree diagram on the right side!

**Alternative (if VS Code doesn't work):**
```cmd
dot -Tpng heap_state.dot -o heap_image.png
start heap_image.png
```

## How to Demo

### Setup (Before Demo):
1. Open VS Code
2. Open terminal on the LEFT side
3. Open `heap_state.dot` preview on the RIGHT side
4. Arrange so you can see both

### During Demo:
1. Run the program in terminal
2. Press ENTER to advance each step
3. Watch the tree diagram change on the right!
4. Explain: "See how the trees merge during consolidation?"

## What to Say to Your Guide

**When inserting:**
> "Each patient is inserted as a new tree in O(1) time. See how there are multiple trees in the root list?"

**When extracting max:**
> "Now we remove the max (red node). Watch the consolidation..."
> "See how trees of the same degree are merged together? This is the key property of Fibonacci Heap that gives us O(log n) amortized time."

**About the structure:**
> "The red node is always the maximum. Children are linked in a circular doubly-linked list. This allows O(1) insertion."

## Files in This Folder

- `hospital_graphviz.cpp` - Main C++ code
- `triage.py` - ML scoring model
- `patients_data.csv` - Patient data
- `doctors_data.csv` - Doctor data
- `heap_state.dot` - Generated tree (view in VS Code)
- `RUN.bat` - One-click run

## Troubleshooting

**"g++ not recognized"**
- Install MinGW: https://winlibs.com/

**"VS Code preview not showing"**
- Make sure Graphviz extension is installed
- Try: View -> Command Palette -> "Graphviz: Open Preview"

**"dot command not found"**
- Graphviz not in PATH. Reinstall and check "Add to PATH"

## Why This Version is Good for Demo

1. **Proves the algorithm works** - You can SEE the tree structure
2. **Shows consolidation** - The key Fibonacci Heap operation
3. **Interactive** - Press ENTER to control pace
4. **Simple code** - Easy to explain if asked
5. **Visual proof** - Professor can't argue it's fake!
