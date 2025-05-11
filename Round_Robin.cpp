#include <bits/stdc++.h>
using namespace std;

struct Process
{
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int start_time;
    int response_time;
    int waiting_time;
    int turnaround_time;
    bool started;
};

void roundRobin(vector<Process> &processes, int quantum)
{
    int n = processes.size();
    queue<int> ready_queue;
    vector<pair<int, int>> gantt;

    int current_time = 0;
    int completed = 0;
    vector<bool> in_queue(n, false);

    while (completed < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && !in_queue[i])
            {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        if (ready_queue.empty())
        {
            current_time++;
            continue;
        }

        int idx = ready_queue.front();
        ready_queue.pop();
        in_queue[idx] = false;

        if (!processes[idx].started)
        {
            processes[idx].start_time = current_time;
            processes[idx].started = true;
            processes[idx].response_time = processes[idx].start_time - processes[idx].arrival_time;
        }

        int execute_time = min(quantum, processes[idx].remaining_time);
        processes[idx].remaining_time -= execute_time;
        current_time += execute_time;

        gantt.push_back({idx, current_time});

        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && !in_queue[i] && i != idx)
            {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        if (processes[idx].remaining_time > 0)
        {
            ready_queue.push(idx);
            in_queue[idx] = true;
        }
        else
        {
            processes[idx].completion_time = current_time;
            processes[idx].turnaround_time = processes[idx].completion_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            completed++;
        }
    }

    cout << "Gantt Chart:" << endl;
    for (int i = 0; i < gantt.size(); i++)
    {
        cout << "P" << processes[gantt[i].first].id << " ";
    }
    cout << endl;

    int prev_time = 0;
    for (int i = 0; i < gantt.size(); i++)
    {
        cout << prev_time << " ";
        prev_time = gantt[i].second;
    }
    cout << prev_time << endl;

    cout << "\nProcess Details:" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << "Process: P" << processes[i].id << " Start time: " << processes[i].start_time
             << " End time: " << processes[i].completion_time
             << " Response Time: " << processes[i].response_time
             << " Waiting time: " << processes[i].waiting_time
             << " Turnaround time: " << processes[i].turnaround_time << endl;
    }

    double avg_response_time = 0, avg_waiting_time = 0, avg_turnaround_time = 0;
    for (int i = 0; i < n; i++)
    {
        avg_response_time += processes[i].response_time;
        avg_waiting_time += processes[i].waiting_time;
        avg_turnaround_time += processes[i].turnaround_time;
    }

    avg_response_time /= n;
    avg_waiting_time /= n;
    avg_turnaround_time /= n;

    cout << "\nAverage Response Time: " << fixed << setprecision(2) << avg_response_time << endl;
    cout << "Average Waiting Time: " << fixed << setprecision(2) << avg_waiting_time << endl;
    cout << "Average Turnaround Time: " << fixed << setprecision(2) << avg_turnaround_time << endl;
}

int main()
{
    int n;
    cout << "Number of processes, n: ";
    cin >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; i++)
    {
        processes[i].id = i + 1;
        cout << "Enter the burst time of P" << i + 1 << ": ";
        cin >> processes[i].burst_time;
        cout << "Enter the arrival time of P" << i + 1 << ": ";
        cin >> processes[i].arrival_time;

        processes[i].remaining_time = processes[i].burst_time;
        processes[i].started = false;
    }

    int quantum;
    cout << "Time Quantum: ";
    cin >> quantum;

    roundRobin(processes, quantum);

    return 0;
}