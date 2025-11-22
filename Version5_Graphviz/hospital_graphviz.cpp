/*
 * Hospital Triage System using Fibonacci Heap
 * With Graphviz Visualization
 *
 * This program generates a .dot file that can be viewed
 * in VS Code with Graphviz extension to see the heap structure
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <set>

using namespace std;

// Global: Track IDs of newly arrived patients (for visualization)
set<int> newPatientIds;

// ============== PATIENT STRUCTURE ==============
struct Patient {
    int id;
    string name;
    int age;
    int chest_pain;
    int bp;
    int heart_rate;
    string department;
    int priority;  // ML score
};

// ============== DOCTOR STRUCTURE ==============
struct Doctor {
    int id;
    string name;
    string department;
    bool busy;
    string currentPatient;
    int currentPatientPriority;
    Patient currentPatientData;  // Store full patient data for preemption
    int treatmentTime;  // How many iterations doctor has been treating
};

// ============== FIBONACCI HEAP NODE ==============
struct Node {
    Patient data;
    int degree;
    Node* parent;
    Node* child;
    Node* left;
    Node* right;
    bool mark;

    Node(Patient p) {
        data = p;
        degree = 0;
        parent = NULL;
        child = NULL;
        left = this;
        right = this;
        mark = false;
    }
};

// ============== FIBONACCI HEAP CLASS ==============
class FibonacciHeap {
public:
    Node* maxNode;
    int count;

    FibonacciHeap() {
        maxNode = NULL;
        count = 0;
    }

    // Check if heap is empty
    bool isEmpty() {
        return maxNode == NULL;
    }

    // Get max without removing
    Patient getMax() {
        if (maxNode != NULL) {
            return maxNode->data;
        }
        Patient empty;
        empty.id = -1;
        return empty;
    }

    // INSERT operation - O(1)
    void insert(Patient p) {
        Node* newNode = new Node(p);

        if (maxNode == NULL) {
            maxNode = newNode;
        } else {
            // Add to root list (circular doubly linked)
            newNode->left = maxNode;
            newNode->right = maxNode->right;
            maxNode->right->left = newNode;
            maxNode->right = newNode;

            // Update max if needed
            if (newNode->data.priority > maxNode->data.priority) {
                maxNode = newNode;
            }
        }
        count++;

        // DEBUG: Verify circular list integrity
        int listCount = 0;
        Node* curr = maxNode;
        do {
            listCount++;
            curr = curr->right;
        } while (curr != maxNode && listCount <= count + 1);

        if (listCount != count) {
            cout << "   WARNING: List has " << listCount << " nodes but count=" << count << endl;
        }
    }

    // EXTRACT-MAX operation - O(log n) amortized
    Patient extractMax() {
        if (maxNode == NULL) {
            Patient empty;
            empty.id = -1;
            return empty;
        }

        Node* z = maxNode;

        // Add all children to root list
        if (z->child != NULL) {
            Node* child = z->child;
            do {
                Node* nextChild = child->right;

                // Remove from child list
                child->left->right = child->right;
                child->right->left = child->left;

                // Add to root list
                child->left = maxNode;
                child->right = maxNode->right;
                maxNode->right->left = child;
                maxNode->right = child;
                child->parent = NULL;

                child = nextChild;
            } while (child != z->child);
        }

        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            maxNode = NULL;
        } else {
            maxNode = z->right;
            consolidate();  // This is where the magic happens!
        }

        count--;
        return z->data;
    }

    // CONSOLIDATE - combines trees of same degree
    void consolidate() {
        if (maxNode == NULL) return;

        // Array to track trees by degree
        int maxDegree = (int)(log(count + 1) / log(1.618)) + 2;
        vector<Node*> degreeTable(maxDegree + 5, NULL);

        // Collect all root nodes
        vector<Node*> roots;
        Node* curr = maxNode;
        do {
            roots.push_back(curr);
            curr = curr->right;
        } while (curr != maxNode);

        // Process each root
        for (int i = 0; i < roots.size(); i++) {
            Node* x = roots[i];
            int d = x->degree;

            // Combine trees with same degree
            while (degreeTable[d] != NULL) {
                Node* y = degreeTable[d];

                // Make sure x has higher priority
                if (x->data.priority < y->data.priority) {
                    Node* temp = x;
                    x = y;
                    y = temp;
                }

                // Link y under x
                linkNodes(y, x);
                degreeTable[d] = NULL;
                d++;
            }
            degreeTable[d] = x;
        }

        // Rebuild root list and find new max
        maxNode = NULL;
        for (int i = 0; i < degreeTable.size(); i++) {
            if (degreeTable[i] != NULL) {
                Node* node = degreeTable[i];
                if (maxNode == NULL) {
                    maxNode = node;
                    node->left = node;
                    node->right = node;
                } else {
                    node->left = maxNode;
                    node->right = maxNode->right;
                    maxNode->right->left = node;
                    maxNode->right = node;

                    if (node->data.priority > maxNode->data.priority) {
                        maxNode = node;
                    }
                }
            }
        }
    }

    // Link node y as child of node x
    void linkNodes(Node* y, Node* x) {
        // Remove y from root list
        y->left->right = y->right;
        y->right->left = y->left;

        // Make y child of x
        y->parent = x;
        if (x->child == NULL) {
            x->child = y;
            y->left = y;
            y->right = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }
        x->degree++;
        y->mark = false;
    }

    // ============== GRAPHVIZ DOT FILE GENERATION ==============
    void generateDotFile(string filename) {
        ofstream out(filename.c_str());

        out << "digraph FibonacciHeap {" << endl;
        out << "  rankdir=TB;" << endl;
        out << "  bgcolor=white;" << endl;
        out << "  nodesep=0.3;" << endl;
        out << "  ranksep=0.5;" << endl;
        out << "  node [shape=box, style=\"filled,rounded\", fillcolor=\"#4A90D9\", fontcolor=white, fontsize=10, width=0.6, height=0.4];" << endl;
        out << "  edge [penwidth=1.5, color=\"#333333\"];" << endl;
        out << endl;

        if (maxNode == NULL) {
            out << "  empty [label=\"Empty Heap\", fillcolor=\"#90EE90\", fontcolor=black, fontsize=14];" << endl;
            out << "}" << endl;
            out.close();
            cout << "   [DOT file: 0 patients, 0 trees]" << endl;
            return;
        }

        // First, collect all root nodes by traversing circular list
        vector<Node*> rootNodes;
        Node* curr = maxNode;
        int safetyCounter = 0;
        do {
            rootNodes.push_back(curr);
            curr = curr->right;
            safetyCounter++;
            if (safetyCounter > count + 5) {
                cout << "ERROR: Circular list broken! Found " << safetyCounter << " but count=" << count << endl;
                break;
            }
        } while (curr != maxNode && curr != NULL);

        // Title showing count
        out << "  title [label=\"FIBONACCI HEAP\\n" << count << " patients | " << rootNodes.size() << " trees\", shape=box, fillcolor=\"#333333\", fontcolor=white, fontsize=11];" << endl;
        out << endl;

        // Draw all root nodes first (define them)
        for (int i = 0; i < rootNodes.size(); i++) {
            drawNode(out, rootNodes[i]);
        }
        out << endl;

        // Connect title to first few roots for layout
        out << "  title -> node" << rootNodes[0]->data.id << " [style=invis];" << endl;
        out << endl;

        // Create invisible edges between root nodes to keep them in a row
        out << "  { rank=same;" << endl;
        for (int i = 0; i < rootNodes.size(); i++) {
            out << "    node" << rootNodes[i]->data.id << ";" << endl;
        }
        out << "  }" << endl;
        out << endl;

        // Add invisible edges between consecutive root nodes for ordering
        for (int i = 0; i < rootNodes.size() - 1; i++) {
            out << "  node" << rootNodes[i]->data.id << " -> node" << rootNodes[i+1]->data.id << " [style=invis];" << endl;
        }
        out << endl;

        // Highlight max node in red
        out << "  node" << maxNode->data.id << " [fillcolor=\"#E74C3C\", fontcolor=white, penwidth=2];" << endl;

        out << "}" << endl;
        out.close();

        // Debug: print how many nodes written
        cout << "   [DOT file: " << count << " patients, " << rootNodes.size() << " root trees found]" << endl;
    }

    void drawNode(ofstream& out, Node* n) {
        // Draw this node with compact label
        string shortName = n->data.name;
        // Get first name only (before space)
        size_t spacePos = shortName.find(' ');
        if (spacePos != string::npos) {
            shortName = shortName.substr(0, spacePos);
        }
        if (shortName.length() > 8) {
            shortName = shortName.substr(0, 8);
        }

        // Check if this is a new patient (highlight in orange)
        if (newPatientIds.find(n->data.id) != newPatientIds.end()) {
            out << "  node" << n->data.id << " [label=\"NEW!\\n" << shortName << "\\n" << n->data.priority << "\", fillcolor=\"#FF9500\", fontcolor=black];" << endl;
        } else {
            out << "  node" << n->data.id << " [label=\"" << shortName << "\\n" << n->data.priority << "\"];" << endl;
        }

        // Draw children
        if (n->child != NULL) {
            Node* child = n->child;
            do {
                out << "  node" << n->data.id << " -> node" << child->data.id << ";" << endl;
                drawNode(out, child);
                child = child->right;
            } while (child != n->child);
        }
    }
};

// ============== UTILITY FUNCTIONS ==============

// Call Python ML model to get priority score
int getMLScore(int age, int chest_pain, int bp, int heart_rate) {
    stringstream cmd;
    cmd << "python triage.py " << age << " " << chest_pain << " " << bp << " " << heart_rate;
    cmd << " > ml_temp.txt";

    system(cmd.str().c_str());

    ifstream f("ml_temp.txt");
    int score = 50;  // default
    if (f.is_open()) {
        f >> score;
        f.close();
    }
    remove("ml_temp.txt");

    return score;
}

// Load patients from CSV file
vector<Patient> loadPatients(string filename) {
    vector<Patient> patients;
    ifstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return patients;
    }

    string line;
    getline(file, line);  // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Patient p;

        getline(ss, token, ','); p.id = atoi(token.c_str());
        getline(ss, token, ','); p.name = token;
        getline(ss, token, ','); p.age = atoi(token.c_str());
        getline(ss, token, ','); // skip gender
        getline(ss, token, ','); p.department = token;
        getline(ss, token, ','); p.chest_pain = atoi(token.c_str());
        getline(ss, token, ','); // skip diabetes
        getline(ss, token, ','); p.bp = atoi(token.c_str());
        getline(ss, token, ','); // skip cholesterol
        getline(ss, token, ','); // skip bmi
        getline(ss, token, ','); p.heart_rate = atoi(token.c_str());

        p.priority = 0;  // Will be set by ML
        patients.push_back(p);
    }

    file.close();
    return patients;
}

// Load doctors from CSV file
vector<Doctor> loadDoctors(string filename) {
    vector<Doctor> doctors;
    ifstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return doctors;
    }

    string line;
    getline(file, line);  // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Doctor d;

        getline(ss, token, ','); d.id = atoi(token.c_str());
        getline(ss, token, ','); d.name = token;
        getline(ss, token, ','); d.department = token;

        d.busy = false;
        d.currentPatient = "";
        d.currentPatientPriority = 0;
        d.treatmentTime = 0;
        doctors.push_back(d);
    }

    file.close();
    return doctors;
}

// Find a FREE doctor in the specified department
// Returns index of doctor, or -1 if none free
int findFreeDoctorInDept(vector<Doctor>& doctors, string department) {
    for (int i = 0; i < doctors.size(); i++) {
        if (doctors[i].department == department && !doctors[i].busy) {
            return i;
        }
    }
    return -1;
}

// Find doctor in department treating LOWEST priority patient (for preemption)
// Returns index of doctor, or -1 if none found
int findLowestPriorityDoctorInDept(vector<Doctor>& doctors, string department) {
    int lowestIdx = -1;
    int lowestPriority = 999;

    for (int i = 0; i < doctors.size(); i++) {
        if (doctors[i].department == department && doctors[i].busy) {
            if (doctors[i].currentPatientPriority < lowestPriority) {
                lowestPriority = doctors[i].currentPatientPriority;
                lowestIdx = i;
            }
        }
    }
    return lowestIdx;
}

// Clear screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Wait for user to press Enter
void waitForEnter(string msg = "Press ENTER to continue...") {
    cout << "\n" << msg;
    cin.get();
}

// Display Doctor Status Table
void showDoctorStatus(vector<Doctor>& doctors) {
    cout << "+--------------------------------------------------------------+" << endl;
    cout << "|                     DOCTORS STATUS                           |" << endl;
    cout << "+--------------------------------------------------------------+" << endl;

    for (int i = 0; i < doctors.size(); i++) {
        cout << "| Dr. " << doctors[i].name;

        // Pad name to fixed width
        int padding = 10 - doctors[i].name.length();
        for (int j = 0; j < padding; j++) cout << " ";

        cout << "| " << doctors[i].department;

        // Pad department
        padding = 12 - doctors[i].department.length();
        for (int j = 0; j < padding; j++) cout << " ";

        if (doctors[i].busy) {
            // Show patient name and priority
            stringstream status;
            status << doctors[i].currentPatient << " (" << doctors[i].currentPatientPriority << ")";
            cout << "| BUSY -> " << status.str();
            padding = 18 - status.str().length();
            for (int j = 0; j < padding; j++) cout << " ";
        } else {
            cout << "| FREE                       ";
        }
        cout << "|" << endl;
    }

    cout << "+--------------------------------------------------------------+" << endl;
}

// ============== MAIN PROGRAM ==============
int main() {
    FibonacciHeap heap;

    // Load data from files
    cout << "========================================" << endl;
    cout << "   HOSPITAL TRIAGE SYSTEM" << endl;
    cout << "   (Fibonacci Heap + ML + Graphviz)" << endl;
    cout << "========================================" << endl;
    cout << endl;

    cout << "Loading patients from CSV..." << endl;
    vector<Patient> patients = loadPatients("patients_data.csv");
    cout << "Loaded " << patients.size() << " patients" << endl;

    cout << "Loading doctors from CSV..." << endl;
    vector<Doctor> doctors = loadDoctors("doctors_data.csv");
    cout << "Loaded " << doctors.size() << " doctors" << endl;

    if (patients.empty() || doctors.empty()) {
        cout << "Error loading data files!" << endl;
        return 1;
    }

    waitForEnter("\nPress ENTER to start ML scoring...");

    // Calculate ML scores for all patients
    int numPatients = (int)patients.size();

    cout << "\n--- ML TRIAGE SCORING ---" << endl;
    for (int i = 0; i < numPatients; i++) {
        patients[i].priority = getMLScore(
            patients[i].age,
            patients[i].chest_pain,
            patients[i].bp,
            patients[i].heart_rate
        );
        cout << "Patient " << patients[i].id << " (" << patients[i].name << "): Score = " << patients[i].priority << endl;
    }

    waitForEnter("\nPress ENTER to insert patients into Fibonacci Heap...");

    // Insert patients into heap
    cout << "\n--- INSERTING INTO FIBONACCI HEAP ---" << endl;
    for (int i = 0; i < numPatients; i++) {
        heap.insert(patients[i]);
        cout << "Inserted: " << patients[i].name << " (Priority: " << patients[i].priority << ")" << endl;

        // Generate dot file after each insert
        heap.generateDotFile("heap_state.dot");
    }

    cout << "\nHeap built with " << heap.count << " patients" << endl;
    cout << "Max priority patient: " << heap.getMax().name << " (Score: " << heap.getMax().priority << ")" << endl;

    cout << "\n>> Open 'heap_state.dot' in VS Code with Graphviz Preview" << endl;
    cout << ">> You will see the tree structure!" << endl;

    waitForEnter("\nPress ENTER to start doctor allocation simulation...");

    // Simulation loop
    int iteration = 1;
    int treated = 0;
    bool newPatientAdded = false;

    while (!heap.isEmpty()) {
        clearScreen();

        cout << "========================================" << endl;
        cout << "   ITERATION " << iteration << endl;
        cout << "========================================" << endl;
        cout << endl;

        // Free doctors who have finished treatment (after 2 iterations)
        cout << "--- CHECKING TREATMENT COMPLETION ---" << endl;
        bool anyFreed = false;
        for (int i = 0; i < doctors.size(); i++) {
            if (doctors[i].busy) {
                doctors[i].treatmentTime++;
                if (doctors[i].treatmentTime >= 2) {
                    cout << ">> Dr. " << doctors[i].name << " finished treating " << doctors[i].currentPatient << endl;
                    doctors[i].busy = false;
                    doctors[i].currentPatient = "";
                    doctors[i].currentPatientPriority = 0;
                    doctors[i].treatmentTime = 0;
                    anyFreed = true;
                }
            }
        }
        if (!anyFreed) {
            cout << "   (No treatments completed this iteration)" << endl;
        }
        cout << endl;

        // NEW PATIENT ARRIVAL at iteration 5
        if (iteration == 5 && !newPatientAdded) {
            cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
            cout << "!         NEW EMERGENCY PATIENT ARRIVED!          !" << endl;
            cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
            cout << endl;

            Patient newP;
            newP.id = 999;
            newP.name = "CRITICAL-X";
            newP.age = 55;
            newP.chest_pain = 4;
            newP.bp = 200;
            newP.heart_rate = 180;
            newP.department = "Cardiology";
            newP.priority = 95;  // Very high priority!

            cout << "Patient: " << newP.name << endl;
            cout << "Department: " << newP.department << endl;
            cout << "ML Score: " << newP.priority << " (CRITICAL!)" << endl;
            cout << endl;

            cout << ">> INSERT: Adding " << newP.name << " to Fibonacci Heap (O(1) operation!)" << endl;
            heap.insert(newP);
            newPatientIds.insert(newP.id);  // Mark as new for visualization

            newPatientAdded = true;

            // Update visualization immediately
            heap.generateDotFile("heap_state.dot");

            cout << endl;
            cout << ">> Check browser - NEW patient shown in ORANGE!" << endl;
            waitForEnter("\nPress ENTER to continue with allocation...");
            clearScreen();

            cout << "========================================" << endl;
            cout << "   ITERATION " << iteration << " (continued)" << endl;
            cout << "========================================" << endl;
            cout << endl;
        }

        // Show Doctor Status Table
        showDoctorStatus(doctors);
        cout << endl;

        // Show heap status
        cout << "Patients waiting: " << heap.count << endl;
        if (!heap.isEmpty()) {
            Patient maxP = heap.getMax();
            cout << "Highest priority: " << maxP.name << " (" << maxP.department << ", Score: " << maxP.priority << ")" << endl;
        }
        cout << endl;

        // Extract max patient
        cout << "--- EXTRACTING HIGHEST PRIORITY PATIENT ---" << endl;
        cout << "Calling EXTRACT-MAX on Fibonacci Heap..." << endl;
        Patient p = heap.extractMax();
        cout << "Extracted: " << p.name << " (Dept: " << p.department << ", Score: " << p.priority << ")" << endl;

        // Remove from newPatientIds if it was there
        newPatientIds.erase(p.id);

        cout << endl;

        // Find FREE doctor in patient's department
        cout << "--- FINDING " << p.department << " DOCTOR ---" << endl;
        int freeDocIdx = findFreeDoctorInDept(doctors, p.department);

        if (freeDocIdx != -1) {
            // FREE doctor found - direct assignment
            Doctor& doc = doctors[freeDocIdx];
            cout << ">> Dr. " << doc.name << " (" << doc.department << ") is FREE!" << endl;
            cout << endl;

            // Assign patient to doctor
            doc.busy = true;
            doc.currentPatient = p.name;
            doc.currentPatientPriority = p.priority;
            doc.currentPatientData = p;
            doc.treatmentTime = 0;
            treated++;

            cout << ">> ASSIGNED: " << p.name << " -> Dr. " << doc.name << endl;

        } else {
            // No free doctor - check for preemption
            cout << ">> No FREE " << p.department << " doctor available!" << endl;
            cout << endl;

            int lowestDocIdx = findLowestPriorityDoctorInDept(doctors, p.department);

            if (lowestDocIdx != -1) {
                Doctor& doc = doctors[lowestDocIdx];

                cout << "--- PREEMPTION CHECK ---" << endl;
                cout << "Dr. " << doc.name << " is treating " << doc.currentPatient;
                cout << " (Priority: " << doc.currentPatientPriority << ")" << endl;
                cout << "New patient " << p.name << " has Priority: " << p.priority << endl;

                if (p.priority > doc.currentPatientPriority) {
                    // PREEMPT!
                    cout << endl;
                    cout << ">> PREEMPTION TRIGGERED! <<" << endl;
                    cout << ">> " << p.name << " (" << p.priority << ") > " << doc.currentPatient << " (" << doc.currentPatientPriority << ")" << endl;

                    // Re-insert the preempted patient back into heap
                    Patient preempted = doc.currentPatientData;
                    cout << ">> " << preempted.name << " treatment PAUSED - returning to queue" << endl;
                    heap.insert(preempted);
                    cout << ">> INSERT: " << preempted.name << " back into Fibonacci Heap (O(1) operation!)" << endl;

                    // Assign new patient
                    doc.currentPatient = p.name;
                    doc.currentPatientPriority = p.priority;
                    doc.currentPatientData = p;
                    doc.treatmentTime = 0;
                    treated++;

                    cout << endl;
                    cout << ">> ASSIGNED: " << p.name << " -> Dr. " << doc.name << endl;

                } else {
                    // Can't preempt - patient waits, will be assigned when doctor is free
                    cout << endl;
                    cout << ">> Cannot preempt - current patient has HIGHER priority" << endl;
                    cout << ">> " << p.name << " will wait for next available doctor" << endl;
                    heap.insert(p);  // Put back in queue
                }
            }
        }

        // Update dot file
        heap.generateDotFile("heap_state.dot");

        cout << endl;

        // Show updated doctor status
        cout << "--- UPDATED STATUS ---" << endl;
        showDoctorStatus(doctors);

        cout << endl;
        cout << ">> Browser will show updated heap structure" << endl;

        if (!heap.isEmpty()) {
            waitForEnter("\nPress ENTER for next allocation...");
        }

        iteration++;

        // Safety: prevent infinite loop (max 50 iterations)
        if (iteration > 50) {
            cout << "\n[MAX ITERATIONS REACHED - ENDING SIMULATION]" << endl;
            break;
        }
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "   ALL PATIENTS TREATED!" << endl;
    cout << "========================================" << endl;
    cout << "Total patients treated: " << treated << endl;
    cout << "Total iterations: " << (iteration - 1) << endl;
    cout << endl;

    // Show final doctor status (all free now)
    for (int i = 0; i < doctors.size(); i++) {
        doctors[i].busy = false;
        doctors[i].currentPatient = "";
    }
    showDoctorStatus(doctors);

    // Final empty heap
    heap.generateDotFile("heap_state.dot");

    waitForEnter("\nPress ENTER to exit...");

    return 0;
}
