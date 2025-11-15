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

class HTMLDashboardHospital {
private:
    std::map<std::string, PatientQueue*> queues;
    std::vector<Doctor> doctors;
    int totalPatients, treated, critical, high;
    std::ofstream jsonFile;

    std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else result += c;
        }
        return result;
    }

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

            if (score >= 90) critical++;
            else if (score >= 75) high++;

            std::cout << "Patient " << (i+1) << "/" << totalPatients << "\r";
            std::cout.flush();

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

    void saveIterationState(int iteration) {
        jsonFile << (iteration > 1 ? "," : "") << "\n    {\n";
        jsonFile << "      \"iteration\": " << iteration << ",\n";

        // Doctors
        jsonFile << "      \"doctors\": [\n";
        for (size_t i = 0; i < doctors.size(); i++) {
            jsonFile << "        {\n";
            jsonFile << "          \"id\": " << doctors[i].id << ",\n";
            jsonFile << "          \"name\": \"" << escapeJson(doctors[i].name) << "\",\n";
            jsonFile << "          \"department\": \"" << escapeJson(doctors[i].department) << "\",\n";
            jsonFile << "          \"isBusy\": " << (doctors[i].isBusy ? "true" : "false");

            if (doctors[i].isBusy && doctors[i].currentPatient) {
                jsonFile << ",\n";
                jsonFile << "          \"currentPatient\": \"" << escapeJson(doctors[i].currentPatient->name) << "\",\n";
                jsonFile << "          \"patientPriority\": " << doctors[i].currentPatient->priority << ",\n";
                jsonFile << "          \"timeLeft\": " << doctors[i].timeLeft << "\n";
            } else {
                jsonFile << "\n";
            }

            jsonFile << "        }";
            if (i < doctors.size() - 1) jsonFile << ",";
            jsonFile << "\n";
        }
        jsonFile << "      ],\n";

        // Queues
        jsonFile << "      \"queues\": {\n";
        int queueIdx = 0;
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {

            jsonFile << "        \"" << it->first << "\": [\n";

            std::vector<Patient> waiting = it->second->getAll();
            for (size_t i = 0; i < waiting.size() && i < 10; i++) {
                jsonFile << "          {\n";
                jsonFile << "            \"id\": " << waiting[i].id << ",\n";
                jsonFile << "            \"name\": \"" << escapeJson(waiting[i].name) << "\",\n";
                jsonFile << "            \"priority\": " << waiting[i].priority << ",\n";
                jsonFile << "            \"age\": " << waiting[i].age << "\n";
                jsonFile << "          }";
                if (i < waiting.size() - 1 && i < 9) jsonFile << ",";
                jsonFile << "\n";
            }

            jsonFile << "        ]";
            if (++queueIdx < queues.size()) jsonFile << ",";
            jsonFile << "\n";
        }
        jsonFile << "      },\n";

        // Stats
        int totalWaiting = 0;
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {
            totalWaiting += it->second->size();
        }

        jsonFile << "      \"stats\": {\n";
        jsonFile << "        \"totalPatients\": " << totalPatients << ",\n";
        jsonFile << "        \"treated\": " << treated << ",\n";
        jsonFile << "        \"waiting\": " << totalWaiting << ",\n";
        jsonFile << "        \"critical\": " << critical << ",\n";
        jsonFile << "        \"high\": " << high << "\n";
        jsonFile << "      }\n";

        jsonFile << "    }";
    }

public:
    HTMLDashboardHospital()
        : totalPatients(0), treated(0), critical(0), high(0) {}

    ~HTMLDashboardHospital() {
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {
            delete it->second;
        }
    }

    void run() {
        system("cls");
        setColor(COLOR_CYAN);
        std::cout << "========================================================================\n";
        std::cout << "      HOSPITAL TRIAGE SYSTEM - HTML DASHBOARD MODE                     \n";
        std::cout << "========================================================================\n\n";
        setColor(COLOR_RESET);

        jsonFile.open("dashboard_data.json");
        jsonFile << "{\n";
        jsonFile << "  \"iterations\": [\n";

        std::cout << "Loading data...\n";
        loadDoctors();
        loadPatients();

        std::cout << "\nStarting allocation...\n";
        allocate();

        jsonFile << "\n  ]\n";
        jsonFile << "}\n";
        jsonFile.close();

        std::cout << "\n";
        setColor(COLOR_GREEN);
        std::cout << "========================================\n";
        std::cout << "  DATA GENERATED SUCCESSFULLY!\n";
        std::cout << "========================================\n\n";
        setColor(COLOR_RESET);

        std::cout << "Total: " << totalPatients << "\n";
        std::cout << "Treated: " << treated << "\n\n";

        setColor(COLOR_CYAN);
        std::cout << "Run: python create_dashboard.py\n";
        std::cout << "Then open: dashboard.html in your browser\n";
        setColor(COLOR_RESET);

        system("pause");
    }

    void allocate() {
        int iteration = 0;

        while (true) {
            iteration++;
            bool anyAction = false;

            // Update doctors
            for (size_t i = 0; i < doctors.size(); i++) {
                if (doctors[i].isBusy && doctors[i].timeLeft > 0) {
                    doctors[i].timeLeft--;

                    if (doctors[i].timeLeft == 0) {
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

                        anyAction = true;
                    }
                }
            }

            // Save state
            saveIterationState(iteration);

            std::cout << "Iteration " << iteration << "\r";
            std::cout.flush();

            // Check if done
            int totalWaiting = 0;
            for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
                 it != queues.end(); ++it) {
                totalWaiting += it->second->size();
            }

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

            Sleep(100);
        }
        std::cout << "\n";
    }
};

int main() {
    HTMLDashboardHospital system;
    system.run();
    return 0;
}
