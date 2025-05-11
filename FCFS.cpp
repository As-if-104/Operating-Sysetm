#include<bits/stdc++.h>
using namespace std;

struct Process
{
    int id;
    int burstTime;
    int arrivalTime;
    int startTime;
    int waitingTime;
    int turnaroundTime;
};

bool compareArrival(const Process &a, const Process &b)
{
    return a.arrivalTime < b.arrivalTime;
}

void fcfs(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), compareArrival);

    int currentTime = 0;

    for (int i = 0; i < processes.size(); i++)
    {
        if (currentTime < processes[i].arrivalTime)
        {
            currentTime = processes[i].arrivalTime;
        }

        processes[i].startTime = currentTime;
        processes[i].waitingTime = currentTime - processes[i].arrivalTime;
        processes[i].turnaroundTime = processes[i].waitingTime + processes[i].burstTime;

        currentTime += processes[i].burstTime;
    }
}
void printGanttChart(const vector<Process> &processes)
{
    cout << "Gantt Chart" << endl;
    for (const auto &process : processes)
    {
        cout << process.startTime << " P" << process.id << " ";
    }
    cout << endl
         << endl;
}

void printProcessDetails(const vector<Process> &processes)
{
    for (const auto &process : processes)
    {
        cout << "Process: P" << process.id
             << " Start time: " << setw(2) << process.startTime
             << " Waiting time: " << setw(2) << process.waitingTime
             << " Turnaround time: " << setw(2) << process.turnaroundTime << endl;
    }
}

void printAverageMetrics(const vector<Process> &processes)
{
    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    for (const auto &process : processes)
    {
        totalWaitingTime += process.waitingTime;
        totalTurnaroundTime += process.turnaroundTime;
    }

    double avgWaitingTime = totalWaitingTime / processes.size();
    double avgTurnaroundTime = totalTurnaroundTime / processes.size();

    cout << "\nAverage waiting time : " << avgWaitingTime << endl;
    cout << "Average turnaround time : " << avgTurnaroundTime << endl;
}

int main()
{
    int n;
    cout << "Number of process, n: ";
    cin >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; i++)
    {
        processes[i].id = i + 1;

        cout << "Enter the burst time of P" << i + 1 << ": ";
        cin >> processes[i].burstTime;

        cout << "Enter the arrival time of P" << i + 1 << ": ";
        cin >> processes[i].arrivalTime;
    }
    fcfs(processes);

    cout << "\nSample Output:" << endl;
    printGanttChart(processes);
    printProcessDetails(processes);
    printAverageMetrics(processes);
}
