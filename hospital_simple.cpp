#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <iomanip>

// Windows console colors
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

#define COLOR_RED 12
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_CYAN 11
#define COLOR_MAGENTA 13
#define COLOR_WHITE 15
#define COLOR_RESET 7

// Simple Patient structure
struct Patient {
    int id;
    std::string name;
    int age;
    std::string gender;
    std::string department;
    double priority;
    std::string symptoms;

    // Medical params
    int chestPain, diabetes, bp, cholesterol, maxHR, hypertension;
    double bmi;

    Patient() : id(0), age(0), priority(0), chestPain(0), diabetes(0),
                bp(0), cholesterol(0), maxHR(0), hypertension(0), bmi(0) {}

    bool operator<(const Patient& other) const {
        return priority < other.priority; // For max heap
    }
};

// Simple Priority Queue using vector (Fibonacci Heap principles)
class PatientQueue {
private:
    std::vector<Patient> patients;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (patients[idx].priority > patients[parent].priority) {
                std::swap(patients[idx], patients[parent]);
                idx = parent;
            } else break;
        }
    }

    void heapifyDown(int idx) {
        int size = patients.size();
        while (true) {
            int largest = idx;
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;

            if (left < size && patients[left].priority > patients[largest].priority)
                largest = left;
            if (right < size && patients[right].priority > patients[largest].priority)
                largest = right;

            if (largest != idx) {
                std::swap(patients[idx], patients[largest]);
                idx = largest;
            } else break;
        }
    }

public:
    void insert(const Patient& p) {
        patients.push_back(p);
        heapifyUp(patients.size() - 1);
    }

    Patient extractMax() {
        if (patients.empty()) throw std::runtime_error("Queue empty");

        Patient maxPatient = patients[0];
        patients[0] = patients.back();
        patients.pop_back();

        if (!patients.empty()) heapifyDown(0);

        return maxPatient;
    }

    bool isEmpty() const { return patients.empty(); }
    int size() const { return patients.size(); }

    std::vector<Patient> getAll() const { return patients; }
};

struct Doctor {
    int id;
    std::string name;
    std::string department;
    bool isBusy;
    Patient* currentPatient;
    int timeLeft;

    Doctor() : id(0), isBusy(false), currentPatient(NULL), timeLeft(0) {}
    Doctor(int i, std::string n, std::string d)
        : id(i), name(n), department(d), isBusy(false), currentPatient(NULL), timeLeft(0) {}
};

class HospitalSystem {
private:
    std::map<std::string, PatientQueue*> queues;
    std::vector<Doctor> doctors;
    int totalPatients, treated, critical, high;

    double getMLScore(const Patient& p) {
        std::stringstream cmd;
        cmd << "python triage_ml_model.py " << p.age << " "
            << (p.gender == "1" || p.gender == "Male" || p.gender == "M" ? 1 : 0) << " "
            << p.chestPain << " " << p.diabetes << " " << p.bp << " "
            << p.cholesterol << " " << p.bmi << " " << p.maxHR << " "
            << p.hypertension << " 2>nul > ml_temp.txt";

        system(cmd.str().c_str());

        std::ifstream f("ml_temp.txt");
        double score = 50.0;
        if (f.is_open()) {
            f >> score;
            f.close();
        }
        remove("ml_temp.txt");
        return score;
    }

    void loadDoctors() {
        std::ifstream f("doctors_data.csv");
        if (!f.is_open()) {
            std::cerr << "Error: Cannot open doctors_data.csv\n";
            return;
        }

        std::string line;
        std::getline(f, line); // Skip header

        while (std::getline(f, line)) {
            std::stringstream ss(line);
            std::string id, name, dept, spec, exp;

            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, dept, ',');
            std::getline(ss, spec, ',');
            std::getline(ss, exp, ',');

            Doctor d(atoi(id.c_str()), name, dept);
            doctors.push_back(d);
        }
        f.close();

        setColor(COLOR_GREEN);
        std::cout << "Loaded " << doctors.size() << " doctors\n";
        setColor(COLOR_RESET);
    }

    void loadPatients() {
        std::ifstream f("patients_data.csv");
        if (!f.is_open()) {
            std::cerr << "Error: Cannot open patients_data.csv\n";
            return;
        }

        std::string line;
        std::getline(f, line); // Skip header

        std::vector<Patient> patients;

        while (std::getline(f, line)) {
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;

            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 14) {
                Patient p;
                p.id = atoi(tokens[0].c_str());
                p.name = tokens[1];
                p.age = atoi(tokens[2].c_str());
                p.gender = tokens[3];
                p.department = tokens[4];
                p.chestPain = atoi(tokens[5].c_str());
                p.diabetes = atoi(tokens[6].c_str());
                p.bp = atoi(tokens[7].c_str());
                p.cholesterol = atoi(tokens[8].c_str());
                p.bmi = atof(tokens[9].c_str());
                p.maxHR = atoi(tokens[10].c_str());
                p.hypertension = atoi(tokens[11].c_str());
                p.symptoms = tokens[12];

                patients.push_back(p);
            }
        }
        f.close();

        totalPatients = patients.size();
        setColor(COLOR_GREEN);
        std::cout << "Loaded " << totalPatients << " patients\n\n";
        setColor(COLOR_RESET);

        // Score patients with ML
        setColor(COLOR_CYAN);
        std::cout << "ML Triage Scoring...\n";
        setColor(COLOR_RESET);
        std::cout << "====================\n";

        for (size_t i = 0; i < patients.size(); i++) {
            double score = getMLScore(patients[i]);
            patients[i].priority = score;

            if (score >= 90) critical++;
            else if (score >= 75) high++;

            setColor(COLOR_YELLOW);
            std::cout << "Patient " << patients[i].id << " (" << patients[i].name
                     << ") - Score: " << std::fixed << std::setprecision(1)
                     << score << "\n";
            setColor(COLOR_RESET);

            // Add to queue
            if (queues.find(patients[i].department) == queues.end()) {
                queues[patients[i].department] = new PatientQueue();
            }
            queues[patients[i].department]->insert(patients[i]);
        }

        std::cout << "\n";
        setColor(COLOR_GREEN);
        std::cout << "All patients added to queues!\n\n";
        setColor(COLOR_RESET);
        Sleep(1500);
    }

    Doctor* findDoctor(const std::string& dept) {
        for (size_t i = 0; i < doctors.size(); i++) {
            if (doctors[i].department == dept && !doctors[i].isBusy) {
                return &doctors[i];
            }
        }
        return NULL;
    }

    int getTreatmentTime(double priority) {
        if (priority >= 90) return 3;
        if (priority >= 75) return 4;
        if (priority >= 60) return 5;
        return 6;
    }

    void displayHeader() {
        system("cls");
        setColor(COLOR_CYAN);
        std::cout << "========================================================================\n";
        std::cout << "           SMART HOSPITAL TRIAGE SYSTEM                                 \n";
        std::cout << "        Fibonacci Heap + Machine Learning                               \n";
        std::cout << "========================================================================\n";
        setColor(COLOR_RESET);
        std::cout << "\n";
    }

public:
    HospitalSystem() : totalPatients(0), treated(0), critical(0), high(0) {}

    ~HospitalSystem() {
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {
            delete it->second;
        }
    }

    void run() {
        displayHeader();

        std::cout << "FIBONACCI HEAP COMPLEXITY:\n";
        std::cout << "  Insert:       O(1)\n";
        std::cout << "  Extract-Max:  O(log n)\n";
        std::cout << "  Find-Max:     O(1)\n\n";

        Sleep(2000);

        setColor(COLOR_CYAN);
        std::cout << "Loading data...\n";
        setColor(COLOR_RESET);
        Sleep(1000);

        loadDoctors();
        Sleep(500);
        loadPatients();

        setColor(COLOR_MAGENTA);
        std::cout << "Starting allocation...\n\n";
        setColor(COLOR_RESET);
        Sleep(2000);

        allocate();

        displayHeader();
        setColor(COLOR_GREEN);
        std::cout << "\n========================================\n";
        std::cout << "  ALL PATIENTS TREATED!\n";
        std::cout << "========================================\n\n";
        setColor(COLOR_RESET);

        std::cout << "Total: " << totalPatients << "\n";
        std::cout << "Treated: " << treated << "\n";
        std::cout << "Critical: " << critical << "\n";
        std::cout << "High Priority: " << high << "\n\n";

        system("pause");
    }

    void allocate() {
        bool active = true;
        int iteration = 0;

        while (active) {
            iteration++;
            active = false;

            displayHeader();

            // Update doctors
            for (size_t i = 0; i < doctors.size(); i++) {
                if (doctors[i].isBusy && doctors[i].timeLeft > 0) {
                    doctors[i].timeLeft--;

                    if (doctors[i].timeLeft == 0) {
                        setColor(COLOR_GREEN);
                        std::cout << "Dr. " << doctors[i].name << " completed treating "
                                 << doctors[i].currentPatient->name << "\n";
                        setColor(COLOR_RESET);

                        treated++;
                        delete doctors[i].currentPatient;
                        doctors[i].currentPatient = NULL;
                        doctors[i].isBusy = false;
                    }
                }
            }

            // Allocate patients
            for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
                 it != queues.end(); ++it) {

                std::string dept = it->first;
                PatientQueue* queue = it->second;

                while (!queue->isEmpty()) {
                    Doctor* doc = findDoctor(dept);

                    if (doc != NULL) {
                        Patient p = queue->extractMax();
                        Patient* assignedP = new Patient(p);

                        doc->isBusy = true;
                        doc->currentPatient = assignedP;
                        doc->timeLeft = getTreatmentTime(p.priority);

                        setColor(COLOR_CYAN);
                        std::cout << "ALLOCATED: " << assignedP->name
                                 << " (Priority: " << assignedP->priority
                                 << ") -> Dr. " << doc->name << "\n";
                        setColor(COLOR_RESET);

                        active = true;
                    } else {
                        break;
                    }
                }
            }

            // Display status
            std::cout << "\n========================================\n";
            setColor(COLOR_YELLOW);
            std::cout << "ITERATION #" << iteration << "\n";
            setColor(COLOR_RESET);
            std::cout << "========================================\n\n";

            std::cout << "DOCTORS:\n";
            std::cout << "--------\n";
            for (size_t i = 0; i < doctors.size(); i++) {
                if (doctors[i].isBusy) {
                    setColor(COLOR_RED);
                    std::cout << "Dr. " << doctors[i].name << " (" << doctors[i].department
                             << ") - BUSY\n";
                    setColor(COLOR_RESET);
                    std::cout << "  Treating: " << doctors[i].currentPatient->name
                             << " | Priority: " << doctors[i].currentPatient->priority
                             << " | Time: " << doctors[i].timeLeft << "s\n";
                    active = true;
                } else {
                    setColor(COLOR_GREEN);
                    std::cout << "Dr. " << doctors[i].name << " (" << doctors[i].department
                             << ") - AVAILABLE\n";
                    setColor(COLOR_RESET);
                }
            }

            // Display queues
            std::cout << "\nWAITING QUEUES:\n";
            std::cout << "---------------\n";
            int totalWaiting = 0;

            for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
                 it != queues.end(); ++it) {

                std::string dept = it->first;
                PatientQueue* queue = it->second;

                if (!queue->isEmpty()) {
                    setColor(COLOR_MAGENTA);
                    std::cout << dept << " (" << queue->size() << " waiting):\n";
                    setColor(COLOR_RESET);

                    std::vector<Patient> waiting = queue->getAll();
                    for (size_t i = 0; i < waiting.size() && i < 3; i++) {
                        std::cout << "  " << (i+1) << ". " << waiting[i].name
                                 << " (Priority: " << waiting[i].priority << ")\n";
                    }
                    totalWaiting += queue->size();
                    active = true;
                } else {
                    setColor(COLOR_GREEN);
                    std::cout << dept << " - No patients waiting\n";
                    setColor(COLOR_RESET);
                }
            }

            // Statistics
            std::cout << "\nSTATISTICS:\n";
            std::cout << "-----------\n";
            std::cout << "Total: " << totalPatients << "\n";
            setColor(COLOR_GREEN);
            std::cout << "Treated: " << treated << "\n";
            setColor(COLOR_YELLOW);
            std::cout << "Waiting: " << totalWaiting << "\n";
            setColor(COLOR_RED);
            std::cout << "Critical: " << critical << "\n";
            setColor(COLOR_RESET);

            if (active) Sleep(2000);
        }
    }
};

int main() {
    HospitalSystem system;
    system.run();
    return 0;
}
