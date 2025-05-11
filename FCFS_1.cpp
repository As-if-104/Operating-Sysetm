#include <bits/stdc++.h>
using namespace std;

struct process{
    int id;
    int ArrivalTime;
    int BurstTime;
    int WaitingTime;
    int TurnAroundTime;
    int StartTime;
};

bool CompareArrival(const process &a, const process &b){
    return a.ArrivalTime < b. ArrivalTime;
}

void fcfs(vector<process> &processes){
    sort(processes.begin(), processes.end(), CompareArrival);
    int currTime = 0;

    for(int i = 0; i < processes.size(); i++){
        if(currTime < processes[i].ArrivalTime){
            currTime = processes[i].ArrivalTime;
        }

        processes[i].StartTime = currTime;
        processes[i].WaitingTime = currTime - processes[i].ArrivalTime;
        processes[i].TurnAroundTime = processes[i].WaitingTime + processes[i].BurstTime;

        currTime += processes[i].BurstTime;
    }
}

void printGanttChart(const vector<process> &processes){
    cout << "Gantt Chart" << endl;
    for (const auto &process : processes){
        cout << process.StartTime << " P" << process.id << " ";
    }
    cout << endl
         << endl;
}

void printProcessDetails(const vector<process> &processes){
    for(const auto &process: processes){
        cout << "Process: P" << process.id
             << " Start Time: " << process.StartTime
             << " Waiting Time: " << process.WaitingTime
             << " TuranAround Time: " << process.TurnAroundTime << endl;
    }
}

void printAverageMetrics(const vector<process> &processes){
    double totalWaitingTime = 0;
    double totalTrunAroundTime = 0;

    for(const auto &process: processes){
        totalWaitingTime += process.WaitingTime;
        totalTrunAroundTime += process.TurnAroundTime;
    }

    double AverageWaitingTime = totalWaitingTime/processes.size();
    double AverageTurnAroundTime = totalTrunAroundTime/processes.size(); 

    cout << "\nAverage Waiting Time: " << AverageWaitingTime << endl;
    cout << "Average Turn Around Time: " << AverageTurnAroundTime << endl;
}
main(){
    int n;
    cout << "Number of process: ";
    cin >> n;

    vector<process> processes(n);

    for(int i = 0; i < n; i++){
        processes[i].id = i+1;

        cout << "Enter the Burst Time of P" << i+1 << ": ";
        cin >> processes[i].BurstTime;

        cout << "Enter the Arrival Time of P" << i+1 << ": ";
        cin >> processes[i].ArrivalTime;
    }

    fcfs(processes);
    
    cout << "\nSample Output: " << endl;

    printGanttChart(processes);
    printProcessDetails(processes);
    printAverageMetrics(processes);
}