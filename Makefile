# Makefile for Hospital Triage System with Fibonacci Heap

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = hospital_triage
SOURCES = HospitalTriageSystem.cpp
HEADERS = FibonacciHeap.h Visualization.h
PYTHON = python3

# Main build target
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@echo "Compiling Hospital Triage System..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
	@echo "Build successful! Executable: ./$(TARGET)"

# Check Python dependencies
check-deps:
	@echo "Checking Python dependencies..."
	@$(PYTHON) -c "import numpy; import sklearn" 2>/dev/null || \
		(echo "Missing dependencies! Installing..."; \
		 pip3 install numpy scikit-learn 2>/dev/null || \
		 echo "Please install: pip3 install numpy scikit-learn")

# Run the system
run: $(TARGET) check-deps
	@echo "Running Hospital Triage System..."
	@./$(TARGET)

# Generate visualization graph (requires gnuplot)
plot: priority_data.csv
	@echo "Generating visualization graph..."
	@gnuplot plot_triage.gnu 2>/dev/null && echo "Graph saved: triage_visualization.png" || \
		echo "gnuplot not found. Install with: sudo apt-get install gnuplot"

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f $(TARGET)
	rm -f triage_log.txt
	rm -f priority_data.csv
	rm -f plot_triage.gnu
	rm -f triage_visualization.png
	@echo "Clean complete!"

# Test ML model separately
test-ml:
	@echo "Testing ML model with sample patient..."
	$(PYTHON) triage_ml_model.py 65 1 4 1 180 250 32.5 180 1

# Display help
help:
	@echo "Hospital Triage System - Makefile Commands"
	@echo "=========================================="
	@echo "make              - Build the project"
	@echo "make run          - Build and run the system"
	@echo "make plot         - Generate visualization graph"
	@echo "make test-ml      - Test ML model"
	@echo "make clean        - Remove build files"
	@echo "make check-deps   - Check Python dependencies"
	@echo "make help         - Show this help message"

.PHONY: all run clean test-ml check-deps plot help
