/*
 * Hospital Triage System using Fibonacci Heap
 * Version 6 - Simple & Clean (No File Locking Issues)
 *
 * Features:
 * - Fibonacci Heap for patient priority queue
 * - ML-based priority scoring
 * - Real-time Graphviz visualization
 * - Doctor allocation with department matching
 * - Preemption support for critical patients
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

// Track new patient IDs for orange highlighting
set<int> newPatientIds;

// ==================== STRUCTURES ====================

struct Patient {
    int id;
    string name;
    int age;
    int chest_pain;
    int bp;
    int heart_rate;
    string department;
    int priority;
};

struct Doctor {
    int id;
    string name;
    string department;
    bool busy;
    string currentPatient;
    int currentPatientPriority;
    Patient currentPatientData;
    int treatmentTime;
};

// ==================== FIBONACCI HEAP ====================

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

class FibonacciHeap {
public:
    Node* maxNode;
    int count;

    FibonacciHeap() {
        maxNode = NULL;
        count = 0;
    }

    bool isEmpty() { return maxNode == NULL; }

    Patient getMax() {
        if (maxNode) return maxNode->data;
        Patient empty; empty.id = -1;
        return empty;
    }

    // INSERT - O(1)
    void insert(Patient p) {
        Node* node = new Node(p);
        if (maxNode == NULL) {
            maxNode = node;
        } else {
            // Add to root list
            node->left = maxNode;
            node->right = maxNode->right;
            maxNode->right->left = node;
            maxNode->right = node;
            if (node->data.priority > maxNode->data.priority) {
                maxNode = node;
            }
        }
        count++;
    }

    // EXTRACT-MAX - O(log n) amortized
    Patient extractMax() {
        if (maxNode == NULL) {
            Patient empty; empty.id = -1;
            return empty;
        }

        Node* z = maxNode;

        // Add children to root list
        if (z->child != NULL) {
            Node* child = z->child;
            do {
                Node* next = child->right;
                child->left->right = child->right;
                child->right->left = child->left;
                child->left = maxNode;
                child->right = maxNode->right;
                maxNode->right->left = child;
                maxNode->right = child;
                child->parent = NULL;
                child = next;
            } while (child != z->child);
        }

        // Remove z from root list
        z->left->right = z->right;
        z->right->left = z->left;

        if (z == z->right) {
            maxNode = NULL;
        } else {
            maxNode = z->right;
            consolidate();
        }

        count--;
        return z->data;
    }

    void consolidate() {
        if (maxNode == NULL) return;

        int maxDegree = (int)(log(count + 1) / log(1.618)) + 2;
        vector<Node*> A(maxDegree + 5, NULL);

        // Collect roots
        vector<Node*> roots;
        Node* curr = maxNode;
        do {
            roots.push_back(curr);
            curr = curr->right;
        } while (curr != maxNode);

        for (size_t i = 0; i < roots.size(); i++) {
            Node* x = roots[i];
            int d = x->degree;

            while (A[d] != NULL) {
                Node* y = A[d];
                if (x->data.priority < y->data.priority) {
                    Node* temp = x; x = y; y = temp;
                }
                link(y, x);
                A[d] = NULL;
                d++;
            }
            A[d] = x;
        }

        // Rebuild root list
        maxNode = NULL;
        for (size_t i = 0; i < A.size(); i++) {
            if (A[i] != NULL) {
                if (maxNode == NULL) {
                    maxNode = A[i];
                    A[i]->left = A[i];
                    A[i]->right = A[i];
                } else {
                    A[i]->left = maxNode;
                    A[i]->right = maxNode->right;
                    maxNode->right->left = A[i];
                    maxNode->right = A[i];
                    if (A[i]->data.priority > maxNode->data.priority) {
                        maxNode = A[i];
                    }
                }
            }
        }
    }

    void link(Node* y, Node* x) {
        y->left->right = y->right;
        y->right->left = y->left;
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

    // Generate visualization and call Graphviz
    void visualize(string status = "", string details = "") {
        // Write DOT file
        ofstream out("heap.dot");
        out << "digraph FibonacciHeap {\n";
        out << "  rankdir=TB;\n";
        out << "  bgcolor=white;\n";
        out << "  node [shape=box, style=\"filled,rounded\", fontsize=11];\n";
        out << "  splines=true;\n\n";

        // Title at top
        out << "  title [label=\"FIBONACCI HEAP (Max-Heap)\\n";
        out << count << " patients waiting";
        out << "\", fillcolor=\"#333333\", fontcolor=white, fontsize=12];\n\n";

        if (maxNode == NULL) {
            out << "  empty [label=\"Heap is Empty\\nAll patients treated!\", fillcolor=\"#90EE90\", fontsize=14];\n";
            out << "  title -> empty [style=invis];\n";
        } else {
            // Collect root nodes
            vector<Node*> roots;
            Node* curr = maxNode;
            do {
                roots.push_back(curr);
                curr = curr->right;
            } while (curr != maxNode);

            // Draw nodes
            for (size_t i = 0; i < roots.size(); i++) {
                drawNode(out, roots[i]);
            }

            // Layout - keep roots in same row
            out << "\n  { rank=same;";
            for (size_t i = 0; i < roots.size(); i++) {
                out << " node" << roots[i]->data.id << ";";
            }
            out << " }\n";

            // Connect title to first root
            out << "  title -> node" << roots[0]->data.id << " [style=invis];\n";

            // Invisible edges for ordering
            for (size_t i = 0; i < roots.size() - 1; i++) {
                out << "  node" << roots[i]->data.id << " -> node"
                    << roots[i+1]->data.id << " [style=invis];\n";
            }

            // Highlight max node
            out << "  node" << maxNode->data.id
                << " [fillcolor=\"#E74C3C\", fontcolor=white, penwidth=3];\n";

            // Tree count info
            out << "\n  treeinfo [label=\"Root List: " << roots.size() << " trees\", ";
            out << "fillcolor=\"#666666\", fontcolor=white, fontsize=10];\n";
        }

        // Status box at bottom (always shown)
        out << "\n  // Status Box\n";
        out << "  status [label=\"";
        if (!status.empty()) {
            out << "STATUS: " << status;
            if (!details.empty()) {
                out << "\\n" << details;
            }
        } else {
            out << "Ready";
        }
        out << "\", shape=box, style=\"filled,rounded\", ";
        out << "fillcolor=\"#2196F3\", fontcolor=white, fontsize=11];\n";

        // Legend box
        out << "  legend [label=\"";
        out << "RED = Next Patient (Highest Priority)\\n";
        out << "BLUE = Waiting Patients\\n";
        out << "ORANGE = New Emergency Patient";
        out << "\", shape=box, style=\"filled\", ";
        out << "fillcolor=\"#f5f5f5\", fontcolor=\"#333333\", fontsize=9];\n";

        // Position status and legend at bottom
        out << "  { rank=sink; status; legend; }\n";

        out << "}\n";
        out.close();

        // Call Graphviz - cross-platform support
        #ifdef _WIN32
            int result = system("dot -Tpng -Gdpi=150 heap.dot -o heap.png 2>nul");
            if (result != 0) {
                result = system("\"C:\\Program Files\\Graphviz\\bin\\dot.exe\" -Tpng -Gdpi=150 heap.dot -o heap.png 2>nul");
            }
            if (result != 0) {
                result = system("\"C:\\Program Files (x86)\\Graphviz\\bin\\dot.exe\" -Tpng -Gdpi=150 heap.dot -o heap.png 2>nul");
            }
        #else
            int result = system("dot -Tpng -Gdpi=150 heap.dot -o heap.png 2>/dev/null");
        #endif

        cout << "   [Visualization updated]\n";
    }

    void drawNode(ofstream& out, Node* n) {
        string name = n->data.name;
        size_t sp = name.find(' ');
        if (sp != string::npos) name = name.substr(0, sp);

        string color = "#4A90D9";
        string fontcolor = "white";

        if (newPatientIds.count(n->data.id)) {
            color = "#FF9500";  // Orange for new patients
            fontcolor = "black";
            out << "  node" << n->data.id << " [label=\"NEW!\\n" << name
                << "\\n" << n->data.priority << "\", fillcolor=\"" << color
                << "\", fontcolor=" << fontcolor << "];\n";
        } else {
            out << "  node" << n->data.id << " [label=\"" << name
                << "\\n" << n->data.priority << "\", fillcolor=\"" << color
                << "\", fontcolor=" << fontcolor << "];\n";
        }

        if (n->child != NULL) {
            Node* child = n->child;
            do {
                out << "  node" << n->data.id << " -> node" << child->data.id << ";\n";
                drawNode(out, child);
                child = child->right;
            } while (child != n->child);
        }
    }
};

// ==================== UTILITY FUNCTIONS ====================

int getMLScore(int age, int chest_pain, int bp, int heart_rate) {
    stringstream cmd;
    cmd << "python triage.py " << age << " " << chest_pain << " " << bp << " " << heart_rate << " > ml_out.txt";
    system(cmd.str().c_str());

    ifstream f("ml_out.txt");
    int score = 50;
    if (f.is_open()) { f >> score; f.close(); }
    remove("ml_out.txt");
    return score;
}

vector<Patient> loadPatients(string filename) {
    vector<Patient> patients;
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return patients;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        Patient p;

        getline(ss, token, ','); p.id = atoi(token.c_str());
        getline(ss, token, ','); p.name = token;
        getline(ss, token, ','); p.age = atoi(token.c_str());
        getline(ss, token, ','); // gender
        getline(ss, token, ','); p.department = token;
        getline(ss, token, ','); p.chest_pain = atoi(token.c_str());
        getline(ss, token, ','); // diabetes
        getline(ss, token, ','); p.bp = atoi(token.c_str());
        getline(ss, token, ','); // cholesterol
        getline(ss, token, ','); // bmi
        getline(ss, token, ','); p.heart_rate = atoi(token.c_str());
        p.priority = 0;
        patients.push_back(p);
    }
    file.close();
    return patients;
}

vector<Doctor> loadDoctors(string filename) {
    vector<Doctor> doctors;
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return doctors;
    }

    string line;
    getline(file, line);

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

int findFreeDoctor(vector<Doctor>& docs, string dept) {
    for (size_t i = 0; i < docs.size(); i++) {
        if (docs[i].department == dept && !docs[i].busy) return i;
    }
    return -1;
}

int findLowestPriorityDoctor(vector<Doctor>& docs, string dept) {
    int idx = -1, lowest = 999;
    for (size_t i = 0; i < docs.size(); i++) {
        if (docs[i].department == dept && docs[i].busy) {
            if (docs[i].currentPatientPriority < lowest) {
                lowest = docs[i].currentPatientPriority;
                idx = i;
            }
        }
    }
    return idx;
}

void showDoctorStatus(vector<Doctor>& docs) {
    cout << "\n+----------------------------------------------------------+\n";
    cout << "|                    DOCTOR STATUS                         |\n";
    cout << "+----------------------------------------------------------+\n";
    for (size_t i = 0; i < docs.size(); i++) {
        cout << "| Dr. " << docs[i].name;
        for (int j = docs[i].name.length(); j < 12; j++) cout << " ";
        cout << "| " << docs[i].department;
        for (int j = docs[i].department.length(); j < 12; j++) cout << " ";
        if (docs[i].busy) {
            cout << "| BUSY: " << docs[i].currentPatient;
            for (int j = docs[i].currentPatient.length(); j < 15; j++) cout << " ";
        } else {
            cout << "| FREE                  ";
        }
        cout << "|\n";
    }
    cout << "+----------------------------------------------------------+\n";
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause(string msg = "Press ENTER to continue...") {
    cout << "\n" << msg;
    cin.get();
}

// ==================== MAIN ====================

int main() {
    FibonacciHeap heap;

    cout << "========================================\n";
    cout << "    HOSPITAL TRIAGE SYSTEM\n";
    cout << "    Fibonacci Heap + ML + Graphviz\n";
    cout << "========================================\n\n";

    // Load data
    cout << "Loading data...\n";
    vector<Patient> patients = loadPatients("patients_data.csv");
    vector<Doctor> doctors = loadDoctors("doctors_data.csv");
    cout << "Loaded " << patients.size() << " patients, " << doctors.size() << " doctors\n";

    if (patients.empty() || doctors.empty()) {
        cout << "Error loading data!\n";
        return 1;
    }

    pause("\nPress ENTER to calculate ML scores...");

    // ML Scoring
    cout << "\n--- ML TRIAGE SCORING ---\n";
    for (size_t i = 0; i < patients.size(); i++) {
        patients[i].priority = getMLScore(patients[i].age, patients[i].chest_pain,
                                          patients[i].bp, patients[i].heart_rate);
        cout << "Patient " << patients[i].id << " (" << patients[i].name
             << "): Score = " << patients[i].priority << "\n";
    }

    pause("\nPress ENTER to build Fibonacci Heap...");

    // Insert into heap
    cout << "\n--- BUILDING FIBONACCI HEAP ---\n";
    for (size_t i = 0; i < patients.size(); i++) {
        heap.insert(patients[i]);
        cout << "INSERT: " << patients[i].name << " (Priority: " << patients[i].priority << ")\n";
    }

    cout << "\nHeap built with " << heap.count << " patients\n";
    cout << "Maximum: " << heap.getMax().name << " (Score: " << heap.getMax().priority << ")\n";

    // Generate initial visualization
    heap.visualize("INSERT Complete - O(1) per patient",
                   "All patients added to root list. No consolidation yet.");
    cout << "\n>> Visualization saved to heap.png\n";
    cout << ">> Open viewer.html in browser to see live updates!\n";

    pause("\nPress ENTER to start simulation...");

    // Simulation
    int iteration = 1;
    int treated = 0;
    bool newPatientAdded = false;

    while (!heap.isEmpty() && iteration <= 50) {
        clearScreen();
        cout << "========================================\n";
        cout << "    ITERATION " << iteration << "\n";
        cout << "========================================\n";

        // Check treatment completion
        for (size_t i = 0; i < doctors.size(); i++) {
            if (doctors[i].busy) {
                doctors[i].treatmentTime++;
                if (doctors[i].treatmentTime >= 2) {
                    cout << ">> Dr. " << doctors[i].name << " finished treating "
                         << doctors[i].currentPatient << "\n";
                    doctors[i].busy = false;
                    doctors[i].currentPatient = "";
                    doctors[i].currentPatientPriority = 0;
                    doctors[i].treatmentTime = 0;
                }
            }
        }

        // New emergency patient at iteration 5
        if (iteration == 5 && !newPatientAdded) {
            cout << "\n!!! EMERGENCY: New critical patient arrived !!!\n";
            Patient newP;
            newP.id = 999;
            newP.name = "CRITICAL-X";
            newP.age = 55;
            newP.chest_pain = 4;
            newP.bp = 200;
            newP.heart_rate = 180;
            newP.department = "Cardiology";
            newP.priority = 95;

            cout << "Patient: " << newP.name << " | Dept: " << newP.department
                 << " | Priority: " << newP.priority << "\n";

            heap.insert(newP);
            newPatientIds.insert(999);
            newPatientAdded = true;

            heap.visualize("EMERGENCY INSERT - O(1)",
                           "CRITICAL-X added! May trigger PREEMPTION next.");
            pause("Press ENTER to continue...");
            clearScreen();
            cout << "========================================\n";
            cout << "    ITERATION " << iteration << " (continued)\n";
            cout << "========================================\n";
        }

        showDoctorStatus(doctors);

        cout << "\nPatients waiting: " << heap.count << "\n";
        if (!heap.isEmpty()) {
            cout << "Highest priority: " << heap.getMax().name
                 << " (" << heap.getMax().priority << ")\n";
        }

        // Extract max
        cout << "\n--- EXTRACT-MAX ---\n";
        Patient p = heap.extractMax();
        cout << "Extracted: " << p.name << " (Dept: " << p.department
             << ", Priority: " << p.priority << ")\n";

        newPatientIds.erase(p.id);

        // Find doctor
        int docIdx = findFreeDoctor(doctors, p.department);
        string statusMsg, detailMsg;

        if (docIdx != -1) {
            cout << ">> Dr. " << doctors[docIdx].name << " is FREE\n";
            doctors[docIdx].busy = true;
            doctors[docIdx].currentPatient = p.name;
            doctors[docIdx].currentPatientPriority = p.priority;
            doctors[docIdx].currentPatientData = p;
            doctors[docIdx].treatmentTime = 0;
            treated++;
            cout << ">> ASSIGNED: " << p.name << " -> Dr. " << doctors[docIdx].name << "\n";

            stringstream ss1, ss2;
            ss1 << "EXTRACT-MAX + ASSIGN (Iter " << iteration << ")";
            ss2 << p.name << " -> Dr. " << doctors[docIdx].name;
            statusMsg = ss1.str();
            detailMsg = ss2.str();
        } else {
            int lowestDoc = findLowestPriorityDoctor(doctors, p.department);
            if (lowestDoc != -1 && p.priority > doctors[lowestDoc].currentPatientPriority) {
                cout << "\n>> PREEMPTION: " << p.name << " (" << p.priority
                     << ") takes over from " << doctors[lowestDoc].currentPatient
                     << " (" << doctors[lowestDoc].currentPatientPriority << ")\n";

                Patient preempted = doctors[lowestDoc].currentPatientData;
                heap.insert(preempted);
                cout << ">> " << preempted.name << " returned to queue\n";

                stringstream ss1, ss2;
                ss1 << "PREEMPTION! (Iter " << iteration << ")";
                ss2 << p.name << " took over, " << preempted.name << " back to queue";
                statusMsg = ss1.str();
                detailMsg = ss2.str();

                doctors[lowestDoc].currentPatient = p.name;
                doctors[lowestDoc].currentPatientPriority = p.priority;
                doctors[lowestDoc].currentPatientData = p;
                doctors[lowestDoc].treatmentTime = 0;
                treated++;
            } else {
                cout << ">> No doctor available, " << p.name << " waits\n";
                heap.insert(p);

                stringstream ss1, ss2;
                ss1 << "WAITING (Iter " << iteration << ")";
                ss2 << p.name << " re-inserted, no free " << p.department << " doctor";
                statusMsg = ss1.str();
                detailMsg = ss2.str();
            }
        }

        // Update visualization with detailed status
        heap.visualize(statusMsg, detailMsg);

        showDoctorStatus(doctors);

        if (!heap.isEmpty()) {
            pause("\nPress ENTER for next iteration...");
        }
        iteration++;
    }

    cout << "\n========================================\n";
    cout << "    SIMULATION COMPLETE\n";
    cout << "========================================\n";
    cout << "Total patients treated: " << treated << "\n";
    cout << "Total iterations: " << (iteration - 1) << "\n";

    stringstream finalMsg;
    finalMsg << "Treated " << treated << " patients in " << (iteration - 1) << " iterations";
    heap.visualize("SIMULATION COMPLETE", finalMsg.str());
    pause("\nPress ENTER to exit...");

    return 0;
}
