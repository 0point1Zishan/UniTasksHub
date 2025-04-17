#!/bin/bash

echo "===================================="
echo "       MultiAlgo-CPU-Scheduler      "
echo "===================================="
echo

# Function to display menu
display_menu() {
    echo "Select a scheduling algorithm:"
    echo "1. First-Come, First-Served (FCFS)"
    echo "2. Shortest Job First (SJF)"
    echo "3. Round Robin (RR)"
    echo "4. Exit"
    echo
    read -p "Enter your choice (1-4): " choice
}

# Function to get process details from user
get_processes() {
    echo
    read -p "Enter number of processes: " nPro
    
    declare -a process_ids
    declare -a arrival_times
    declare -a burst_times
    
    echo
    echo "Enter process details:"
    for ((i=0; i<nPro; i++)); do
        process_ids[$i]=$((i+1))
        read -p "Process P$((i+1)) - Arrival time: " arrival_times[$i]
        read -p "Process P$((i+1)) - Burst time: " burst_times[$i]
        echo
    done
    
    # Return values through global variables
    PROCESS_IDS=("${process_ids[@]}")
    ARRIVAL_TIMES=("${arrival_times[@]}")
    BURST_TIMES=("${burst_times[@]}")
    N_PROCESSES=$nPro
}

# Function to display a Gantt chart
display_gantt_chart() {
    local chart=("$@")
    local length=${#chart[@]}
    
    echo
    echo "Gantt Chart:"
    echo "============"
    
    # Print top border
    echo -n "┌"
    for ((i=0; i<length; i++)); do
        for ((j=0; j<10; j++)); do
            echo -n "─"
        done
        if (( i < length - 1 )); then
            echo -n "┬"
        fi
    done
    echo "┐"
    
    # Print process IDs
    for ((i=0; i<length; i++)); do
        local pad=$((5 - ${#chart[$i]} / 2))
        echo -n "│"
        for ((j=0; j<pad; j++)); do
            echo -n " "
        done
        echo -n "${chart[$i]}"
        for ((j=0; j<(10-pad-${#chart[$i]}); j++)); do
            echo -n " "
        done
    done
    echo "│"
    
    # Print bottom border
    echo -n "└"
    for ((i=0; i<length; i++)); do
        for ((j=0; j<10; j++)); do
            echo -n "─"
        done
        if (( i < length - 1 )); then
            echo -n "┴"
        fi
    done
    echo "┘"
    
    # Print timeline
    echo -n "0"
    for ((i=0; i<length; i++)); do
        local time_label="${GANTT_TIMES[$i]}"
        local pad=$((10 - ${#time_label}))
        for ((j=0; j<pad; j++)); do
            echo -n " "
        done
        echo -n "$time_label"
    done
    echo
}


# Function to display queue state
display_queue_state() {
    local queue_name=$1
    local queue=("${!2}")
    
    echo
    echo "$queue_name:"
    echo "$(printf '=%.0s' $(seq 1 ${#queue_name}))"
    
    if [ ${#queue[@]} -eq 0 ]; then
        echo "[Empty]"
    else
        echo -n "["
        for ((i=0; i<${#queue[@]}; i++)); do
            echo -n "P${queue[$i]}"
            if (( i < ${#queue[@]} - 1 )); then
                echo -n ", "
            fi
        done
        echo "]"
    fi
}

# Function to implement FCFS algorithm
fcfs_scheduling() {
    local n=$1
    local process_ids=("${!2}")
    local arrival_times=("${!3}")
    local burst_times=("${!4}")
    
    # Arrays to store calculated times
    declare -a completion_times
    declare -a waiting_times
    declare -a turnaround_times
    declare -a gantt_chart
    declare -a gantt_times
    declare -a ready_queue
    declare -a job_queue
    declare -a process_states
    declare -a execution_history
    
    # Sort Arrival Time, Bust Time and Process Time
    for ((i=0; i<n-1; i++)); do
        for ((j=0; j<n-i-1; j++)); do
            if (( $(echo "${arrival_times[$j]} > ${arrival_times[$j+1]}" | bc -l) )); then
                # Swap arrival times
                temp=${arrival_times[$j]}
                arrival_times[$j]=${arrival_times[$j+1]}
                arrival_times[$j+1]=$temp
                
                # Swap burst times
                temp=${burst_times[$j]}
                burst_times[$j]=${burst_times[$j+1]}
                burst_times[$j+1]=$temp
                
                # Swap process IDs
                temp=${process_ids[$j]}
                process_ids[$j]=${process_ids[$j+1]}
                process_ids[$j+1]=$temp
            fi
        done
    done
    
    for ((i=0; i<n; i++)); do
        process_states[$i]=0
    done
    
    # Find earliest arrival time
    local min_arrival=${arrival_times[0]}
    local current_time=$min_arrival
    
    # Add first timestamp to Gantt chart
    gantt_times[0]=$current_time
    
    # Simulation loop
    local completed=0
    local current_process=-1
    
    echo
    echo "FCFS Scheduling Simulation:"
    echo "==========================="
    
    while (( completed < n )); do
        # Update job queue and ready queue based on arrival times
        job_queue=()
        ready_queue=()
        
        for ((i=0; i<n; i++)); do
            if (( arrival_times[i] <= current_time && process_states[i] < 4 )); then
                if (( process_states[i] == 0 )); then
                    process_states[$i]=1 # Move to job queue
                fi
                
                if (( process_states[i] == 1 )); then
                    job_queue+=(${process_ids[$i]})
                    process_states[$i]=2 # Move to ready queue
                fi
                
                if (( process_states[i] == 2 && i != current_process )); then
                    ready_queue+=(${process_ids[$i]})
                fi
            fi
        done
        
        # Display queue states
        echo
        echo "At time $current_time:"
        display_queue_state "Job Queue" job_queue[@]
        display_queue_state "Ready Queue" ready_queue[@]
        
        # If no process is running, select next one from ready queue (FCFS)
        if (( current_process == -1 )); then
            for ((i=0; i<n; i++)); do
                if (( process_states[i] == 2 )); then
                    current_process=$i
                    process_states[$i]=3 # Mark as running
                    
                    # Display which process starts running
                    echo "Process P${process_ids[$i]} starts execution"
                    
                    # Update Gantt chart
                    gantt_chart+=(P${process_ids[$i]})
                    gantt_times+=($(( current_time + burst_times[i] )))
                    
                    break
                fi
            done
        fi
        
        # If a process is running, advance time to its completion
        if (( current_process != -1 )); then
            # Complete current process
            completion_times[$current_process]=$(( current_time + burst_times[current_process] ))
            current_time=${completion_times[$current_process]}

            
            # Record that this process is complete
            echo "Process P${process_ids[$current_process]} completes at time $current_time"
            process_states[$current_process]=4
            completed=$(( completed + 1 ))
            
            # Reset current process
            current_process=-1
        else
            # No process available to run, advance time to next arrival
            local next_arrival=999999
            for ((i=0; i<n; i++)); do
                if (( process_states[i] == 0 && arrival_times[i] < next_arrival )); then
                    next_arrival=${arrival_times[$i]}
                fi
            done
            
            if (( next_arrival < 999999 )); then
                echo "CPU idle until time $next_arrival"
                gantt_chart+=("IDLE")
                gantt_times+=(${next_arrival})
                current_time=$next_arrival
            else 
                # Something went wrong, break loop
                echo "Error: No more processes to schedule but not all completed."
                break
            fi
        fi
    done
    
    # Calculate turnaround and waiting times
    local total_waiting_time=0
    local total_turnaround_time=0
    
    for ((i=0; i<n; i++)); do
        turnaround_times[$i]=$(( completion_times[i] - arrival_times[i] ))
        waiting_times[$i]=$(( turnaround_times[i] - burst_times[i] ))
        total_waiting_time=$(( total_waiting_time + waiting_times[i] ))
        total_turnaround_time=$(( total_turnaround_time + turnaround_times[i] ))
    done
    
    # Calculate total execution time
    local total_execution_time=$(( gantt_times[${#gantt_times[@]}-1] - gantt_times[0] ))
    
    # Display Gantt chart
    GANTT_TIMES=("${gantt_times[@]}")
    display_gantt_chart "${gantt_chart[@]}"
    
    # Display results table
    echo
    echo "FCFS Scheduling Results:"
    echo "========================"
    echo
    printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time" "Completion Time" "Turnaround Time" "Waiting Time"
    echo "-----------------------------------------------------------------------------------------"
    
    for ((i=0; i<n; i++)); do
        printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "P${process_ids[$i]}" "${arrival_times[$i]}" "${burst_times[$i]}" "${completion_times[$i]}" "${turnaround_times[$i]}" "${waiting_times[$i]}"
    done
    
    echo
    echo "Performance Metrics:"
    echo "===================="
    echo "Throughput: $(echo "scale=2; $n / $total_execution_time" | bc) processes per unit time"
    echo "Average Waiting Time: $(echo "scale=2; $total_waiting_time / $n" | bc) time units"
    echo "Average Turnaround Time: $(echo "scale=2; $total_turnaround_time / $n" | bc) time units"
    echo "Total Execution Time: $total_execution_time time units"
}

# Function to implement SJF algorithm (non-preemptive)
sjf_scheduling() {
    local n=$1
    local process_ids=("${!2}")
    local arrival_times=("${!3}")
    local burst_times=("${!4}")
    
    # Arrays to store calculated times
    declare -a completion_times
    declare -a waiting_times
    declare -a turnaround_times
    declare -a gantt_chart
    declare -a gantt_times
    declare -a ready_queue
    declare -a job_queue
    declare -a process_states
    declare -a remaining_times
    
    # Initialize process states and remaining times
    for ((i=0; i<n; i++)); do
        process_states[$i]=0  # 0=not arrived, 1=in job queue, 2=in ready queue, 3=running, 4=completed
        remaining_times[$i]=${burst_times[$i]}
    done
    
    # Find earliest arrival time
    local min_arrival=${arrival_times[0]}
    for ((i=1; i<n; i++)); do
        if (( arrival_times[i] < min_arrival )); then
            min_arrival=${arrival_times[$i]}
        fi
    done
    
    local current_time=$min_arrival
    gantt_times[0]=$current_time
    
    echo
    echo "SJF Scheduling Simulation:"
    echo "=========================="
    
    local completed=0
    local current_process=-1
    
    while (( completed < n )); do
        # Update job queue and ready queue based on arrival times
        job_queue=()
        ready_queue=()
        
        for ((i=0; i<n; i++)); do
            if (( arrival_times[i] <= current_time && process_states[i] < 4 )); then
                if (( process_states[i] == 0 )); then
                    process_states[$i]=1  # Move to job queue
                fi
                
                if (( process_states[i] == 1 )); then
                    job_queue+=(${process_ids[$i]})
                    process_states[$i]=2  # Move to ready queue
                fi
                
                if (( process_states[i] == 2 && i != current_process )); then
                    ready_queue+=(${process_ids[$i]})
                fi
            fi
        done
        
        # Display queue states
        echo
        echo "At time $current_time:"
        display_queue_state "Job Queue" job_queue[@]
        display_queue_state "Ready Queue" ready_queue[@]
        
        # If no process is running, select shortest job from ready queue
        if (( current_process == -1 )); then
            local shortest_job=-1
            local min_burst=999999
            
            for ((i=0; i<n; i++)); do
                if (( process_states[i] == 2 && remaining_times[i] < min_burst )); then
                    min_burst=${remaining_times[$i]}
                    shortest_job=$i
                fi
            done
            
            if (( shortest_job != -1 )); then
                current_process=$shortest_job
                process_states[$current_process]=3  # Mark as running
                
                # Display which process starts running
                echo "Process P${process_ids[$current_process]} starts execution"
                
                # Update Gantt chart
                gantt_chart+=(P${process_ids[$current_process]})
                gantt_times+=($(( current_time + remaining_times[current_process] )))
            fi
        fi
        
        # If a process is running, advance time to its completion
        if (( current_process != -1 )); then
            # Complete current process
            completion_times[$current_process]=$(( current_time + remaining_times[current_process] ))
            current_time=${completion_times[$current_process]}
            remaining_times[$current_process]=0
            
            # Record that this process is complete
            echo "Process P${process_ids[$current_process]} completes at time $current_time"
            process_states[$current_process]=4  # Mark as completed
            completed=$(( completed + 1 ))
            
            # Reset current process
            current_process=-1
        else 
            # No process available to run, advance time to next arrival
            local next_arrival=999999
            for ((i=0; i<n; i++)); do
                if (( process_states[i] == 0 && arrival_times[i] < next_arrival )); then
                    next_arrival=${arrival_times[$i]}
                fi
            done
            
            if (( next_arrival < 999999 )); then
                echo "CPU idle until time $next_arrival"
                gantt_chart+=("IDLE")
                gantt_times+=(${next_arrival})
                current_time=$next_arrival
            else 
                # Something went wrong, break loop
                echo "Error: No more processes to schedule but not all completed."
                break
            fi
            
        fi
    done
    
    # Calculate turnaround and waiting times
    local total_waiting_time=0
    local total_turnaround_time=0
    
    for ((i=0; i<n; i++)); do
        turnaround_times[$i]=$(( completion_times[i] - arrival_times[i] ))
        waiting_times[$i]=$(( turnaround_times[i] - burst_times[i] ))
        total_waiting_time=$(( total_waiting_time + waiting_times[i] ))
        total_turnaround_time=$(( total_turnaround_time + turnaround_times[i] ))
    done
    
    # Calculate total execution time
    local total_execution_time=$(( gantt_times[${#gantt_times[@]}-1] - gantt_times[0] ))
    
    # Display Gantt chart
    GANTT_TIMES=("${gantt_times[@]}")
    display_gantt_chart "${gantt_chart[@]}"
    
    # Display results table
    echo
    echo "SJF Scheduling Results:"
    echo "======================="
    echo
    printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time" "Completion Time" "Turnaround Time" "Waiting Time"
    echo "-----------------------------------------------------------------------------------------"
    
    for ((i=0; i<n; i++)); do
        printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "P${process_ids[$i]}" "${arrival_times[$i]}" "${burst_times[$i]}" "${completion_times[$i]}" "${turnaround_times[$i]}" "${waiting_times[$i]}"
    done
    
    echo
    echo "Performance Metrics:"
    echo "===================="
    echo "Throughput: $(echo "scale=2; $n / $total_execution_time" | bc) processes per unit time"
    echo "Average Waiting Time: $(echo "scale=2; $total_waiting_time / $n" | bc) time units"
    echo "Average Turnaround Time: $(echo "scale=2; $total_turnaround_time / $n" | bc) time units"
    echo "Total Execution Time: $total_execution_time time units"
}

# Function to implement Round Robin algorithm with enhanced visuals
round_robin_scheduling() {
    if ((nPro == 0)); then
        return
    fi
    
    local n=$1
    local process_ids=("${!2}")
    local arrival_times=("${!3}")
    local burst_times=("${!4}")
    
    # Get time quantum for Round Robin
    echo
    read -p "Enter time quantum for Round Robin: " time_quantum
    
    # Arrays to store calculated times
    declare -a completion_times
    declare -a waiting_times
    declare -a turnaround_times
    declare -a remaining_times
    declare -a gantt_chart
    declare -a gantt_times
    declare -a ready_queue
    declare -a job_queue
    declare -a process_states
    declare -a last_execution
    
    # Initialize remaining times and process states
    for ((i=0; i<n; i++)); do
        completion_times[$i]=0
        remaining_times[$i]=${burst_times[$i]}
        process_states[$i]=0  # 0=not arrived, 1=in job queue, 2=in ready queue, 3=running, 4=completed
        last_execution[$i]=0
    done
    
    # Find earliest arrival time
    local min_arrival=${arrival_times[0]}
    for ((i=1; i<n; i++)); do
        if (( arrival_times[i] < min_arrival )); then
            min_arrival=${arrival_times[$i]}
        fi
    done
    
    local current_time=$min_arrival
    gantt_times[0]=$current_time
    
    # Display initial process table
    echo
    echo "======================="
    echo "Process Table:"
    echo "======================="
    printf "%-10s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time"
    echo "--------------------------------------"
    
    for ((i=0; i<n; i++)); do
        printf "%-10s %-15s %-15s\n" "P${process_ids[$i]}" "${arrival_times[$i]}" "${burst_times[$i]}"
    done
    
    echo
    echo "=========================================================="
    echo "Starting Round Robin (Quantum=$time_quantum) Simulation..."
    echo "=========================================================="
    
    local completed=0
    
    while (( completed < n )); do
        echo
        echo "=============================="
        echo "Time: $current_time"
        echo "=============================="
        
        # Update job queue and ready queue based on arrival times
        job_queue=()
        ready_queue=()
        remaining_queue=()
        
        for ((i=0; i<n; i++)); do
            if (( arrival_times[i] <= current_time && process_states[i] < 4 )); then
                if (( process_states[i] == 0 )); then
                    process_states[$i]=1  # Move to job queue
                fi
                
                if (( process_states[i] == 1 )); then
                    job_queue+=(${process_ids[$i]})
                    process_states[$i]=2  # Move to ready queue
                fi
                
                if (( process_states[i] == 2 && remaining_times[i] > 0 )); then
                    ready_queue+=(${process_ids[$i]})
                    remaining_queue+=(${remaining_times[$i]})
                fi
            fi
        done
        
        # Display visual queue states
        display_visual_queue "Job Queue" job_queue[@] remaining_times[@] $current_time
        display_visual_queue "Ready Queue" ready_queue[@] remaining_times[@] $current_time
        
        # If ready queue is empty, advance time to next arrival
        if [ ${#ready_queue[@]} -eq 0 ]; then
            local next_arrival=999999
            for ((i=0; i<n; i++)); do
                if (( process_states[i] < 2 && arrival_times[i] < next_arrival )); then
                    next_arrival=${arrival_times[$i]}
                fi
            done
            
            if (( next_arrival < 999999 && next_arrival > current_time )); then
                echo
                echo "** CPU idle until time $next_arrival **"
                gantt_chart+=("IDLE")
                gantt_times+=(${next_arrival})
                current_time=$next_arrival
                continue
            elif (( completed < n )); then
                # Something is wrong, we should have found a process
                echo
                echo "!! Error: No more processes to schedule but not all completed !!"
                break
            fi
        fi
        
        # Create a queue for this round robin cycle
        local round_robin_queue=()
        for proc_id in "${ready_queue[@]}"; do
            for ((i=0; i<n; i++)); do
                if (( process_ids[i] == proc_id )); then
                    round_robin_queue+=($i)  # Store indices
                    break
                fi
            done
        done
        
        # Process each ready process according to RR
        if [ ${#round_robin_queue[@]} -gt 0 ]; then
            local current_idx=${round_robin_queue[0]}
            
            echo
            echo ">> Process P${process_ids[$current_idx]} gets CPU"
            
            local execution_time=$time_quantum
            if (( remaining_times[current_idx] < time_quantum )); then
                execution_time=${remaining_times[$current_idx]}
            fi
            
            # Visualize process execution
            echo
            echo "   Executing P${process_ids[$current_idx]}: ["
            for ((i=0; i<10; i++)); do
                echo -n "■"
            done
            echo "] ($execution_time time units)"
            
            # Update Gantt chart
            gantt_chart+=(P${process_ids[$current_idx]})
            current_time=$(( current_time + execution_time ))
            gantt_times+=(${current_time})
            
            # Update remaining time
            remaining_times[$current_idx]=$(( remaining_times[current_idx] - execution_time ))
            last_execution[$current_idx]=$current_time
            
            # Check if process is completed
            if (( remaining_times[$current_idx] <= 0 )); then
                echo
                echo "✓✓ Process P${process_ids[$current_idx]} COMPLETES at time $current_time"
                completion_times[$current_idx]=$current_time
                process_states[$current_idx]=4
                completed=$(( completed + 1 ))
            else 
                echo
                echo ">> Process P${process_ids[$current_idx]} PREEMPTED at time $current_time"
                echo "   Remaining time: ${remaining_times[$current_idx]} units"
                
                # Move this process to end of ready queue
                process_states[$current_idx]=2
            fi
        fi
    done
    
    # Calculate turnaround and waiting times
    local total_waiting_time=0
    local total_turnaround_time=0
    
    for ((i=0; i<n; i++)); do
        turnaround_times[$i]=$(( completion_times[i] - arrival_times[i] ))
        waiting_times[$i]=$(( turnaround_times[i] - burst_times[i] ))
        total_waiting_time=$(( total_waiting_time + waiting_times[i] ))
        total_turnaround_time=$(( total_turnaround_time + turnaround_times[i] ))
    done
    
    # Calculate total execution time
    local total_execution_time=$(( gantt_times[${#gantt_times[@]}-1] - gantt_times[0] ))
    
    # Display Gantt chart
    GANTT_TIMES=("${gantt_times[@]}")
    display_gantt_chart "${gantt_chart[@]}"
    
    # Display results table
    echo
    echo "============================================================"
    echo "Round Robin (Quantum=$time_quantum) Scheduling Results:"
    echo "============================================================"
    echo
    printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time" "Completion Time" "Turnaround Time" "Waiting Time"
    echo "-----------------------------------------------------------------------------------------"
    
    for ((i=0; i<n; i++)); do
        printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "P${process_ids[$i]}" "${arrival_times[$i]}" "${burst_times[$i]}" "${completion_times[$i]}" "${turnaround_times[$i]}" "${waiting_times[$i]}"
    done
    
    echo
    echo "=================="
    echo "Performance Metrics:"
    echo "=================="
    echo "→ Throughput: $(echo "scale=2; $n / $total_execution_time" | bc) processes per unit time"
    echo "→ Average Waiting Time: $(echo "scale=2; $total_waiting_time / $n" | bc) time units"
    echo "→ Average Turnaround Time: $(echo "scale=2; $total_turnaround_time / $n" | bc) time units"
    echo "→ Total Execution Time: $total_execution_time time units"
    
    # RR Explanation
    echo
    echo "==============================="
    echo "Round Robin Algorithm Explanation:"
    echo "==============================="
    echo "Round Robin (RR) is a preemptive CPU scheduling algorithm:"
    echo "  • Each process gets a small unit of CPU time (time quantum)"
    echo "  • After time quantum expires, process is preempted and moved to end of queue"
    echo "  • Ensures fairness as each process gets equal CPU time in each cycle"
    echo "  • Good for time-sharing systems and interactive processes"
    echo "  • Performance depends heavily on choice of time quantum"
    echo "  • Small quantum: better response time but more context switches"
    echo "  • Large quantum: fewer context switches but may behave like FCFS"
    echo
}

# Function to display visual queue state without colors
display_visual_queue() {
    local queue_name=$1
    local queue=("${!2}")
    local burst_remaining=("${!3}")
    local curr_time=$4
    
    echo
    echo "$queue_name:"
    echo "$(printf '=%.0s' $(seq 1 ${#queue_name}))"
    
    if [ ${#queue[@]} -eq 0 ]; then
        echo "[Empty]"
    else
        # Draw a visual queue with boxes
        echo "┌─────────────────────────────────────────┐"
        echo -n "│ "
        
        for ((i=0; i<${#queue[@]}; i++)); do
            local proc_id=${queue[$i]}
            local proc_index=0
            
            # Find process index
            for ((j=0; j<N_PROCESSES; j++)); do
                if (( PROCESS_IDS[j] == proc_id )); then
                    proc_index=$j
                    break
                fi
            done
            
            # Show process ID and remaining time if available
            if [[ "$queue_name" == *"Ready Queue"* ]] && [ -n "${burst_remaining[$proc_index]}" ]; then
                echo -n "[P$proc_id:${burst_remaining[$proc_index]}] "
            else
                echo -n "[P$proc_id] "
            fi
        done
        
        echo "│"
        echo "└─────────────────────────────────────────┘"
        
        if [[ "$queue_name" == *"Ready Queue"* ]]; then
            echo "Note: [Process:RemainingTime]"
        fi
    fi
}

# Function to display a non-colorful Gantt chart
display_gantt_chart() {
    local chart=("$@")
    local length=${#chart[@]}
    
    echo
    echo "Gantt Chart:"
    echo "==========="
    
    # Print top border
    echo -n "┌"
    for ((i=0; i<length; i++)); do
        for ((j=0; j<10; j++)); do
            echo -n "─"
        done
        if (( i < length - 1 )); then
            echo -n "┬"
        fi
    done
    echo "┐"
    
    # Print process IDs
    for ((i=0; i<length; i++)); do
        local proc_id=${chart[$i]}
        local pad=$((5 - ${#proc_id} / 2))
        echo -n "│"
        for ((j=0; j<pad; j++)); do
            echo -n " "
        done
        echo -n "${proc_id}"
        for ((j=0; j<(10-pad-${#proc_id}); j++)); do
            echo -n " "
        done
    done
    echo "│"
    
    # Print bottom border
    echo -n "└"
    for ((i=0; i<length; i++)); do
        for ((j=0; j<10; j++)); do
            echo -n "─"
        done
        if (( i < length - 1 )); then
            echo -n "┴"
        fi
    done
    echo "┘"
    
    # Print timeline
    echo -n "0"
    for ((i=0; i<length; i++)); do
        local time_label="${GANTT_TIMES[$i]}"
        local pad=$((10 - ${#time_label}))
        for ((j=0; j<pad; j++)); do
            echo -n " "
        done
        echo -n "$time_label"
    done
    echo
}

# Function to compare and suggest best algorithm based on process characteristics
compare_algorithms() {
    echo
    echo "================================================="
    echo "ALGORITHM COMPARISON AND RECOMMENDATION"
    echo "================================================="
    
    # Implement logic to compare algorithms based on current data
    local n=${#BURST_TIMES[@]}
    local all_similar=1
    local similar_arrival=1
    local first_burst=${BURST_TIMES[0]}
    local first_arrival=${ARRIVAL_TIMES[0]}
    
    # Check if all burst times are similar
    for ((i=1; i<n; i++)); do
        if (( ${BURST_TIMES[$i]} != first_burst )); then
            all_similar=0
            break
        fi
    done
    
    # Check if all arrival times are similar
    for ((i=1; i<n; i++)); do
        if (( ${ARRIVAL_TIMES[$i]} != first_arrival )); then
            similar_arrival=0
            break
        fi
    done
    
    echo
    echo "ANALYSIS OF YOUR PROCESS DATA:"
    echo "----------------------------------"
    
    if (( all_similar == 1 && similar_arrival == 1 )); then
        echo "✓ All processes have similar burst times and arrival times"
        echo
        echo "RECOMMENDATION:"
        echo "► Round Robin would be fair and provide good response times"
        echo "► FCFS would also work well in this scenario"
        echo
        echo "BEST CHOICE: Round Robin with an appropriate time quantum"
    elif (( all_similar == 1 && similar_arrival == 0 )); then
        echo "✓ All processes have similar burst times but different arrival times"
        echo
        echo "RECOMMENDATION:"
        echo "► FCFS would be simple and fair in this case"
        echo
        echo "BEST CHOICE: FCFS (First-Come, First-Served)"
    elif (( all_similar == 0 && similar_arrival == 1 )); then
        echo "✓ Processes have different burst times but arrive at similar times"
        echo
        echo "RECOMMENDATION:"
        echo "► SJF would minimize average waiting time"
        echo
        echo "BEST CHOICE: SJF (Shortest Job First)"
    else
        # Find variance in burst times
        local sum=0
        local sum_sq=0
        
        for ((i=0; i<n; i++)); do
            sum=$((sum + ${BURST_TIMES[$i]}))
            sum_sq=$((sum_sq + ${BURST_TIMES[$i]}*${BURST_TIMES[$i]}))
        done
        
        local mean=$((sum / n))
        local variance=$((sum_sq/n - mean*mean))
        
        echo "✓ Processes have varied burst times and arrival patterns"
        echo "✓ Burst time variance: $variance"
        echo
        
        if (( variance > 20 )); then
            echo "RECOMMENDATION:"
            echo "► High variability in process burst times"
            echo "► SJF would minimize average waiting time but might cause starvation"
            echo "► Round Robin ensures fairness and prevents starvation"
            echo
            echo "BEST CHOICE: Round Robin with a carefully chosen time quantum"
        else
            echo "RECOMMENDATION:"
            echo "► Moderate variability in process burst times"
            echo "► SJF offers good average waiting time without significant starvation risk"
            echo
            echo "BEST CHOICE: SJF for best overall performance"
        fi
    fi
    
    echo
    echo "================================================="
    echo "NOTE: This recommendation is based on theoretical"
    echo "performance characteristics of each algorithm and"
    echo  specific pattern of your process data."
    echo "================================================="
}

# Function to display about section with ASCII art
display_about() {
    clear
    echo "================================================="
    echo "          MultiAlgo-CPU-Scheduler v1.0"
    echo "================================================="
    echo
    echo "DEVELOPED BY:"
    echo "-------------------------------------------------"
    echo "1. Md. Zishan Sarkar Murad - 0242220005101632"
    echo "2. Md. Fahim Shahriar Sakib - 0242220005101635"
    echo
    echo "FEATURES:"
    echo "-------------------------------------------------"
    echo "• Multiple CPU scheduling algorithm implementations"
    echo "• Visual process queue representation"
    echo "• Detailed Gantt charts"
    echo "• Performance metrics calculation"
    echo "• Algorithm comparison and recommendation"
    echo
    echo "This project was developed as part of Operating Systems"
    echo "course requirement at Daffodil International University."
}

while true; do
    display_menu
    
    case $choice in
        1)
            get_processes
            fcfs_scheduling $N_PROCESSES PROCESS_IDS[@] ARRIVAL_TIMES[@] BURST_TIMES[@]
            compare_algorithms
            ;;
        2)
            get_processes
            sjf_scheduling $N_PROCESSES PROCESS_IDS[@] ARRIVAL_TIMES[@] BURST_TIMES[@]
            compare_algorithms
            ;;
        3)
            get_processes
            round_robin_scheduling $N_PROCESSES PROCESS_IDS[@] ARRIVAL_TIMES[@] BURST_TIMES[@]
            compare_algorithms
            ;;
        4)
            display_about
            exit 0
            ;;
        *)
            echo "Invalid choice. Please enter a number between 1 and 4."
            ;;
    esac
    
    echo
    read -p "Press Enter to continue..."
    clear
done