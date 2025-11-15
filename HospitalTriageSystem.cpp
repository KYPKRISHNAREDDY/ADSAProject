#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <iomanip>
#include "FibonacciHeap.h"
#include "Visualization.h"

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
    // Fibonacci Heaps for each department
    std::map<std::string, FibonacciHeap<Patient>*> departmentQueues;

    // Doctors
    std::vector<Doctor> doctors;

    // Statistics
    int totalPatients;
    int patientsAdmitted;
    int patientsTreated;
    int criticalCases;
    int highPriorityCases;

    // Visualization
    Visualization viz;

    // Call Python ML model to get triage score
    double getPriorityFromML(const Patient& patient) {
        std::stringstream cmd;
        cmd << "python3 triage_ml_model.py "
            << patient.age << " "
            << (patient.gender == "Male" || patient.gender == "M" ? 1 : 0) << " "
            << patient.chestPain << " "
            << patient.diabetes << " "
            << patient.bloodPressure << " "
            << patient.cholesterol << " "
            << patient.bmi << " "
            << patient.maxHeartRate << " "
            << patient.hypertension;

        FILE* pipe = popen(cmd.str().c_str(), "r");
        if (!pipe) {
            std::cerr << "Error: Could not run ML model" << std::endl;
            return 50.0; // Default medium priority
        }

        char buffer[128];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);

        try {
            return std::stod(result);
        } catch (...) {
            std::cerr << "Error parsing ML output: " << result << std::endl;
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
                    std::stoi(tokens[0]),  // id
                    tokens[1],              // name
                    tokens[2],              // department
                    tokens[3],              // specialization
                    std::stoi(tokens[4])   // experience
                );
                doctors.push_back(doc);
            }
        }
        file.close();

        viz.displayEvent("Loaded " + std::to_string(doctors.size()) + " doctors from file", GREEN);
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
        viz.displayEvent("Loaded " + std::to_string(totalPatients) + " patients from file", GREEN);

        // Process patients one by one with ML scoring
        processPatientsBatch(patients);
    }

    void processPatientsBatch(std::vector<Patient>& patients) {
        viz.displaySeparator();
        viz.displayEvent("Starting ML-based Triage Scoring for all patients...", std::string(CYAN) + BOLD);
        viz.pauseForEffect(1000);

        for (auto& patient : patients) {
            // Get ML priority score
            double mlScore = getPriorityFromML(patient);
            patient.priorityScore = mlScore;

            // Track statistics
            if (mlScore >= 90) criticalCases++;
            else if (mlScore >= 75) highPriorityCases++;

            std::stringstream msg;
            msg << "Patient " << patient.id << " (" << patient.name << ") - ML Score: "
                << std::fixed << std::setprecision(1) << mlScore;
            viz.displayEvent(msg.str(), YELLOW);

            // Add to appropriate department queue
            if (departmentQueues.find(patient.department) == departmentQueues.end()) {
                departmentQueues[patient.department] = new FibonacciHeap<Patient>();
            }
            departmentQueues[patient.department]->insert(patient, mlScore, patient.id);

            patientsAdmitted++;

            // Small pause for visualization effect
            viz.pauseForEffect(200);
        }

        viz.displaySeparator();
        viz.displayEvent("All patients scored and added to Fibonacci Heap queues!", std::string(GREEN) + BOLD);
        viz.pauseForEffect(1500);
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
        // Higher priority patients might need more immediate but shorter initial assessment
        // This is simplified; in reality, treatment time varies
        if (priority >= 90) return 3;  // Critical - quick assessment
        else if (priority >= 75) return 4;
        else if (priority >= 60) return 5;
        else return 6;
    }

    void allocatePatients() {
        bool systemActive = true;
        int iteration = 0;

        while (systemActive) {
            iteration++;
            systemActive = false;

            viz.clearScreen();
            viz.displayHeader();

            // Update doctor statuses (decrease treatment time)
            for (auto& doc : doctors) {
                if (doc.isBusy && doc.treatmentTimeRemaining > 0) {
                    doc.treatmentTimeRemaining--;

                    if (doc.treatmentTimeRemaining == 0) {
                        // Doctor finished treating patient
                        std::stringstream msg;
                        msg << "Dr. " << doc.name << " completed treating "
                            << doc.currentPatient->name;
                        viz.displayEvent(msg.str(), GREEN);

                        patientsTreated++;
                        delete doc.currentPatient;
                        doc.currentPatient = nullptr;
                        doc.isBusy = false;
                    }
                }
            }

            // Try to allocate waiting patients to available doctors
            for (auto& [dept, heap] : departmentQueues) {
                while (!heap->isEmpty()) {
                    Doctor* availableDoc = findAvailableDoctor(dept);

                    if (availableDoc != nullptr) {
                        // Allocate highest priority patient to this doctor
                        Patient p = heap->extractMax();
                        Patient* assignedPatient = new Patient(p);

                        availableDoc->isBusy = true;
                        availableDoc->currentPatient = assignedPatient;
                        availableDoc->treatmentTimeRemaining = calculateTreatmentTime(p.priorityScore);

                        std::stringstream msg;
                        msg << "ALLOCATED: " << assignedPatient->name << " (Priority: "
                            << std::fixed << std::setprecision(1) << assignedPatient->priorityScore
                            << ") → Dr. " << availableDoc->name;
                        viz.displayEvent(msg.str(), std::string(CYAN) + BOLD);

                        systemActive = true;
                    } else {
                        // No available doctor in this department
                        break;
                    }
                }
            }

            // Display current system state
            viz.displaySeparator();
            std::cout << "\n" << BOLD << YELLOW << "ITERATION #" << iteration << RESET << "\n";
            viz.displaySeparator();

            // Display doctor statuses
            std::cout << "\n" << BOLD << CYAN << "═══ DOCTOR STATUS ═══" << RESET << "\n\n";
            for (const auto& doc : doctors) {
                if (doc.isBusy) {
                    viz.displayDoctorStatus(doc.id, doc.name, doc.department, true,
                                          doc.currentPatient->name,
                                          doc.currentPatient->priorityScore,
                                          doc.treatmentTimeRemaining);
                    systemActive = true; // Keep running if any doctor is busy
                } else {
                    viz.displayDoctorStatus(doc.id, doc.name, doc.department, false);
                }
                std::cout << "\n";
            }

            // Display waiting queues for each department
            for (auto& [dept, heap] : departmentQueues) {
                if (!heap->isEmpty()) {
                    std::vector<Patient> waitingPatients;
                    std::vector<double> priorities;
                    heap->getAllElements(waitingPatients, priorities);

                    viz.displayWaitingQueue(waitingPatients, priorities, dept);
                    systemActive = true; // Keep running if there are waiting patients
                }
            }

            // Display statistics
            int currentWaiting = 0;
            for (auto& [dept, heap] : departmentQueues) {
                currentWaiting += heap->size();
            }

            viz.displayStatistics(totalPatients, patientsTreated, currentWaiting,
                                criticalCases, highPriorityCases);

            // Check if system should continue
            bool anyDoctorBusy = false;
            for (const auto& doc : doctors) {
                if (doc.isBusy) {
                    anyDoctorBusy = true;
                    break;
                }
            }

            if (!anyDoctorBusy && currentWaiting == 0) {
                systemActive = false;
            }

            if (systemActive) {
                viz.pauseForEffect(2000); // 2 second delay between iterations
            }
        }

        // Final summary
        viz.clearScreen();
        viz.displayHeader();
        viz.displaySeparator();
        std::cout << "\n" << GREEN << BOLD << "🎉 ALL PATIENTS TREATED SUCCESSFULLY! 🎉\n" << RESET;
        viz.displaySeparator();

        viz.displayStatistics(totalPatients, patientsTreated, 0,
                            criticalCases, highPriorityCases);

        // Export data for graphing
        std::vector<std::string> labels = {"Critical", "High", "Medium", "Low"};
        std::vector<int> values = {
            criticalCases,
            highPriorityCases,
            totalPatients - criticalCases - highPriorityCases - (totalPatients / 4),
            totalPatients / 4
        };

        viz.exportVisualizationData("priority_data.csv", labels, values);
        viz.generateGnuplotScript();

        viz.displayAlgorithmInfo();
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
        viz.clearScreen();
        viz.displayHeader();

        viz.displayAlgorithmInfo();
        viz.pauseForEffect(3000);

        viz.clearScreen();
        viz.displayHeader();

        // Load data from files
        viz.displayEvent("Loading system data from files...", std::string(CYAN) + BOLD);
        viz.pauseForEffect(1000);

        loadDoctorsFromFile("doctors_data.csv");
        viz.pauseForEffect(500);

        loadPatientsFromFile("patients_data.csv");
        viz.pauseForEffect(1000);

        viz.displaySeparator();
        viz.displayEvent("Starting dynamic patient allocation...", std::string(MAGENTA) + BOLD);
        viz.pauseForEffect(2000);

        // Start allocation process
        allocatePatients();

        // Final message
        viz.displaySeparator();
        std::cout << "\n" << CYAN << BOLD;
        std::cout << "Triage log saved to: triage_log.txt\n";
        std::cout << "Visualization data saved to: priority_data.csv\n";
        std::cout << "To generate graph, run: gnuplot plot_triage.gnu\n";
        std::cout << RESET << "\n";
    }
};

int main() {
    try {
        HospitalTriageSystem system;
        system.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
