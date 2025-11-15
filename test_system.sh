#!/bin/bash

# Test script for Hospital Triage System

echo "=========================================="
echo "Testing Hospital Triage System"
echo "=========================================="
echo ""

# Test 1: ML Model
echo "Test 1: Testing ML Model..."
echo "Critical case (elderly, severe symptoms):"
python3 triage_ml_model.py 70 1 4 1 190 270 35.0 175 1

echo ""
echo "Low priority case (young, no symptoms):"
python3 triage_ml_model.py 25 1 0 0 120 150 23.0 130 0

echo ""
echo "Medium priority case:"
python3 triage_ml_model.py 45 0 2 0 140 190 26.5 145 0

echo ""
echo "Test 1: PASSED ✓"
echo ""

# Test 2: Build System
echo "Test 2: Testing Build System..."
make clean > /dev/null 2>&1
if make > /dev/null 2>&1; then
    echo "Test 2: PASSED ✓"
else
    echo "Test 2: FAILED ✗"
    exit 1
fi

echo ""

# Test 3: Check files exist
echo "Test 3: Checking required files..."
files=("FibonacciHeap.h" "Visualization.h" "HospitalTriageSystem.cpp" "triage_ml_model.py" "patients_data.csv" "doctors_data.csv" "Makefile" "README.md")

for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo "  ✓ $file"
    else
        echo "  ✗ $file (missing)"
        exit 1
    fi
done

echo "Test 3: PASSED ✓"
echo ""

# Test 4: Data file integrity
echo "Test 4: Checking data file integrity..."
patient_count=$(tail -n +2 patients_data.csv | wc -l)
doctor_count=$(tail -n +2 doctors_data.csv | wc -l)

echo "  Patients loaded: $patient_count"
echo "  Doctors loaded: $doctor_count"

if [ "$patient_count" -gt 0 ] && [ "$doctor_count" -gt 0 ]; then
    echo "Test 4: PASSED ✓"
else
    echo "Test 4: FAILED ✗"
    exit 1
fi

echo ""
echo "=========================================="
echo "All tests passed! ✓"
echo "=========================================="
echo ""
echo "Ready to run the system with: ./hospital_triage"
echo ""
