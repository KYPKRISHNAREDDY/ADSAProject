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
#include "FibonacciHeap.h"

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

// Patient structure
struct Patient {
    int id;
    std::string name;
    int age;
    std::string gender;
    std::string department;
    double priorityScore;
    std::string symptoms;
    std::string arrivalTime;

    int chestPain;
    int diabetes;
    int bloodPressure;
    int cholesterol;
    double bmi;
    int maxHeartRate;
    int hypertension;

    Patient() : id(0), age(0), priorityScore(0.0), chestPain(0), diabetes(0),
                bloodPressure(0), cholesterol(0), bmi(0.0), maxHeartRate(0), hypertension(0) {}

    void display() const {
        std::cout << "ID: " << id << " | " << name << " | Age: " << age
                  << " | Priority: " << priorityScore << " | Dept: " << department << std::endl;
    }
};

// Doctor structure
struct Doctor {
    int id;
    std::string name;
    std::string department;
    std::string specialization;
    int experienceYears;
    bool isBusy;
    Patient* currentPatient;
    int treatmentTimeRemaining;

    Doctor() : id(0), experienceYears(0), isBusy(false),
               currentPatient(nullptr), treatmentTimeRemaining(0) {}

    Doctor(int _id, std::string _name, std::string _dept, std::string _spec, int _exp)
        : id(_id), name(_name), department(_dept), specialization(_spec),
          experienceYears(_exp), isBusy(false), currentPatient(nullptr),
          treatmentTimeRemaining(0) {}
};

class HospitalTriageSystem {
private:
    std::map<std::string, FibonacciHeap<Patient>*> departmentQueues;
    std::vector<Doctor> doctors;

    int totalPatients;
    int patientsAdmitted;
    int patientsTreated;
    int criticalCases;
    int highPriorityCases;

    // Call Python ML model - Windows version
    double getPriorityFromML(const Patient& patient) {
        std::stringstream cmd;
        cmd << "python triage_ml_model.py "
            << patient.age << " "
            << (patient.gender == "Male" || patient.gender == "M" || patient.gender == "1" ? 1 : 0) << " "
            << patient.chestPain << " "
            << patient.diabetes << " "
            << patient.bloodPressure << " "
            << patient.cholesterol << " "
            << patient.bmi << " "
            << patient.maxHeartRate << " "
            << patient.hypertension
            << " 2>nul";  // Suppress error messages on Windows

        // Create temporary file for output
        std::string tempFile = "ml_output.tmp";
        std::string fullCmd = cmd.str() + " > " + tempFile;

        int result = system(fullCmd.c_str());

        if (result != 0) {
            std::cerr << "Warning: ML model execution failed, using default priority" << std::endl;
            return 50.0;
        }

        // Read output from temp file
        std::ifstream inFile(tempFile);
        if (!inFile.is_open()) {
            return 50.0;
        }

        std::string scoreStr;
        std::getline(inFile, scoreStr);
        inFile.close();

        // Delete temp file
        remove(tempFile.c_str());

        try {
            return std::stod(scoreStr);
        } catch (...) {
            std::cerr << "Warning: Invalid ML output, using default priority" << std::endl;
            return 50.0;
        }
    }

    void loadDoctorsFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open doctors file: " << filename << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 5) {
                Doctor doc(
                    std::stoi(tokens[0]),
                    tokens[1],
                    tokens[2],
                    tokens[3],
                    std::stoi(tokens[4])
                );
                doctors.push_back(doc);
            }
        }
        file.close();

        setColor(COLOR_GREEN);
        std::cout << "Loaded " << doctors.size() << " doctors from file" << std::endl;
        setColor(COLOR_RESET);
    }

    void loadPatientsFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open patients file: " << filename << std::endl;
            return;
        }

        std::string line;
        std::getline(file, line); // Skip header

        std::vector<Patient> patients;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;

            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 14) {
                Patient p;
                p.id = std::stoi(tokens[0]);
                p.name = tokens[1];
                p.age = std::stoi(tokens[2]);
                p.gender = tokens[3];
                p.department = tokens[4];
                p.chestPain = std::stoi(tokens[5]);
                p.diabetes = std::stoi(tokens[6]);
                p.bloodPressure = std::stoi(tokens[7]);
                p.cholesterol = std::stoi(tokens[8]);
                p.bmi = std::stod(tokens[9]);
                p.maxHeartRate = std::stoi(tokens[10]);
                p.hypertension = std::stoi(tokens[11]);
                p.symptoms = tokens[12];
                p.arrivalTime = tokens[13];

                patients.push_back(p);
            }
        }
        file.close();

        totalPatients = patients.size();
        setColor(COLOR_GREEN);
        std::cout << "Loaded " << totalPatients << " patients from file" << std::endl;
        setColor(COLOR_RESET);

        processPatientsBatch(patients);
    }

    void processPatientsBatch(std::vector<Patient>& patients) {
        std::cout << "\n========================================" << std::endl;
        setColor(COLOR_CYAN);
        std::cout << "Starting ML-based Triage Scoring..." << std::endl;
        setColor(COLOR_RESET);
        std::cout << "========================================\n" << std::endl;

        for (auto& patient : patients) {
            double mlScore = getPriorityFromML(patient);
            patient.priorityScore = mlScore;

            if (mlScore >= 90) criticalCases++;
            else if (mlScore >= 75) highPriorityCases++;

            setColor(COLOR_YELLOW);
            std::cout << "Patient " << patient.id << " (" << patient.name << ") - ML Score: "
                     << std::fixed << std::setprecision(1) << mlScore << std::endl;
            setColor(COLOR_RESET);

            if (departmentQueues.find(patient.department) == departmentQueues.end()) {
                departmentQueues[patient.department] = new FibonacciHeap<Patient>();
            }
            departmentQueues[patient.department]->insert(patient, mlScore, patient.id);

            patientsAdmitted++;
        }

        std::cout << "\n========================================" << std::endl;
        setColor(COLOR_GREEN);
        std::cout << "All patients scored and added to queues!" << std::endl;
        setColor(COLOR_RESET);
        std::cout << "========================================\n" << std::endl;
        Sleep(1500);
    }

    Doctor* findAvailableDoctor(const std::string& department) {
        for (auto& doc : doctors) {
            if (doc.department == department && !doc.isBusy) {
                return &doc;
            }
        }
        return nullptr;
    }

    int calculateTreatmentTime(double priority) {
        if (priority >= 90) return 3;
        else if (priority >= 75) return 4;
        else if (priority >= 60) return 5;
        else return 6;
    }

    void displayHeader() {
        system("cls");
        setColor(COLOR_CYAN);
        std::cout << "========================================================================" << std::endl;
        std::cout << "              SMART HOSPITAL TRIAGE SYSTEM                              " << std::endl;
        std::cout << "          Real-time ML-Based Patient Allocation                         " << std::endl;
        std::cout << "          Using Fibonacci Heap Data Structure                           " << std::endl;
        std::cout << "========================================================================" << std::endl;
        setColor(COLOR_RESET);
        std::cout << std::endl;
    }

    void allocatePatients() {
        bool systemActive = true;
        int iteration = 0;

        while (systemActive) {
            iteration++;
            systemActive = false;

            displayHeader();

            // Update doctor statuses
            for (auto& doc : doctors) {
                if (doc.isBusy && doc.treatmentTimeRemaining > 0) {
                    doc.treatmentTimeRemaining--;

                    if (doc.treatmentTimeRemaining == 0) {
                        setColor(COLOR_GREEN);
                        std::cout << "Dr. " << doc.name << " completed treating "
                                 << doc.currentPatient->name << std::endl;
                        setColor(COLOR_RESET);

                        patientsTreated++;
                        delete doc.currentPatient;
                        doc.currentPatient = nullptr;
                        doc.isBusy = false;
                    }
                }
            }

            // Allocate patients
            for (auto& [dept, heap] : departmentQueues) {
                while (!heap->isEmpty()) {
                    Doctor* availableDoc = findAvailableDoctor(dept);

                    if (availableDoc != nullptr) {
                        Patient p = heap->extractMax();
                        Patient* assignedPatient = new Patient(p);

                        availableDoc->isBusy = true;
                        availableDoc->currentPatient = assignedPatient;
                        availableDoc->treatmentTimeRemaining = calculateTreatmentTime(p.priorityScore);

                        setColor(COLOR_CYAN);
                        std::cout << "ALLOCATED: " << assignedPatient->name << " (Priority: "
                                 << std::fixed << std::setprecision(1) << assignedPatient->priorityScore
                                 << ") -> Dr. " << availableDoc->name << std::endl;
                        setColor(COLOR_RESET);

                        systemActive = true;
                    } else {
                        break;
                    }
                }
            }

            // Display status
            std::cout << "\n========================================" << std::endl;
            setColor(COLOR_YELLOW);
            std::cout << "ITERATION #" << iteration << std::endl;
            setColor(COLOR_RESET);
            std::cout << "========================================\n" << std::endl;

            std::cout << "DOCTOR STATUS:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            for (const auto& doc : doctors) {
                if (doc.isBusy) {
                    setColor(COLOR_RED);
                    std::cout << "Dr. " << doc.name << " (" << doc.department << ") - BUSY" << std::endl;
                    setColor(COLOR_RESET);
                    std::cout << "  Treating: " << doc.currentPatient->name
                             << " | Priority: " << doc.currentPatient->priorityScore
                             << " | Time Left: " << doc.treatmentTimeRemaining << "s" << std::endl;
                    systemActive = true;
                } else {
                    setColor(COLOR_GREEN);
                    std::cout << "Dr. " << doc.name << " (" << doc.department << ") - AVAILABLE" << std::endl;
                    setColor(COLOR_RESET);
                }
            }

            // Display waiting queues
            std::cout << "\nWAITING QUEUES:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            for (auto& [dept, heap] : departmentQueues) {
                if (!heap->isEmpty()) {
                    std::vector<Patient> waitingPatients;
                    std::vector<double> priorities;
                    heap->getAllElements(waitingPatients, priorities);

                    setColor(COLOR_MAGENTA);
                    std::cout << dept << " Department (" << waitingPatients.size() << " waiting):" << std::endl;
                    setColor(COLOR_RESET);

                    for (size_t i = 0; i < std::min(size_t(5), waitingPatients.size()); i++) {
                        std::cout << "  " << (i+1) << ". " << waitingPatients[i].name
                                 << " (Priority: " << priorities[i] << ")" << std::endl;
                    }
                    systemActive = true;
                } else {
                    setColor(COLOR_GREEN);
                    std::cout << dept << " Department - No patients waiting" << std::endl;
                    setColor(COLOR_RESET);
                }
            }

            // Display statistics
            int currentWaiting = 0;
            for (auto& [dept, heap] : departmentQueues) {
                currentWaiting += heap->size();
            }

            std::cout << "\nSTATISTICS:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << "Total Patients: " << totalPatients << std::endl;
            setColor(COLOR_GREEN);
            std::cout << "Patients Treated: " << patientsTreated << std::endl;
            setColor(COLOR_YELLOW);
            std::cout << "Patients Waiting: " << currentWaiting << std::endl;
            setColor(COLOR_RED);
            std::cout << "Critical Cases: " << criticalCases << std::endl;
            setColor(COLOR_MAGENTA);
            std::cout << "High Priority Cases: " << highPriorityCases << std::endl;
            setColor(COLOR_RESET);

            if (!systemActive && currentWaiting == 0) {
                bool anyDoctorBusy = false;
                for (const auto& doc : doctors) {
                    if (doc.isBusy) {
                        anyDoctorBusy = true;
                        break;
                    }
                }
                systemActive = anyDoctorBusy;
            }

            if (systemActive) {
                Sleep(2000); // 2 second delay
            }
        }

        // Final summary
        displayHeader();
        setColor(COLOR_GREEN);
        std::cout << "\n========================================" << std::endl;
        std::cout << "  ALL PATIENTS TREATED SUCCESSFULLY!   " << std::endl;
        std::cout << "========================================\n" << std::endl;
        setColor(COLOR_RESET);

        std::cout << "FINAL STATISTICS:" << std::endl;
        std::cout << "  Total Patients: " << totalPatients << std::endl;
        std::cout << "  Patients Treated: " << patientsTreated << std::endl;
        std::cout << "  Critical Cases: " << criticalCases << std::endl;
        std::cout << "  High Priority Cases: " << highPriorityCases << std::endl;
        std::cout << std::endl;
    }

public:
    HospitalTriageSystem()
        : totalPatients(0), patientsAdmitted(0), patientsTreated(0),
          criticalCases(0), highPriorityCases(0) {
        srand(time(0));
    }

    ~HospitalTriageSystem() {
        for (auto& [dept, heap] : departmentQueues) {
            delete heap;
        }
    }

    void run() {
        displayHeader();

        std::cout << "FIBONACCI HEAP PROPERTIES:" << std::endl;
        std::cout << "  - Insert Operation:      O(1) amortized" << std::endl;
        std::cout << "  - Find Maximum:          O(1)" << std::endl;
        std::cout << "  - Extract Maximum:       O(log n) amortized" << std::endl;
        std::cout << "  - Increase Key:          O(1) amortized" << std::endl;
        std::cout << std::endl;

        setColor(COLOR_CYAN);
        std::cout << "Loading system data..." << std::endl;
        setColor(COLOR_RESET);
        Sleep(1000);

        loadDoctorsFromFile("doctors_data.csv");
        Sleep(500);

        loadPatientsFromFile("patients_data.csv");
        Sleep(1000);

        std::cout << "\n========================================" << std::endl;
        setColor(COLOR_MAGENTA);
        std::cout << "Starting dynamic patient allocation..." << std::endl;
        setColor(COLOR_RESET);
        std::cout << "========================================\n" << std::endl;
        Sleep(2000);

        allocatePatients();

        std::cout << "Program completed. Press any key to exit..." << std::endl;
    }
};

int main() {
    // Enable ANSI escape codes on Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    try {
        HospitalTriageSystem system;
        system.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    system("pause");
    return 0;
}
