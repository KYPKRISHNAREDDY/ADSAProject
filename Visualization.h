#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <fstream>

// ANSI color codes for terminal visualization
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

// Background colors
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"

class Visualization {
private:
    int frameCount;
    std::ofstream logFile;

    std::string getPriorityColor(double priority) {
        if (priority >= 90) return std::string(BG_RED) + WHITE + BOLD;
        else if (priority >= 75) return std::string(RED) + BOLD;
        else if (priority >= 60) return YELLOW;
        else if (priority >= 40) return CYAN;
        else return GREEN;
    }

    std::string getPriorityLabel(double priority) {
        if (priority >= 90) return "CRITICAL";
        else if (priority >= 75) return "HIGH";
        else if (priority >= 60) return "MEDIUM";
        else if (priority >= 40) return "LOW";
        else return "ROUTINE";
    }

    void drawBox(const std::string& content, int width, const std::string& color = "") {
        std::cout << color << "╔" << std::string(width - 2, '═') << "╗" << RESET << std::endl;
        std::cout << color << "║ " << std::setw(width - 4) << std::left << content << " ║" << RESET << std::endl;
        std::cout << color << "╚" << std::string(width - 2, '═') << "╝" << RESET << std::endl;
    }

    void drawProgressBar(int current, int total, int width = 40) {
        float progress = (total > 0) ? (float)current / total : 0;
        int pos = (int)(width * progress);

        std::cout << "[";
        for (int i = 0; i < width; ++i) {
            if (i < pos) std::cout << GREEN << "█" << RESET;
            else std::cout << "░";
        }
        std::cout << "] " << current << "/" << total << std::endl;
    }

public:
    Visualization() : frameCount(0) {
        logFile.open("triage_log.txt", std::ios::app);
        if (logFile.is_open()) {
            logFile << "\n\n=== New Session Started ===\n";
        }
    }

    ~Visualization() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void clearScreen() {
        // ANSI escape code to clear screen
        std::cout << "\033[2J\033[1;1H";
    }

    void displayHeader() {
        std::cout << BOLD << CYAN;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   🏥 SMART HOSPITAL TRIAGE SYSTEM 🏥                          ║\n";
        std::cout << "║                  Real-time ML-Based Patient Allocation                        ║\n";
        std::cout << "║                    Using Fibonacci Heap Data Structure                        ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << RESET << std::endl;
    }

    void displayDoctorStatus(int /* docId */, const std::string& name, const std::string& dept,
                            bool isBusy, const std::string& currentPatient = "",
                            double patientPriority = 0, int treatmentTime = 0) {
        std::string status;
        std::string color;

        if (isBusy) {
            status = "BUSY";
            color = std::string(RED) + BOLD;
            std::cout << "┌─────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ " << YELLOW << "Dr. " << name << " (" << dept << ")" << RESET;
            std::cout << std::string(45 - name.length() - dept.length(), ' ') << "│\n";
            std::cout << "│ Status: " << color << status << RESET;
            std::cout << " │ Time Remaining: " << CYAN << treatmentTime << "s" << RESET;
            std::cout << std::string(20 - std::to_string(treatmentTime).length(), ' ') << "│\n";
            std::cout << "│ Treating: " << BOLD << currentPatient << RESET;
            std::cout << std::string(50 - currentPatient.length(), ' ') << "│\n";
            std::cout << "│ Priority: " << getPriorityColor(patientPriority) << " "
                     << std::fixed << std::setprecision(1) << patientPriority
                     << " (" << getPriorityLabel(patientPriority) << ") " << RESET;
            std::cout << std::string(30, ' ') << "│\n";
            std::cout << "└─────────────────────────────────────────────────────────────────┘\n";
        } else {
            status = "AVAILABLE";
            color = std::string(GREEN) + BOLD;
            std::cout << "┌─────────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ " << YELLOW << "Dr. " << name << " (" << dept << ")" << RESET;
            std::cout << std::string(45 - name.length() - dept.length(), ' ') << "│\n";
            std::cout << "│ Status: " << color << status << RESET;
            std::cout << " ✓ Ready to accept patients";
            std::cout << std::string(20, ' ') << "│\n";
            std::cout << "└─────────────────────────────────────────────────────────────────┘\n";
        }
    }

    void displayWaitingQueue(const std::vector<Patient>& patients,
                            const std::vector<double>& priorities,
                            const std::string& department) {
        std::cout << "\n" << BOLD << MAGENTA;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  WAITING QUEUE - " << department << " DEPARTMENT";
        std::cout << std::string(56 - department.length(), ' ') << "║\n";
        std::cout << "║  (Fibonacci Heap - Max Priority First)";
        std::cout << std::string(42, ' ') << "║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << RESET;

        if (patients.empty()) {
            std::cout << GREEN << "  ✓ No patients waiting in queue\n" << RESET;
            return;
        }

        std::cout << "\n┌──────┬─────────────────────┬──────┬──────────┬──────────────────────────────┐\n";
        std::cout << "│ " << BOLD << "Pos" << RESET << "  │ " << BOLD << "Name" << RESET;
        std::cout << std::string(16, ' ') << "│ " << BOLD << "Age" << RESET << "  │ ";
        std::cout << BOLD << "Priority" << RESET << " │ " << BOLD << "Status" << RESET;
        std::cout << std::string(23, ' ') << "│\n";
        std::cout << "├──────┼─────────────────────┼──────┼──────────┼──────────────────────────────┤\n";

        for (size_t i = 0; i < patients.size() && i < 10; i++) {
            std::cout << "│ " << std::setw(4) << i + 1 << " │ ";
            std::cout << std::setw(19) << std::left << patients[i].name.substr(0, 19) << " │ ";
            std::cout << std::setw(4) << patients[i].age << " │ ";

            std::string priorityStr = std::to_string((int)priorities[i]);
            std::cout << getPriorityColor(priorities[i]) << std::setw(8) << priorityStr << RESET << " │ ";
            std::cout << getPriorityColor(priorities[i]) << std::setw(28) << std::left
                     << getPriorityLabel(priorities[i]) << RESET << " │\n";
        }

        std::cout << "└──────┴─────────────────────┴──────┴──────────┴──────────────────────────────┘\n";

        if (patients.size() > 10) {
            std::cout << YELLOW << "  ... and " << (patients.size() - 10) << " more patients waiting\n" << RESET;
        }
    }

    void displayStatistics(int totalPatients, int treated, int waiting, int critical, int high) {
        std::cout << "\n" << BOLD << BLUE;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  REAL-TIME STATISTICS & ANALYTICS                                             ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << RESET;

        std::cout << "\n  Total Patients Arrived: " << CYAN << BOLD << totalPatients << RESET << "\n";
        std::cout << "  Patients Treated:       " << GREEN << BOLD << treated << RESET << "\n";
        std::cout << "  Patients Waiting:       " << YELLOW << BOLD << waiting << RESET << "\n";
        std::cout << "  Critical Cases:         " << RED << BOLD << critical << RESET << "\n";
        std::cout << "  High Priority Cases:    " << MAGENTA << BOLD << high << RESET << "\n\n";

        std::cout << "  Treatment Progress:\n  ";
        drawProgressBar(treated, totalPatients, 60);

        if (waiting > 0) {
            float avgWaitScore = (float)(critical * 95 + high * 80) / (critical + high + 1);
            std::cout << "\n  Average Wait Queue Priority: " << YELLOW << BOLD
                     << std::fixed << std::setprecision(1) << avgWaitScore << RESET << "\n";
        }
    }

    void displayEvent(const std::string& event, const std::string& color = CYAN) {
        std::cout << "\n" << color << "► " << event << RESET << std::endl;

        if (logFile.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            logFile << std::ctime(&time) << " - " << event << std::endl;
        }
    }

    void displayAlgorithmInfo() {
        std::cout << "\n" << BOLD << CYAN;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  DATA STRUCTURE & ALGORITHM DETAILS                                           ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝\n";
        std::cout << RESET;

        std::cout << "\n  " << YELLOW << "● Fibonacci Heap Properties:" << RESET << "\n";
        std::cout << "    - Insert Operation:        O(1) amortized\n";
        std::cout << "    - Find Maximum:            O(1)\n";
        std::cout << "    - Extract Maximum:         O(log n) amortized\n";
        std::cout << "    - Decrease/Increase Key:   O(1) amortized\n";
        std::cout << "    - Delete:                  O(log n) amortized\n";
        std::cout << "    - Merge:                   O(1)\n\n";

        std::cout << "  " << YELLOW << "● ML Model:" << RESET << "\n";
        std::cout << "    - Algorithm:    Random Forest Regressor\n";
        std::cout << "    - Features:     9 medical parameters\n";
        std::cout << "    - Output:       Priority Score (0-100)\n\n";

        std::cout << "  " << YELLOW << "● Allocation Strategy:" << RESET << "\n";
        std::cout << "    - Priority-based assignment (highest first)\n";
        std::cout << "    - Department-specific doctor allocation\n";
        std::cout << "    - Dynamic load balancing\n\n";
    }

    void pauseForEffect(int milliseconds = 1000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void displaySeparator() {
        std::cout << "\n" << BOLD << WHITE;
        std::cout << "════════════════════════════════════════════════════════════════════════════════\n";
        std::cout << RESET;
    }

    void exportVisualizationData(const std::string& filename,
                                const std::vector<std::string>& labels,
                                const std::vector<int>& values) {
        std::ofstream dataFile(filename);
        if (dataFile.is_open()) {
            for (size_t i = 0; i < labels.size(); i++) {
                dataFile << labels[i] << "," << values[i] << "\n";
            }
            dataFile.close();
            std::cout << GREEN << "  ✓ Visualization data exported to " << filename << RESET << std::endl;
        }
    }

    void generateGnuplotScript() {
        std::ofstream script("plot_triage.gnu");
        if (script.is_open()) {
            script << "set terminal png size 1200,800\n";
            script << "set output 'triage_visualization.png'\n";
            script << "set title 'Hospital Triage System - Priority Distribution'\n";
            script << "set xlabel 'Priority Level'\n";
            script << "set ylabel 'Number of Patients'\n";
            script << "set style fill solid\n";
            script << "set boxwidth 0.8\n";
            script << "set grid\n";
            script << "plot 'priority_data.csv' using 2:xtic(1) with boxes title 'Patients' lc rgb '#00AA00'\n";
            script.close();
        }
    }
};

#endif // VISUALIZATION_H
