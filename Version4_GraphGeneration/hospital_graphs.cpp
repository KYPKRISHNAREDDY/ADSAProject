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

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

#define COLOR_GREEN 10
#define COLOR_CYAN 11
#define COLOR_YELLOW 14
#define COLOR_RESET 7

struct Patient {
    int id;
    std::string name;
    int age;
    std::string gender;
    std::string department;
    double priority;
    std::string symptoms;
    int chestPain, diabetes, bp, cholesterol, maxHR, hypertension;
    double bmi;

    Patient() : id(0), age(0), priority(0), chestPain(0), diabetes(0),
                bp(0), cholesterol(0), maxHR(0), hypertension(0), bmi(0) {}
};

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

struct AllocationEvent {
    int iteration;
    int patientId;
    std::string patientName;
    double priority;
    int doctorId;
    std::string doctorName;
    std::string department;
    int timestamp;
};

class GraphGeneratingHospital {
private:
    std::map<std::string, PatientQueue*> queues;
    std::vector<Doctor> doctors;
    std::vector<AllocationEvent> allocationHistory;
    std::vector<std::pair<int, int> > queueSizeHistory; // iteration, total_waiting
    std::vector<double> allPriorities;
    int totalPatients, treated, critical, high, medium, low;

    std::ofstream timelineFile;
    std::ofstream queueFile;
    std::ofstream priorityFile;
    std::ofstream doctorFile;
    std::ofstream mlScoreFile;

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
        if (!f.is_open()) return;

        std::string line;
        std::getline(f, line);

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
        if (!f.is_open()) return;

        std::string line;
        std::getline(f, line);

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
        std::cout << "Loaded " << totalPatients << " patients\n";
        setColor(COLOR_RESET);

        // Score patients
        setColor(COLOR_CYAN);
        std::cout << "ML scoring...\n";
        setColor(COLOR_RESET);

        for (size_t i = 0; i < patients.size(); i++) {
            double score = getMLScore(patients[i]);
            patients[i].priority = score;
            allPriorities.push_back(score);

            // Categorize
            if (score >= 90) critical++;
            else if (score >= 75) high++;
            else if (score >= 60) medium++;
            else low++;

            // Save ML score data
            mlScoreFile << patients[i].id << "," << patients[i].name << ","
                       << patients[i].age << "," << score << ","
                       << patients[i].department << "\n";

            std::cout << "Patient " << (i+1) << "/" << totalPatients << "\r";
            std::cout.flush();

            // Add to queue
            if (queues.find(patients[i].department) == queues.end()) {
                queues[patients[i].department] = new PatientQueue();
            }
            queues[patients[i].department]->insert(patients[i]);
        }

        std::cout << "\n";
        setColor(COLOR_GREEN);
        std::cout << "All patients scored!\n";
        setColor(COLOR_RESET);
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

    void openDataFiles() {
        timelineFile.open("graph_data/allocation_timeline.csv");
        queueFile.open("graph_data/queue_sizes.csv");
        priorityFile.open("graph_data/priority_distribution.csv");
        doctorFile.open("graph_data/doctor_utilization.csv");
        mlScoreFile.open("graph_data/ml_scores.csv");

        // Headers
        timelineFile << "Iteration,PatientID,PatientName,Priority,DoctorID,DoctorName,Department,Timestamp\n";
        queueFile << "Iteration,TotalWaiting,CardiologyWaiting,EmergencyWaiting\n";
        priorityFile << "Category,Count\n";
        doctorFile << "DoctorID,DoctorName,Department,StartTime,EndTime,PatientName,Priority\n";
        mlScoreFile << "PatientID,PatientName,Age,MLScore,Department\n";
    }

    void closeDataFiles() {
        timelineFile.close();
        queueFile.close();
        priorityFile.close();
        doctorFile.close();
        mlScoreFile.close();

        // Write priority distribution
        std::ofstream pf("graph_data/priority_distribution.csv");
        pf << "Category,Count\n";
        pf << "Critical (90-100)," << critical << "\n";
        pf << "High (75-89)," << high << "\n";
        pf << "Medium (60-74)," << medium << "\n";
        pf << "Low (<60)," << low << "\n";
        pf.close();
    }

public:
    GraphGeneratingHospital()
        : totalPatients(0), treated(0), critical(0), high(0), medium(0), low(0) {

        // Create graph_data directory
        system("mkdir graph_data 2>nul");
    }

    ~GraphGeneratingHospital() {
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {
            delete it->second;
        }
    }

    void run() {
        system("cls");
        setColor(COLOR_CYAN);
        std::cout << "========================================================================\n";
        std::cout << "      HOSPITAL TRIAGE SYSTEM - GRAPH GENERATION MODE                   \n";
        std::cout << "========================================================================\n\n";
        setColor(COLOR_RESET);

        openDataFiles();

        std::cout << "Loading data...\n";
        loadDoctors();
        loadPatients();

        std::cout << "\nStarting allocation...\n";
        allocate();

        closeDataFiles();

        std::cout << "\n";
        setColor(COLOR_GREEN);
        std::cout << "========================================\n";
        std::cout << "  ALL PATIENTS TREATED!\n";
        std::cout << "========================================\n\n";
        setColor(COLOR_RESET);

        std::cout << "Total: " << totalPatients << "\n";
        std::cout << "Treated: " << treated << "\n";
        std::cout << "Critical: " << critical << "\n";
        std::cout << "High Priority: " << high << "\n\n";

        setColor(COLOR_CYAN);
        std::cout << "Data saved to graph_data/ folder\n";
        std::cout << "Run: python generate_graphs.py to create visualizations\n";
        setColor(COLOR_RESET);

        system("pause");
    }

    void allocate() {
        int iteration = 0;
        int timestamp = 0;

        while (true) {
            iteration++;
            bool anyAction = false;

            // Update doctors
            for (size_t i = 0; i < doctors.size(); i++) {
                if (doctors[i].isBusy && doctors[i].timeLeft > 0) {
                    doctors[i].timeLeft--;

                    if (doctors[i].timeLeft == 0) {
                        // Log doctor completion
                        treated++;
                        delete doctors[i].currentPatient;
                        doctors[i].currentPatient = NULL;
                        doctors[i].isBusy = false;
                        anyAction = true;
                    }
                }
            }

            // Allocate patients
            for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
                 it != queues.end(); ++it) {

                std::string dept = it->first;
                PatientQueue* queue = it->second;

                if (!queue->isEmpty()) {
                    Doctor* doc = findDoctor(dept);

                    if (doc != NULL) {
                        Patient p = queue->extractMax();
                        Patient* assignedP = new Patient(p);

                        doc->isBusy = true;
                        doc->currentPatient = assignedP;
                        doc->timeLeft = getTreatmentTime(p.priority);

                        // Log allocation
                        AllocationEvent event;
                        event.iteration = iteration;
                        event.patientId = p.id;
                        event.patientName = p.name;
                        event.priority = p.priority;
                        event.doctorId = doc->id;
                        event.doctorName = doc->name;
                        event.department = dept;
                        event.timestamp = timestamp;

                        allocationHistory.push_back(event);

                        // Write to timeline file
                        timelineFile << iteration << "," << p.id << "," << p.name << ","
                                    << p.priority << "," << doc->id << "," << doc->name << ","
                                    << dept << "," << timestamp << "\n";

                        // Write to doctor file
                        doctorFile << doc->id << "," << doc->name << "," << dept << ","
                                  << timestamp << "," << (timestamp + doc->timeLeft) << ","
                                  << p.name << "," << p.priority << "\n";

                        anyAction = true;
                    }
                }
            }

            // Log queue sizes
            int cardioWaiting = 0, emergWaiting = 0;
            for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
                 it != queues.end(); ++it) {
                if (it->first == "Cardiology") cardioWaiting = it->second->size();
                else if (it->first == "Emergency") emergWaiting = it->second->size();
            }

            int totalWaiting = cardioWaiting + emergWaiting;
            queueFile << iteration << "," << totalWaiting << ","
                     << cardioWaiting << "," << emergWaiting << "\n";

            queueSizeHistory.push_back(std::make_pair(iteration, totalWaiting));

            std::cout << "Iteration " << iteration << " - Waiting: " << totalWaiting << "\r";
            std::cout.flush();

            // Check if done
            bool anyBusy = false;
            for (size_t i = 0; i < doctors.size(); i++) {
                if (doctors[i].isBusy) {
                    anyBusy = true;
                    break;
                }
            }

            if (totalWaiting == 0 && !anyBusy) {
                break;
            }

            timestamp++;
            Sleep(100); // Small delay for realism
        }
        std::cout << "\n";
    }
};

int main() {
    GraphGeneratingHospital system;
    system.run();
    return 0;
}
