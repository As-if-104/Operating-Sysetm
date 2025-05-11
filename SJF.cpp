#include <bits/stdc++.h>
using namespace std;

class Process
{
private:
    int id;
    int burstTime;
    int arrivalTime;
    int startTime;
    int waitingTime;
    int turnaroundTime;
    bool isCompleted;

public:
    Process(int id, int burst, int arrival)
    {
        this->id = id;
        this->burstTime = burst;
        this->arrivalTime = arrival;
        this->startTime = 0;
        this->waitingTime = 0;
        this->turnaroundTime = 0;
        this->isCompleted = false;
    }

    int getId() const { return id; }
    int getBurstTime() const { return burstTime; }
    int getArrivalTime() const { return arrivalTime; }
    int getStartTime() const { return startTime; }
    int getWaitingTime() const { return waitingTime; }
    int getTurnaroundTime() const { return turnaroundTime; }
    bool getIsCompleted() const { return isCompleted; }

    void setStartTime(int time) { startTime = time; }
    void setWaitingTime(int time) { waitingTime = time; }
    void setTurnaroundTime(int time) { turnaroundTime = time; }
    void setCompleted(bool completed) { isCompleted = completed; }
};

class SJFScheduler
{
private:
    vector<Process> processes;
    vector<Process> completedProcesses;
    double avgWaitingTime;
    double avgTurnaroundTime;

public:
    void addProcess(const Process &process)
    {
        processes.push_back(process);
    }

    int findNextProcess(int currentTime)
    {
        int minBurst = INT_MAX;
        int selectedProcess = -1;

        for (int i = 0; i < processes.size(); i++)
        {
            if (processes[i].getArrivalTime() <= currentTime &&
                !processes[i].getIsCompleted())
            {
                if (processes[i].getBurstTime() < minBurst)
                {
                    minBurst = processes[i].getBurstTime();
                    selectedProcess = i;
                }
            }
        }

        return selectedProcess;
    }

    void schedule()
    {
        int currentTime = 0;
        int completed = 0;
        double totalWaitingTime = 0;
        double totalTurnaroundTime = 0;
        while (completed < processes.size())
        {
            int next = findNextProcess(currentTime);
            if (next == -1)
            {
                currentTime++;
                continue;
            }
            processes[next].setStartTime(currentTime);
            processes[next].setWaitingTime(currentTime - processes[next].getArrivalTime());
            processes[next].setTurnaroundTime(processes[next].getWaitingTime() + processes[next].getBurstTime());

            totalWaitingTime += processes[next].getWaitingTime();
            totalTurnaroundTime += processes[next].getTurnaroundTime();

            currentTime += processes[next].getBurstTime();

            processes[next].setCompleted(true);
            completedProcesses.push_back(processes[next]);
            completed++;
        }

        avgWaitingTime = totalWaitingTime / processes.size();
        avgTurnaroundTime = totalTurnaroundTime / processes.size();
    }
    void displayGanttChart() const
    {
        cout << "Gantt Chart" << endl;
        for (const auto &process : completedProcesses)
        {
            cout << process.getStartTime() << " P" << process.getId() << " ";
        }
        cout << endl
             << endl;
    }

    void displayProcessDetails() const
    {
        for (const auto &process : completedProcesses)
        {
            cout << "Process: P" << process.getId()
                 << " Start time: " << setw(2) << process.getStartTime()
                 << " Waiting time: " << setw(2) << process.getWaitingTime()
                 << " Turnaround time: " << setw(2) << process.getTurnaroundTime() << endl;
        }
    }
    void displayAverageMetrics() const
    {
        cout << "\nAverage waiting time : " << fixed << setprecision(1) << avgWaitingTime << endl;
        cout << "Average turnaround time : " << fixed << setprecision(1) << avgTurnaroundTime << endl;
    }
};

int main()
{
    int n;
    cout << "Number of process, n: ";
    cin >> n;

    SJFScheduler scheduler;

    for (int i = 0; i < n; i++)
    {
        int burst, arrival;

        cout << "Enter the burst time of P" << i + 1 << ": ";
        cin >> burst;

        cout << "Enter the arrival time of P" << i + 1 << ": ";
        cin >> arrival;

        Process process(i + 1, burst, arrival);
        scheduler.addProcess(process);
    }

    scheduler.schedule();

    cout << "\nSample Output:" << endl;
    scheduler.displayGanttChart();
    scheduler.displayProcessDetails();
    scheduler.displayAverageMetrics();
}