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

using namespace std;

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
        out << "  node [shape=circle, style=filled, fillcolor=lightblue];" << endl;
        out << endl;

        if (maxNode == NULL) {
            out << "  empty [label=\"Empty Heap\", shape=box];" << endl;
        } else {
            // Draw all trees in root list
            Node* curr = maxNode;
            do {
                drawNode(out, curr);
                curr = curr->right;
            } while (curr != maxNode);

            // Highlight max node in red
            out << "  node" << maxNode->data.id << " [fillcolor=red, fontcolor=white];" << endl;

            // Add legend
            out << endl;
            out << "  // Legend" << endl;
            out << "  subgraph cluster_legend {" << endl;
            out << "    label=\"Legend\";" << endl;
            out << "    style=dashed;" << endl;
            out << "    legend1 [label=\"Red = Max\", shape=box, fillcolor=red, fontcolor=white];" << endl;
            out << "    legend2 [label=\"Blue = Other\", shape=box, fillcolor=lightblue];" << endl;
            out << "  }" << endl;
        }

        out << "}" << endl;
        out.close();
    }

    void drawNode(ofstream& out, Node* n) {
        // Draw this node
        out << "  node" << n->data.id << " [label=\"" << n->data.id << "\\nP:" << n->data.priority << "\"];" << endl;

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
        doctors.push_back(d);
    }

    file.close();
    return doctors;
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

    // Calculate ML scores for first 10 patients (to keep demo short)
    int numPatients = min(10, (int)patients.size());

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
    int docIndex = 0;

    while (!heap.isEmpty()) {
        clearScreen();

        cout << "========================================" << endl;
        cout << "   ITERATION " << iteration << endl;
        cout << "========================================" << endl;
        cout << endl;

        // Show heap status
        cout << "Patients waiting: " << heap.count << endl;
        if (!heap.isEmpty()) {
            Patient maxP = heap.getMax();
            cout << "Highest priority: " << maxP.name << " (Score: " << maxP.priority << ")" << endl;
        }
        cout << endl;

        // Get available doctor (round robin)
        Doctor& doc = doctors[docIndex % doctors.size()];
        docIndex++;

        cout << "--- DOCTOR ALLOCATION ---" << endl;
        cout << "Available: Dr. " << doc.name << " (" << doc.department << ")" << endl;
        cout << endl;

        // Extract max patient
        cout << "Calling EXTRACT-MAX on Fibonacci Heap..." << endl;
        cout << "(This triggers CONSOLIDATION - check the .dot file!)" << endl;
        cout << endl;

        Patient p = heap.extractMax();

        cout << ">> ASSIGNED: " << p.name << " (ID: " << p.id << ")" << endl;
        cout << "   Priority Score: " << p.priority << endl;
        cout << "   Department: " << p.department << endl;
        cout << "   -> Assigned to Dr. " << doc.name << endl;

        // Update dot file
        heap.generateDotFile("heap_state.dot");

        cout << endl;
        cout << "----------------------------------------" << endl;
        cout << ">> Dot file updated: heap_state.dot" << endl;
        cout << ">> Check VS Code to see tree changes!" << endl;
        cout << "----------------------------------------" << endl;

        if (!heap.isEmpty()) {
            waitForEnter("\nPress ENTER for next allocation...");
        }

        iteration++;
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "   ALL PATIENTS TREATED!" << endl;
    cout << "========================================" << endl;
    cout << "Total iterations: " << (iteration - 1) << endl;
    cout << endl;

    // Final empty heap
    heap.generateDotFile("heap_state.dot");

    waitForEnter("Press ENTER to exit...");

    return 0;
}
