#include<bits/stdc++.h>

using namespace std;

// Structure for Process
struct Process
{
    int id;              // Process ID
    int arrival_time;    // Arrival Time
    int burst_time;      // Burst Time
    int priority;        // Priority (lower number means higher priority)
    int response_time;   // Response Time
    int waiting_time;    // Waiting Time
    int turnaround_time; // Turnaround Time
    int remaining_time;  // Remaining Time for preemptive algorithms
    int start_time;      // First CPU burst start time
    int completion_time; // Completion Time
    float value_ratio;   // For the novel algorithm (priority/burst_time ratio)
};

// Structure to represent the time slice in Gantt chart
struct TimeSlice
{
    int process_id;
    int start_time;
    int end_time;
};

// Function to print the Gantt chart
void printGanttChart(const vector<TimeSlice> &gantt_chart)
{
    cout << "\nGantt Chart: ";
    for (const auto &slice : gantt_chart)
    {
        cout << "P" << slice.process_id << "(" << slice.start_time << "-" << slice.end_time << ") ";
    }
    cout << endl;
}

// Function to print process details
void printProcessDetails(const vector<Process> &processes)
{
    cout << "\nProcess Details:\n";
    cout << setw(10) << "Process" << setw(15) << "Start Time" << setw(15) << "End Time"
         << setw(15) << "Response Time" << setw(15) << "Waiting Time" << setw(20) << "Turnaround Time" << endl;

    for (const auto &p : processes)
    {
        cout << setw(10) << "P" << p.id
             << setw(15) << p.start_time
             << setw(15) << p.completion_time
             << setw(15) << p.response_time
             << setw(15) << p.waiting_time
             << setw(20) << p.turnaround_time << endl;
    }
}

// Function to print algorithm metrics
void printAlgorithmMetrics(const string &algo_name, double avg_response, double avg_waiting, double avg_turnaround)
{
    cout << "\nAlgorithm: " << algo_name << endl;
    cout << "Average Response Time: " << fixed << setprecision(2) << avg_response << endl;
    cout << "Average Waiting Time: " << fixed << setprecision(2) << avg_waiting << endl;
    cout << "Average Turnaround Time: " << fixed << setprecision(2) << avg_turnaround << endl;
}

// Function to calculate metrics for all processes
void calculateMetrics(vector<Process> &processes, vector<TimeSlice> &gantt_chart)
{
    // Initialize first response time for each process
    vector<int> first_response(processes.size() + 1, -1);

    // Go through the Gantt chart to find first response time for each process
    for (const auto &slice : gantt_chart)
    {
        if (slice.process_id != 0 && first_response[slice.process_id] == -1)
        {
            first_response[slice.process_id] = slice.start_time;
        }
    }

    // Calculate metrics for each process
    for (auto &p : processes)
    {
        p.start_time = first_response[p.id];
        p.response_time = p.start_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
    }
}

// Function to calculate and print average metrics
void calculateAverages(const vector<Process> &processes)
{
    double total_response = 0, total_waiting = 0, total_turnaround = 0;

    for (const auto &p : processes)
    {
        total_response += p.response_time;
        total_waiting += p.waiting_time;
        total_turnaround += p.turnaround_time;
    }

    double avg_response = total_response / processes.size();
    double avg_waiting = total_waiting / processes.size();
    double avg_turnaround = total_turnaround / processes.size();

    cout << "\nAverage Response Time: " << fixed << setprecision(2) << avg_response << endl;
    cout << "Average Waiting Time: " << fixed << setprecision(2) << avg_waiting << endl;
    cout << "Average Turnaround Time: " << fixed << setprecision(2) << avg_turnaround << endl;
}

// Function to prepare processes for scheduling
vector<Process> prepareProcesses(const vector<Process> &original_processes)
{
    vector<Process> processes = original_processes;
    for (auto &p : processes)
    {
        p.remaining_time = p.burst_time;
        p.start_time = -1;
        p.completion_time = 0;
        p.response_time = 0;
        p.waiting_time = 0;
        p.turnaround_time = 0;
    }
    return processes;
}

// Function to get algorithm metrics for comparison
void getAlgorithmMetrics(const vector<Process> &processes, double &avg_response, double &avg_waiting, double &avg_turnaround)
{
    double total_response = 0, total_waiting = 0, total_turnaround = 0;

    for (const auto &p : processes)
    {
        total_response += p.response_time;
        total_waiting += p.waiting_time;
        total_turnaround += p.turnaround_time;
    }

    avg_response = total_response / processes.size();
    avg_waiting = total_waiting / processes.size();
    avg_turnaround = total_turnaround / processes.size();
}

// 1. First Come First Serve (FCFS) Scheduling Algorithm
vector<Process> fcfs(vector<Process> processes, vector<TimeSlice> &gantt_chart)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrival_time < b.arrival_time; });

    int current_time = 0;
    for (auto &p : processes)
    {
        // If there's a gap between the current time and the process arrival
        if (current_time < p.arrival_time)
        {
            // Add idle time to Gantt chart if needed
            if (current_time < p.arrival_time)
            {
                gantt_chart.push_back({0, current_time, p.arrival_time}); // Process ID 0 represents idle time
            }
            current_time = p.arrival_time;
        }

        p.start_time = current_time;
        p.completion_time = current_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
        p.response_time = p.start_time - p.arrival_time;

        // Add to Gantt chart
        gantt_chart.push_back({p.id, current_time, p.completion_time});
        current_time = p.completion_time;
    }

    return processes;
}

// 2. Non-Preemptive Shortest Job First (SJF) Scheduling Algorithm
vector<Process> non_preemptive_sjf(vector<Process> processes, vector<TimeSlice> &gantt_chart)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    int n = processes.size();
    int completed = 0;
    int current_time = 0;
    vector<bool> is_completed(n, false);

    while (completed != n)
    {
        int shortest_job_idx = -1;
        int shortest_burst = numeric_limits<int>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && processes[i].arrival_time <= current_time)
            {
                if (processes[i].burst_time < shortest_burst)
                {
                    shortest_burst = processes[i].burst_time;
                    shortest_job_idx = i;
                }
            }
        }

        // If no process is available at current time
        if (shortest_job_idx == -1)
        {
            // Find the next arriving process
            int next_arrival_time = numeric_limits<int>::max();
            int next_process_idx = -1;

            for (int i = 0; i < n; i++)
            {
                if (!is_completed[i] && processes[i].arrival_time < next_arrival_time)
                {
                    next_arrival_time = processes[i].arrival_time;
                    next_process_idx = i;
                }
            }

            // Add idle time to Gantt chart
            if (current_time < next_arrival_time)
            {
                gantt_chart.push_back({0, current_time, next_arrival_time}); // Process ID 0 represents idle time
            }

            current_time = next_arrival_time;
            continue;
        }

        // Process the shortest job
        auto &p = processes[shortest_job_idx];

        if (p.start_time == -1)
        {
            p.start_time = current_time;
        }

        p.completion_time = current_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
        p.response_time = p.start_time - p.arrival_time;

        // Add to Gantt chart
        gantt_chart.push_back({p.id, current_time, p.completion_time});

        current_time = p.completion_time;
        is_completed[shortest_job_idx] = true;
        completed++;
    }

    return processes;
}

// 3. Preemptive Shortest Job First (SJF) Scheduling Algorithm (also known as SRTF)
vector<Process> preemptive_sjf(vector<Process> processes, vector<TimeSlice> &gantt_chart)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    int n = processes.size();
    int completed = 0;
    int current_time = 0;
    int prev_process_id = -1;
    vector<bool> is_completed(n, false);

    while (completed != n)
    {
        int shortest_job_idx = -1;
        int shortest_remaining_time = numeric_limits<int>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && processes[i].arrival_time <= current_time)
            {
                if (processes[i].remaining_time < shortest_remaining_time)
                {
                    shortest_remaining_time = processes[i].remaining_time;
                    shortest_job_idx = i;
                }
            }
        }

        // If no process is available at current time
        if (shortest_job_idx == -1)
        {
            // Find the next arriving process
            int next_arrival_time = numeric_limits<int>::max();

            for (int i = 0; i < n; i++)
            {
                if (!is_completed[i] && processes[i].arrival_time < next_arrival_time)
                {
                    next_arrival_time = processes[i].arrival_time;
                }
            }

            // Add idle time to Gantt chart
            if (current_time < next_arrival_time && prev_process_id != 0)
            {
                gantt_chart.push_back({0, current_time, next_arrival_time}); // Process ID 0 represents idle time
                prev_process_id = 0;
            }

            current_time = next_arrival_time;
            continue;
        }

        auto &p = processes[shortest_job_idx];

        if (p.start_time == -1)
        {
            p.start_time = current_time;
        }

        // Determine how long this process will run
        int run_time = 1; // Run for 1 time unit by default

        // Check if a new process will arrive before this process completes
        bool will_be_preempted = false;
        int next_arrival_time = numeric_limits<int>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && i != shortest_job_idx && processes[i].arrival_time > current_time)
            {
                if (processes[i].arrival_time < current_time + p.remaining_time &&
                    processes[i].arrival_time < next_arrival_time)
                {
                    next_arrival_time = processes[i].arrival_time;
                    will_be_preempted = true;
                }
            }
        }

        if (will_be_preempted)
        {
            run_time = next_arrival_time - current_time;
        }
        else
        {
            run_time = p.remaining_time;
        }

        // Update the Gantt chart
        if (prev_process_id != p.id)
        {
            gantt_chart.push_back({p.id, current_time, current_time + run_time});
            prev_process_id = p.id;
        }
        else
        {
            // Update the end time of the last slice
            gantt_chart.back().end_time = current_time + run_time;
        }

        p.remaining_time -= run_time;
        current_time += run_time;

        // Check if the process is completed
        if (p.remaining_time == 0)
        {
            p.completion_time = current_time;
            p.turnaround_time = p.completion_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            is_completed[shortest_job_idx] = true;
            completed++;
        }
    }

    return processes;
}

// 4. Non-Preemptive Priority Scheduling Algorithm
vector<Process> non_preemptive_priority(vector<Process> processes, vector<TimeSlice> &gantt_chart)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    int n = processes.size();
    int completed = 0;
    int current_time = 0;
    vector<bool> is_completed(n, false);

    while (completed != n)
    {
        int highest_priority_idx = -1;
        int highest_priority = numeric_limits<int>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && processes[i].arrival_time <= current_time)
            {
                if (processes[i].priority < highest_priority)
                {
                    highest_priority = processes[i].priority;
                    highest_priority_idx = i;
                }
            }
        }

        // If no process is available at current time
        if (highest_priority_idx == -1)
        {
            // Find the next arriving process
            int next_arrival_time = numeric_limits<int>::max();
            int next_process_idx = -1;

            for (int i = 0; i < n; i++)
            {
                if (!is_completed[i] && processes[i].arrival_time < next_arrival_time)
                {
                    next_arrival_time = processes[i].arrival_time;
                    next_process_idx = i;
                }
            }

            // Add idle time to Gantt chart
            if (current_time < next_arrival_time)
            {
                gantt_chart.push_back({0, current_time, next_arrival_time}); // Process ID 0 represents idle time
            }

            current_time = next_arrival_time;
            continue;
        }

        // Process the highest priority job
        auto &p = processes[highest_priority_idx];

        if (p.start_time == -1)
        {
            p.start_time = current_time;
        }

        p.completion_time = current_time + p.burst_time;
        p.turnaround_time = p.completion_time - p.arrival_time;
        p.waiting_time = p.turnaround_time - p.burst_time;
        p.response_time = p.start_time - p.arrival_time;

        // Add to Gantt chart
        gantt_chart.push_back({p.id, current_time, p.completion_time});

        current_time = p.completion_time;
        is_completed[highest_priority_idx] = true;
        completed++;
    }

    return processes;
}

// 5. Preemptive Priority Scheduling Algorithm
vector<Process> preemptive_priority(vector<Process> processes, vector<TimeSlice> &gantt_chart)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    int n = processes.size();
    int completed = 0;
    int current_time = 0;
    int prev_process_id = -1;
    vector<bool> is_completed(n, false);

    while (completed != n)
    {
        int highest_priority_idx = -1;
        int highest_priority = numeric_limits<int>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && processes[i].arrival_time <= current_time)
            {
                if (processes[i].priority < highest_priority)
                {
                    highest_priority = processes[i].priority;
                    highest_priority_idx = i;
                }
            }
        }

        // If no process is available at current time
        if (highest_priority_idx == -1)
        {
            // Find the next arriving process
            int next_arrival_time = numeric_limits<int>::max();

            for (int i = 0; i < n; i++)
            {
                if (!is_completed[i] && processes[i].arrival_time < next_arrival_time)
                {
                    next_arrival_time = processes[i].arrival_time;
                }
            }

            // Add idle time to Gantt chart
            if (current_time < next_arrival_time && prev_process_id != 0)
            {
                gantt_chart.push_back({0, current_time, next_arrival_time}); // Process ID 0 represents idle time
                prev_process_id = 0;
            }

            current_time = next_arrival_time;
            continue;
        }

        auto &p = processes[highest_priority_idx];

        if (p.start_time == -1)
        {
            p.start_time = current_time;
        }

        // Determine how long this process will run
        int run_time = 1; // Run for 1 time unit by default

        // Check if a new process will arrive before this process completes
        bool will_be_preempted = false;
        int next_event_time = numeric_limits<int>::max();

        // Check for arrival of higher priority process
        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && processes[i].arrival_time > current_time &&
                processes[i].arrival_time < current_time + p.remaining_time)
            {
                if (processes[i].priority < p.priority && processes[i].arrival_time < next_event_time)
                {
                    next_event_time = processes[i].arrival_time;
                    will_be_preempted = true;
                }
            }
        }

        if (will_be_preempted)
        {
            run_time = next_event_time - current_time;
        }
        else
        {
            run_time = p.remaining_time;
        }

        // Update the Gantt chart
        if (prev_process_id != p.id)
        {
            gantt_chart.push_back({p.id, current_time, current_time + run_time});
            prev_process_id = p.id;
        }
        else
        {
            // Update the end time of the last slice
            gantt_chart.back().end_time = current_time + run_time;
        }

        p.remaining_time -= run_time;
        current_time += run_time;

        // Check if the process is completed
        if (p.remaining_time == 0)
        {
            p.completion_time = current_time;
            p.turnaround_time = p.completion_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            is_completed[highest_priority_idx] = true;
            completed++;
        }
    }

    return processes;
}

// 6. Round Robin Scheduling Algorithm
vector<Process> round_robin(vector<Process> processes, vector<TimeSlice> &gantt_chart, int time_quantum)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    int n = processes.size();
    int current_time = 0;
    int completed = 0;

    // Sort processes by arrival time initially
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrival_time < b.arrival_time; });

    queue<int> ready_queue;
    vector<bool> in_queue(n, false);
    vector<bool> is_completed(n, false);

    // Initialize with first process that arrives at time 0
    for (int i = 0; i < n; i++)
    {
        if (processes[i].arrival_time == 0)
        {
            ready_queue.push(i);
            in_queue[i] = true;
        }
    }

    while (completed != n)
    {
        // If ready queue is empty, find the next arriving process
        if (ready_queue.empty())
        {
            int next_arrival_time = numeric_limits<int>::max();
            int next_process_idx = -1;

            for (int i = 0; i < n; i++)
            {
                if (!is_completed[i] && processes[i].arrival_time < next_arrival_time && !in_queue[i])
                {
                    next_arrival_time = processes[i].arrival_time;
                    next_process_idx = i;
                }
            }

            // Add idle time to Gantt chart
            if (current_time < next_arrival_time)
            {
                gantt_chart.push_back({0, current_time, next_arrival_time}); // Process ID 0 represents idle time
            }

            current_time = next_arrival_time;
            ready_queue.push(next_process_idx);
            in_queue[next_process_idx] = true;
        }

        // Get the process at the front of the queue
        int idx = ready_queue.front();
        ready_queue.pop();
        in_queue[idx] = false;

        auto &p = processes[idx];

        if (p.start_time == -1)
        {
            p.start_time = current_time;
        }

        // Calculate execution time for this time slice
        int execution_time = min(time_quantum, p.remaining_time);

        // Update Gantt chart
        gantt_chart.push_back({p.id, current_time, current_time + execution_time});

        // Update process and system state
        p.remaining_time -= execution_time;
        current_time += execution_time;

        // Check for new arrivals during this time slice
        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && !in_queue[i] && processes[i].arrival_time <= current_time &&
                processes[i].arrival_time > current_time - execution_time)
            {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        // If the process is not completed, add it back to the queue
        if (p.remaining_time > 0)
        {
            ready_queue.push(idx);
            in_queue[idx] = true;
        }
        // If the process is completed, update its completion metrics
        else
        {
            p.completion_time = current_time;
            p.turnaround_time = p.completion_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            p.response_time = p.start_time - p.arrival_time;
            is_completed[idx] = true;
            completed++;
        }
    }

    return processes;
}

// 7. Novel Scheduling Algorithm: Adaptive Priority with Dynamic Time Quantum
// This algorithm combines priority and burst time considerations with a dynamic time quantum
vector<Process> adaptive_priority(vector<Process> processes, vector<TimeSlice> &gantt_chart)
{
    gantt_chart.clear();
    processes = prepareProcesses(processes);

    // Calculate the value ratio for each process (lower priority + shorter burst = higher value)
    for (auto &p : processes)
    {
        // Lower priority number means higher priority
        // Normalize priority and burst time to avoid division by zero issues
        float normalized_priority = p.priority + 1;
        float normalized_burst = p.burst_time + 1;

        // Value ratio calculation: processes with higher priority (lower number) and shorter burst get lower ratio
        p.value_ratio = normalized_priority / normalized_burst;
    }

    int n = processes.size();
    int current_time = 0;
    int completed = 0;
    int prev_process_id = -1;
    vector<bool> is_completed(n, false);

    // Base time quantum
    int base_time_quantum = 2;

    while (completed != n)
    {
        // Find the best process to execute based on value ratio and arrival time
        int best_process_idx = -1;
        float best_value = numeric_limits<float>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && processes[i].arrival_time <= current_time)
            {
                // Lower value ratio means higher priority
                if (processes[i].value_ratio < best_value)
                {
                    best_value = processes[i].value_ratio;
                    best_process_idx = i;
                }
            }
        }

        // If no process is available at current time
        if (best_process_idx == -1)
        {
            // Find the next arriving process
            int next_arrival_time = numeric_limits<int>::max();

            for (int i = 0; i < n; i++)
            {
                if (!is_completed[i] && processes[i].arrival_time < next_arrival_time)
                {
                    next_arrival_time = processes[i].arrival_time;
                }
            }

            // Add idle time to Gantt chart
            if (current_time < next_arrival_time && prev_process_id != 0)
            {
                gantt_chart.push_back({0, current_time, next_arrival_time}); // Process ID 0 represents idle time
                prev_process_id = 0;
            }

            current_time = next_arrival_time;
            continue;
        }

        auto &p = processes[best_process_idx];

        if (p.start_time == -1)
        {
            p.start_time = current_time;
        }

        // Calculate dynamic time quantum based on priority and remaining time
        // Higher priority processes get longer time quantum
        int priority_factor = max(1, 10 - p.priority); // Range 1-10
        int dynamic_time_quantum = base_time_quantum * priority_factor / 5;

        // Ensure the time quantum is at least 1
        dynamic_time_quantum = max(1, dynamic_time_quantum);

        // Determine how long this process will run
        int run_time = min(dynamic_time_quantum, p.remaining_time);

        // Check if a new process with better value will arrive during this time quantum
        bool will_be_preempted = false;
        int next_event_time = numeric_limits<int>::max();

        for (int i = 0; i < n; i++)
        {
            if (!is_completed[i] && i != best_process_idx &&
                processes[i].arrival_time > current_time &&
                processes[i].arrival_time < current_time + run_time)
            {

                // If the arriving process has a better value ratio
                if (processes[i].value_ratio < p.value_ratio &&
                    processes[i].arrival_time < next_event_time)
                {
                    next_event_time = processes[i].arrival_time;
                    will_be_preempted = true;
                }
            }
        }

        if (will_be_preempted)
        {
            run_time = next_event_time - current_time;
        }

        // Update the Gantt chart
        if (prev_process_id != p.id)
        {
            gantt_chart.push_back({p.id, current_time, current_time + run_time});
            prev_process_id = p.id;
        }
        else
        {
            // Update the end time of the last slice
            gantt_chart.back().end_time = current_time + run_time;
        }

        p.remaining_time -= run_time;
        current_time += run_time;

        // Check if the process is completed
        if (p.remaining_time == 0)
        {
            p.completion_time = current_time;
            p.turnaround_time = p.completion_time - p.arrival_time;
            p.waiting_time = p.turnaround_time - p.burst_time;
            p.response_time = p.start_time - p.arrival_time;
            is_completed[best_process_idx] = true;
            completed++;
        }

        // If the current process is not yet complete but its time quantum expired,
        // it will be considered in the next scheduling decision
    }

    return processes;
}

// Function to compare all algorithms
void compareAllAlgorithms(vector<Process> &original_processes, int time_quantum)
{
    vector<TimeSlice> gantt_chart;
    vector<Process> result;

    // Define a structure to hold algorithm results
    struct AlgorithmResult
    {
        string name;
        double avg_response;
        double avg_waiting;
        double avg_turnaround;
    };

    vector<AlgorithmResult> results;

    // 1. FCFS
    result = fcfs(original_processes, gantt_chart);
    double avg_response, avg_waiting, avg_turnaround;
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"FCFS", avg_response, avg_waiting, avg_turnaround});

    // 2. Non-Preemptive SJF
    result = non_preemptive_sjf(original_processes, gantt_chart);
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"Non-Preemptive-SJF", avg_response, avg_waiting, avg_turnaround});

    // 3. Preemptive SJF
    result = preemptive_sjf(original_processes, gantt_chart);
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"Preemptive-SJF", avg_response, avg_waiting, avg_turnaround});

    // 4. Non-Preemptive Priority
    result = non_preemptive_priority(original_processes, gantt_chart);
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"Non-Preemptive-Priority", avg_response, avg_waiting, avg_turnaround});

    // 5. Preemptive Priority
    result = preemptive_priority(original_processes, gantt_chart);
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"Preemptive-Priority", avg_response, avg_waiting, avg_turnaround});

    // 6. Round Robin
    result = round_robin(original_processes, gantt_chart, time_quantum);
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"Round-Robin", avg_response, avg_waiting, avg_turnaround});

    // 7. Adaptive Priority (Novel Algorithm)
    result = adaptive_priority(original_processes, gantt_chart);
    getAlgorithmMetrics(result, avg_response, avg_waiting, avg_turnaround);
    results.push_back({"Adaptive-Priority", avg_response, avg_waiting, avg_turnaround});

    // Print comparison results
    cout << "\nComparison of All Algorithms:" << endl;
    cout << left << setw(30) << "Algorithm"
         << setw(20) << "Avg Response Time"
         << setw(20) << "Avg Waiting Time"
         << setw(20) << "Avg Turnaround Time" << endl;
    cout << string(90, '-') << endl;

    for (const auto &result : results)
    {
        cout << left << setw(30) << result.name
             << setw(20) << fixed << setprecision(2) << result.avg_response
             << setw(20) << fixed << setprecision(2) << result.avg_waiting
             << setw(20) << fixed << setprecision(2) << result.avg_turnaround << endl;
    }
}

// Function to get process input from user
vector<Process> getProcessInput(bool needsPriority)
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

        if (needsPriority)
        {
            cout << "Enter the priority of P" << i + 1 << " (lower number means higher priority): ";
            cin >> processes[i].priority;
        }
        else
        {
            processes[i].priority = 0; // Default priority
        }

        processes[i].remaining_time = processes[i].burst_time;
        processes[i].start_time = -1;
        processes[i].completion_time = 0;
        processes[i].response_time = 0;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
    }

    return processes;
}

int main()
{
    int choice;
    vector<Process> processes;
    vector<TimeSlice> gantt_chart;
    int time_quantum;

    while (true)
    {
        cout << "\n1: FCFS" << endl;
        cout << "2: Non-Preemptive-SJF" << endl;
        cout << "3: Preemptive-SJF" << endl;
        cout << "4: Non-Preemptive-Priority" << endl;
        cout << "5: Preemptive-Priority" << endl;
        cout << "6: Round-Robin" << endl;
        cout << "7: Adaptive-Priority (Novel Algorithm)" << endl;
        cout << "8: Compare-All" << endl;
        cout << "9: Exit" << endl;
        cout << "Input your Choice: ";
        cin >> choice;

        if (choice == 9)
        {
            cout << "Exiting program." << endl;
            break;
        }

        switch (choice)
        {
        case 1:
        { // FCFS
            processes = getProcessInput(false);
            processes = fcfs(processes, gantt_chart);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 2:
        { // Non-Preemptive SJF
            processes = getProcessInput(false);
            processes = non_preemptive_sjf(processes, gantt_chart);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 3:
        { // Preemptive SJF
            processes = getProcessInput(false);
            processes = preemptive_sjf(processes, gantt_chart);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 4:
        { // Non-Preemptive Priority
            processes = getProcessInput(true);
            processes = non_preemptive_priority(processes, gantt_chart);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 5:
        { // Preemptive Priority
            processes = getProcessInput(true);
            processes = preemptive_priority(processes, gantt_chart);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 6:
        { // Round Robin
            processes = getProcessInput(false);
            cout << "Time Quantum: ";
            cin >> time_quantum;
            processes = round_robin(processes, gantt_chart, time_quantum);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 7:
        { // Adaptive Priority (Novel Algorithm)
            processes = getProcessInput(true);
            cout << "Note: This algorithm uses a dynamic time quantum based on process priority." << endl;
            processes = adaptive_priority(processes, gantt_chart);
            printGanttChart(gantt_chart);
            printProcessDetails(processes);
            calculateAverages(processes);
            break;
        }
        case 8:
        { // Compare All
            processes = getProcessInput(true);
            cout << "Time Quantum for Round Robin: ";
            cin >> time_quantum;
            compareAllAlgorithms(processes, time_quantum);
            break;
        }
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}