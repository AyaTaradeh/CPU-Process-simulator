#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <tuple>
using namespace std;


struct Process {
    int id;
    int arrivalTime;
    int cpuBurst;
    int remainingTime;
    int startTime;
    int completionTime;
};

class Simulator {
private:
    vector<Process> processes;
    int contextSwitchTime;
    int timeQuantum;
    int currentTime;
    vector<int> ganttChart;

public:
    Simulator(vector<Process> processes, int contextSwitchTime, int timeQuantum) {
        this->processes = processes;
        this->contextSwitchTime = contextSwitchTime;
        this->timeQuantum = timeQuantum;
        this->currentTime = 0;
        this->ganttChart.reserve(processes.size() * 10); // reserve memory for ganttChart
    }

    void fcfs() {
        for (Process& process : processes) {
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }
            process.startTime = currentTime;
            currentTime += process.cpuBurst;
            process.completionTime = currentTime;
            ganttChart.push_back(process.id);
        }
    }

    void srt() {
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> queue;
        for (Process& process : processes) {
            queue.push({ process.arrivalTime, process.id });
        }

        while (!queue.empty()) {
            int arrivalTime, id;
            tie(arrivalTime, id) = queue.top();
            queue.pop();

            Process& process = processes[id];
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }
            process.startTime = currentTime;
            if (process.remainingTime <= timeQuantum) {
                currentTime += process.remainingTime;
                process.completionTime = currentTime;
                process.remainingTime = 0;
            }
            else {
                currentTime += timeQuantum;
                process.remainingTime -= timeQuantum;
                queue.push({ currentTime, id });
            }
            ganttChart.push_back(process.id);
        }
    }

    void roundRobin() {
        queue<int> queue;
        for (int i = 0; i < processes.size(); i++) {
            queue.push(i);
        }

        while (!queue.empty()) {
            int id = queue.front();
            queue.pop();

            Process& process = processes[id];
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }
            process.startTime = currentTime;
            if (process.remainingTime <= timeQuantum) {
                currentTime += process.remainingTime;
                process.completionTime = currentTime;
                process.remainingTime = 0;
            }
            else {
                currentTime += timeQuantum;
                process.remainingTime -= timeQuantum;
                queue.push(id);
            }
            ganttChart.push_back(process.id);
        }
    }

    void displayResults() {
        cout << "Gantt Chart: ";
        for (int id : ganttChart) {
            cout << id << " ";
        }
        cout << endl;

        double totalWaitingTime = 0;
        double totalTurnaroundTime = 0;
        for (Process& process : processes) {
            int waitingTime = process.startTime - process.arrivalTime;
            int turnaroundTime = process.completionTime - process.arrivalTime;
            totalWaitingTime += waitingTime; totalTurnaroundTime += turnaroundTime;
            cout << "Process " << process.id << ": Finish Time = " << process.completionTime << ", Waiting Time = " << waitingTime << ", Turnaround Time = " << turnaroundTime << endl;
        }

        double cpuUtilization = (currentTime - contextSwitchTime * (processes.size() - 1)) / (double)currentTime;
        cout << "CPU Utilization: " << cpuUtilization << endl;
    }
};

int main() {
    ifstream inputFile("C:\\Users\\hamza\\Desktop\\Aya\\aya\\OS\\input.txt");
    int contextSwitchTime, timeQuantum, numProcesses;
    inputFile >> contextSwitchTime >> timeQuantum >> numProcesses;

    vector<Process> processes;
    for (int i = 0; i < numProcesses; i++) {
        Process process;
        process.id = i;
        inputFile >> process.arrivalTime >> process.cpuBurst;
        process.remainingTime = process.cpuBurst;
        processes.push_back(process);
    }

    Simulator simulator(processes, contextSwitchTime, timeQuantum);

    cout << "FCFS Scheduling:" << endl;
    simulator.fcfs();
    simulator.displayResults();

    cout << "SRT Scheduling:" << endl;
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime || (a.arrivalTime == b.arrivalTime && a.cpuBurst < b.cpuBurst);
        });
    simulator.srt();
    simulator.displayResults();

    cout << "Round Robin Scheduling:" << endl;
    simulator.roundRobin();
    simulator.displayResults();

    return 0;
}