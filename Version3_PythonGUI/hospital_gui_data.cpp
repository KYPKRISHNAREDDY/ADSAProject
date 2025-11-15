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

// Simple version that just generates data for GUI
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

class GUIDataGenerator {
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
        std::cout << "Loaded " << doctors.size() << " doctors\n";
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
        std::cout << "Loaded " << totalPatients << " patients\n";

        std::cout << "ML scoring...\n";

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

        std::cout << "\nAll patients scored!\n";
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

public:
    GUIDataGenerator() : totalPatients(0), treated(0), critical(0), high(0) {}

    ~GUIDataGenerator() {
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {
            delete it->second;
        }
    }

    void run() {
        system("cls");
        std::cout << "========================================================================\n";
        std::cout << "      HOSPITAL TRIAGE SYSTEM - GUI DATA GENERATOR                      \n";
        std::cout << "========================================================================\n\n";

        std::cout << "Loading data...\n";
        loadDoctors();
        loadPatients();

        std::cout << "\nGenerating GUI data file...\n";

        std::ofstream dataFile("gui_data.txt");
        dataFile << totalPatients << "\n";
        dataFile << critical << "\n";
        dataFile << high << "\n";

        // Save doctors
        dataFile << doctors.size() << "\n";
        for (size_t i = 0; i < doctors.size(); i++) {
            dataFile << doctors[i].id << "|" << doctors[i].name << "|"
                    << doctors[i].department << "\n";
        }

        // Save all patients with priorities
        std::vector<Patient> allPatients;
        for (std::map<std::string, PatientQueue*>::iterator it = queues.begin();
             it != queues.end(); ++it) {
            std::vector<Patient> dept = it->second->getAll();
            allPatients.insert(allPatients.end(), dept.begin(), dept.end());
        }

        dataFile << allPatients.size() << "\n";
        for (size_t i = 0; i < allPatients.size(); i++) {
            dataFile << allPatients[i].id << "|" << allPatients[i].name << "|"
                    << allPatients[i].age << "|" << allPatients[i].department << "|"
                    << allPatients[i].priority << "\n";
        }

        dataFile.close();

        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "  DATA GENERATED SUCCESSFULLY!\n";
        std::cout << "========================================\n\n";

        std::cout << "Total: " << totalPatients << "\n";
        std::cout << "Critical: " << critical << "\n\n";

        std::cout << "Run: python gui_visualizer.py\n";
        std::cout << "to launch the GUI window!\n\n";

        system("pause");
    }
};

int main() {
    GUIDataGenerator system;
    system.run();
    return 0;
}
