#!/bin/bash
echo "===================================="
echo "       MultiAlgo-CPU-Scheduler      "
echo "===================================="
echo

# Menu
menu() {
    echo "Select a scheduling algorithm:"
    echo "1. First-Come, First-Served (FCFS)"
    echo "2. Shortest Job First (SJF)"
    echo "3. Round Robin (RR)"
    echo "4. Exit"
    echo
    read -p "Enter your choice (1-4): " choice
}

# Process info
process() {
    echo
    read -p "Enter number of processes: " no_pro_local
    
    declare -a pro_id_local
    declare -a arr_time_local
    declare -a b_time_local
    
    echo
    echo "Enter process details:"
    for (( i=0; i < no_pro_local; i++ )); do

        pro_id_local[$i]=$((i+1))
        read -p "Process P$((i+1)) - Arrival time: " arr_time_local[$i]
        
        while ! [[ "${arr_time_local[$i]}" =~ ^[0-9]+$ ]]; do
            echo "Error: Arrival time must be a non-negative integer."
            read -p "Process P$((i+1)) - Arrival time: " arr_time_local[$i]
        done
        
        read -p "Process P$((i+1)) - Burst time: " b_time_local[$i]
        
        while ! [[ "${b_time_local[$i]}" =~ ^[0-9]+$ ]] || (( b_time_local[i] <= 0 )); do
            echo "Error: Burst time must be a positive integer."
            read -p "Process P$((i+1)) - Burst time: " b_time_local[$i]
        done
        
        echo
    done
    
    process_id=("${pro_id_local[@]}")
    arrival_time=("${arr_time_local[@]}")
    burst_time=("${b_time_local[@]}")
    no_pro=$no_pro_local
}

# Gantt chart
gantt_chat() {
    local chart=("$@")
    local length=${#chart[@]}
    
    if [ ${#GANTT_TIMES[@]} -lt $length ]; then
        echo "Error: GANTT_TIMES array is not properly defined."
        return 1
    fi
    
    echo
    echo "Gantt Chart:"
    echo "============"
    
    # TimeLine with border
    echo -n "┌"
    for (( i=0; i < length; i++)); do
        for (( j=0; j < 10; j++)); do
            echo -n "─"
        done

        if (( i < length - 1 )); then
            echo -n "┬"
        fi
    done
    echo "┐"
    
    for (( i=0; i < length; i++ )); do
        local process_label="${chart[$i]}"
        local label_length=${#process_label}
        local left_pad=$(( (10 - label_length) / 2 ))
        local right_pad=$(( 10 - label_length - left_pad ))
        
        echo -n "│"
        printf "%${left_pad}s%s%${right_pad}s" "" "$process_label" ""
    done
    echo "│"
    
    echo -n "└"
    for (( i=0; i < length; i++)); do
        for (( j=0; j < 10; j++)); do
            echo -n "─"
        done

        if (( i < length - 1 )); then
            echo -n "┴"
        fi
    done
    echo "┘"
    
    echo -n "${GANTT_TIMES[0]}"
    for (( i=1; i < ${#GANTT_TIMES[@]}; i++)); do
        local time_label="${GANTT_TIMES[$i]}"
        local label_length=${#time_label}
        local pad=$(( 10 - label_length + 1 ))
        
        printf "%${pad}s%s" "" "$time_label"
    done
    echo
}


# Queue state
queue_state() {
    local queue_name=$1
    local queue=("${!2}")
    
    echo
    echo "$queue_name:"
    echo "$(printf '=%.0s' $(seq 1 ${#queue_name}))"
    
    if [ ${#queue[@]} -eq 0 ]; then
        echo "[Empty]"
    else
        echo -n "["
        for (( i=0; i < ${#queue[@]}; i++ )); do

            echo -n "P${queue[$i]}"
            if (( i < ${#queue[@]} - 1 )); then
                echo -n ", "
            fi
        done
        echo "]"
    fi
}

# FCFS algorithm
fcfs_algo() {
    local n=$1
    local pro_id=("${!2}")
    local arr_time=("${!3}")
    local b_time=("${!4}")
    
    declare -a com_time
    declare -a wa_time
    declare -a ta_time
    declare -a g_chat
    declare -a gantt_time
    declare -a ready_queue
    declare -a job_queue
    declare -a pro_sta
    declare -a history
    
    for (( i=0; i < n-1; i++ )); do

        for (( j=0; j <n-i-1; j++ )); do

            if (( arr_time[j] > arr_time[j+1] )); then
                temp=${arr_time[$j]}
                arr_time[$j]=${arr_time[$j+1]}
                arr_time[$j+1]=$temp
                
                temp=${b_time[$j]}
                b_time[$j]=${b_time[$j+1]}
                b_time[$j+1]=$temp
                
                temp=${pro_id[$j]}
                pro_id[$j]=${pro_id[$j+1]}
                pro_id[$j+1]=$temp
            fi
            
        done
    done
    
    for ((i=0; i<n; i++)); do
        pro_sta[$i]=0
    done
    
    local min_arri=${arr_time[0]}
    local cur_time=$min_arri
    
    gantt_time[0]=$cur_time
    
    local completed=0
    local cur_pro=-1
    
    echo
    echo "FCFS Scheduling Simulation:"
    echo "==========================="
    
    while (( completed < n )); do

        job_queue=()
        ready_queue=()
        
        for (( i=0; i < n; i++ )); do

            if (( arr_time[i] <= cur_time && pro_sta[i] < 4 )); then

                if (( pro_sta[i] == 0 )); then
                    pro_sta[$i]=1
                fi
                
                if (( pro_sta[i] == 1 )); then
                    job_queue+=(${pro_id[$i]})
                    pro_sta[$i]=2
                fi
                
                if (( pro_sta[i] == 2 && i != cur_pro )); then
                    ready_queue+=(${pro_id[$i]})
                fi
            fi
        done
        
        echo
        echo "At time $cur_time:"
        queue_state "Job Queue" job_queue[@]
        queue_state "Ready Queue" ready_queue[@]
        
        if (( cur_pro == -1 )); then
            for (( i=0; i < n; i++ )); do
                if (( pro_sta[i] == 2 )); then
                    cur_pro=$i
                    pro_sta[$i]=3
                    
                    echo "Process P${pro_id[$i]} starts execution"
                    
                    g_chat+=(P${pro_id[$i]})
                    gantt_time+=($(( cur_time + b_time[i] )))
                    break
                fi
            done
        fi
        
        if (( cur_pro != -1 )); then

            com_time[$cur_pro]=$(( cur_time + b_time[cur_pro] ))
            cur_time=${com_time[$cur_pro]}

            
            echo "Process P${pro_id[$cur_pro]} completes at time $cur_time"
            pro_sta[$cur_pro]=4
            completed=$(( completed + 1 ))
            
            cur_pro=-1

        else
            local next=999999
            for ((i=0; i<n; i++)); do
                if (( pro_sta[i] == 0 && arr_time[i] < next )); then
                    next=${arr_time[$i]}
                fi
            done
            
            if (( next < 999999 )); then
                echo "CPU idle until time $next"
                g_chat+=("IDLE")
                gantt_time+=(${next})
                cur_time=$next

            else 
                echo "Error: No more processes to schedule but not all completed."
                break
            fi
        fi
    done
    
    local total_wt=0
    local total_tat=0
    
    for (( i=0; i < n; i++ )); do
        ta_time[$i]=$(( com_time[i] - arr_time[i] ))
        wa_time[$i]=$(( ta_time[i] - b_time[i] ))
        total_wt=$(( total_wt + wa_time[i] ))
        total_tat=$(( total_tat + ta_time[i] ))
    done
    
    local total_et=$(( gantt_time[${#gantt_time[@]}-1] - gantt_time[0] ))
    
    GANTT_TIMES=("${gantt_time[@]}")
    gantt_chat "${g_chat[@]}"
    
    echo
    echo "FCFS Scheduling Results:"
    echo "========================"
    echo
    printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time" "Completion Time" "Turnaround Time" "Waiting Time"
    echo "-----------------------------------------------------------------------------------------"
    
    for (( i=0; i < n; i++ )); do
        printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "P${pro_id[$i]}" "${arr_time[$i]}" "${b_time[$i]}" "${com_time[$i]}" "${ta_time[$i]}" "${wa_time[$i]}"
    done
    
    echo
    echo "Performance Metrics:"
    echo "===================="
    echo "Throughput: $(echo "scale=2; $n / $total_et" | bc) processes per unit time"
    echo "Average Waiting Time: $(echo "scale=2; $total_wt / $n" | bc) time units"
    echo "Average Turnaround Time: $(echo "scale=2; $total_tat / $n" | bc) time units"
    echo "Total Execution Time: $total_et time units"
}

# SJF algorithm (non-preemptive)
sjf_algo() {
    local n=$1
    local pro_id=("${!2}")
    local arr_time=("${!3}")
    local b_time=("${!4}")
    
    declare -a com_time
    declare -a wa_time
    declare -a ta_time
    declare -a g_chat
    declare -a gantt_time
    declare -a ready_queue
    declare -a job_queue
    declare -a pro_sta
    declare -a rem_time
    
    for (( i=0; i < n; i++ )); do
        pro_sta[$i]=0
        rem_time[$i]=${b_time[$i]}
    done
    
    local min_arri=${arr_time[0]}
    for (( i=1; i < n; i++ )); do
        if (( arr_time[i] < min_arri )); then
            min_arri=${arr_time[$i]}
        fi
    done
    
    local cur_time=$min_arri
    gantt_time[0]=$cur_time
    
    echo
    echo "SJF Scheduling Simulation:"
    echo "=========================="
    
    local completed=0
    local cur_pro=-1
    
    while (( completed < n )); do
        job_queue=()
        ready_queue=()
        
        for (( i=0; i < n; i++ )); do
            if (( arr_time[i] <= cur_time && pro_sta[i] < 4 )); then

                if (( pro_sta[i] == 0 )); then
                    pro_sta[$i]=1
                fi
                
                if (( pro_sta[i] == 1 )); then
                    job_queue+=(${pro_id[$i]})
                    pro_sta[$i]=2
                fi
                
                if (( pro_sta[i] == 2 && i != cur_pro )); then
                    ready_queue+=(${pro_id[$i]})
                fi
            fi
        done
        
        echo
        echo "At time $cur_time:"
        queue_state "Job Queue" job_queue[@]
        queue_state "Ready Queue" ready_queue[@]
        
        if (( cur_pro == -1 )); then
            local st_job=-1
            local min_burst=999999
            
            for (( i=0; i < n; i++ )); do
                if (( pro_sta[i] == 2 && rem_time[i] < min_burst )); then
                    min_burst=${rem_time[$i]}
                    st_job=$i
                fi
            done
            
            if (( st_job != -1 )); then
                cur_pro=$st_job
                pro_sta[$cur_pro]=3
                
                echo "Process P${pro_id[$cur_pro]} starts execution"
                
                g_chat+=(P${pro_id[$cur_pro]})
                gantt_time+=($(( cur_time + rem_time[cur_pro] )))
            fi
        fi
        
        if (( cur_pro != -1 )); then

            com_time[$cur_pro]=$(( cur_time + rem_time[cur_pro] ))
            cur_time=${com_time[$cur_pro]}
            rem_time[$cur_pro]=0
            
            echo "Process P${pro_id[$cur_pro]} completes at time $cur_time"
            pro_sta[$cur_pro]=4  # Mark as completed
            completed=$(( completed + 1 ))
            
            cur_pro=-1

        else 
            local next=999999
            for (( i=0; i < n; i++ )); do
                if (( pro_sta[i] == 0 && arr_time[i] < next )); then
                    next=${arr_time[$i]}
                fi
            done
            
            if (( next < 999999 )); then
                echo "CPU idle until time $next"
                g_chat+=("IDLE")
                gantt_time+=(${next})
                cur_time=$next

            else 
                echo "Error: No more processes to schedule but not all completed."
                break
            fi
            
        fi
    done
    
    local total_wt=0
    local total_tat=0
    
    for (( i=0; i < n; i++ )); do
        ta_time[$i]=$(( com_time[i] - arr_time[i] ))
        wa_time[$i]=$(( ta_time[i] - b_time[i] ))
        total_wt=$(( total_wt + wa_time[i] ))
        total_tat=$(( total_tat + ta_time[i] ))
    done
    
    local total_et=$(( gantt_time[${#gantt_time[@]}-1] - gantt_time[0] ))
    
    GANTT_TIMES=("${gantt_time[@]}")
    gantt_chat "${g_chat[@]}"
    
    echo
    echo "SJF Scheduling Results:"
    echo "======================="
    echo
    printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time" "Completion Time" "Turnaround Time" "Waiting Time"
    echo "-----------------------------------------------------------------------------------------"
    
    for (( i=0; i < n; i++ )); do
        printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "P${pro_id[$i]}" "${arr_time[$i]}" "${b_time[$i]}" "${com_time[$i]}" "${ta_time[$i]}" "${wa_time[$i]}"
    done
    
    echo
    echo "Performance Metrics:"
    echo "===================="
    echo "Throughput: $(echo "scale=2; $n / $total_et" | bc) processes per unit time"
    echo "Average Waiting Time: $(echo "scale=2; $total_wt / $n" | bc) time units"
    echo "Average Turnaround Time: $(echo "scale=2; $total_tat / $n" | bc) time units"
    echo "Total Execution Time: $total_et time units"

}

# Round Robin(RR) Algorithm
rr_algo() {
    if ((no_pro == 0)); then
        return
    fi
    
    local n=$1
    local pro_id=("${!2}")
    local arr_time=("${!3}")
    local b_time=("${!4}")
    
    echo
    read -p "Enter time quantum for Round Robin: " t_qua
    
    declare -a com_time
    declare -a wa_time
    declare -a ta_time
    declare -a rem_time
    declare -a g_chat
    declare -a gantt_time
    declare -a ready_queue
    declare -a job_queue
    declare -a pro_sta
    declare -a last_exe
    
    for (( i=0; i < n; i++ )); do
        com_time[$i]=0
        rem_time[$i]=${b_time[$i]}
        pro_sta[$i]=0
        last_exe[$i]=0
    done
    
    local min_arri=${arr_time[0]}
    for (( i=1; i < n; i++ )); do
        if (( arr_time[i] < min_arri )); then
            min_arri=${arr_time[$i]}
        fi
    done
    
    local cur_time=$min_arri
    gantt_time[0]=$cur_time
    
    echo
    echo "======================="
    echo "Process Table:"
    echo "======================="
    printf "%-10s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time"
    echo "--------------------------------------"
    
    for (( i=0; i < n; i++ )); do
        printf "%-10s %-15s %-15s\n" "P${pro_id[$i]}" "${arr_time[$i]}" "${b_time[$i]}"
    done
    
    echo
    echo "=========================================================="
    echo "Starting Round Robin (Quantum=$t_qua) Simulation..."
    echo "=========================================================="
    
    local completed=0
    
    while (( completed < n )); do
        echo
        echo "=============================="
        echo "Time: $cur_time"
        echo "=============================="
        
        job_queue=()
        ready_queue=()
        remaining_queue=()
        
        for (( i=0; i < n; i++ )); do
            if (( arr_time[i] <= cur_time && pro_sta[i] < 4 )); then

                if (( pro_sta[i] == 0 )); then
                    pro_sta[$i]=1
                fi
                
                if (( pro_sta[i] == 1 )); then
                    job_queue+=(${pro_id[$i]})
                    pro_sta[$i]=2
                fi
                
                if (( pro_sta[i] == 2 && rem_time[i] > 0 )); then
                    ready_queue+=(${pro_id[$i]})
                    remaining_queue+=(${rem_time[$i]})
                fi
            fi
        done
        
        dis_queue "Job Queue" job_queue[@] rem_time[@] $cur_time
        dis_queue "Ready Queue" ready_queue[@] rem_time[@] $cur_time
        
        if [ ${#ready_queue[@]} -eq 0 ]; then

            local next=999999
            for (( i=0; i < n; i++ )); do
                if (( pro_sta[i] < 2 && arr_time[i] < next )); then
                    next=${arr_time[$i]}
                fi
            done
            
            if (( next < 999999 && next > cur_time )); then
                echo
                echo "** CPU idle until time $next **"
                g_chat+=("IDLE")
                gantt_time+=(${next})
                cur_time=$next
                continue

            elif (( completed < n )); then
                echo
                echo "!! Error: No more processes to schedule but not all completed !!"
                break
            fi
        fi
        
        local rr=()
        for i in "${ready_queue[@]}"; do

            for (( j=0; j < n; j++ )); do
                if (( pro_id[j] == i )); then
                    rr+=($i)
                    break
                fi
            done
        done
        
        if [ ${#rr[@]} -gt 0 ]; then
            local curr_pid=${rr[0]}
            local cur_idx=-1
            
            for (( j=0; j < n; j++ )); do
                if (( pro_id[j] == curr_pid )); then
                    cur_idx=$j
                    break
                fi
            done
            
            if (( cur_idx != -1 )); then
                echo
                echo ">> Process P${pro_id[$cur_idx]} gets CPU"

                local e_time=$t_qua
                if (( rem_time[cur_idx] < t_qua )); then
                    e_time=${rem_time[$cur_idx]}
                fi
                
                echo
                echo "   Executing P${pro_id[$cur_idx]}: ["
                for (( i=0; i < 10; i++ )); do
                    echo -n "■"
                done
                echo "] ($e_time time units)"
                
                g_chat+=(P${pro_id[$cur_idx]})
                cur_time=$(( cur_time + e_time ))
                gantt_time+=(${cur_time})
                
                rem_time[$cur_idx]=$(( rem_time[cur_idx] - e_time ))
                last_exe[$cur_idx]=$cur_time
                
                if (( rem_time[$cur_idx] <= 0 )); then
                    echo
                    echo "✓✓ Process P${pro_id[$cur_idx]} COMPLETES at time $cur_time"
                    com_time[$cur_idx]=$cur_time
                    pro_sta[$cur_idx]=4
                    (( completed++ ))
                    
                else 
                    echo
                    echo ">> Process P${pro_id[$cur_idx]} PREEMPTED at time $cur_time"
                    echo "   Remaining time: ${rem_time[$cur_idx]} units"
                    
                    pro_sta[$cur_idx]=2
                fi
                
                if [ ${#ready_queue[@]} -gt 0 ]; then
                    ready_queue=("${ready_queue[@]:1}")
                    
                    if (( rem_time[cur_idx] > 0 )); then
                        ready_queue+=(${pro_id[$cur_idx]})
                    fi
                fi
            fi
        fi
    done
    
    local total_wt=0
    local total_tat=0
    
    for (( i=0; i < n; i++ )); do
        ta_time[$i]=$(( com_time[i] - arr_time[i] ))
        wa_time[$i]=$(( ta_time[i] - b_time[i] ))
        total_wt=$(( total_wt + wa_time[i] ))
        total_tat=$(( total_tat + ta_time[i] ))
    done
    
    local total_et=$(( gantt_time[${#gantt_time[@]}-1] - gantt_time[0] ))

    
    GANTT_TIMES=("${gantt_time[@]}")
    gantt_chat "${g_chat[@]}"
    
    echo
    echo "============================================================"
    echo "Round Robin (Quantum=$t_qua) Scheduling Results:"
    echo "============================================================"
    echo
    printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "Process" "Arrival Time" "Burst Time" "Completion Time" "Turnaround Time" "Waiting Time"
    echo "-----------------------------------------------------------------------------------------"
    
    for (( i=0; i < n; i++ )); do
        printf "%-10s %-15s %-15s %-15s %-15s %-15s\n" "P${pro_id[$i]}" "${arr_time[$i]}" "${b_time[$i]}" "${com_time[$i]}" "${ta_time[$i]}" "${wa_time[$i]}"
    done
    
    echo
    echo "=================="
    echo "Performance Metrics:"
    echo "=================="
    echo "→ Throughput: $(echo "scale=2; $n / $total_et" | bc) processes per unit time"
    echo "→ Average Waiting Time: $(echo "scale=2; $total_wt / $n" | bc) time units"
    echo "→ Average Turnaround Time: $(echo "scale=2; $total_tat / $n" | bc) time units"
    echo "→ Total Execution Time: $total_et time units"
    
    echo
    echo "==============================="
    echo "Round Robin Algorithm Explanation:"
    echo "==============================="
    echo "Round Robin (RR) is a preemptive CPU scheduling algorithm:"
    echo "  • Each process gets a small unit of CPU time (time quantum)"
    echo "  • After time quantum expires, process is preempted and moved to the end of the queue"
    echo "  • Ensures fairness as each process gets equal CPU time in each cycle"
    echo "  • Good for time-sharing systems and interactive processes"
    echo "  • Performance depends heavily on the choice of time quantum"
    echo "  • Small quantum: better response time but more context switches"
    echo "  • Large quantum: fewer context switches but may behave like FCFS"
    echo
}

# Displaying queue
dis_queue() {
    local queue_name=$1
    local queue=("${!2}")
    local b_rem=("${!3}")
    local curr_time=$4
    
    echo
    echo "$queue_name:"
    echo "$(printf '=%.0s' $(seq 1 ${#queue_name}))"
    
    if [ ${#queue[@]} -eq 0 ]; then
        echo "[Empty]"

    else
        echo "┌─────────────────────────────────────────┐"
        echo -n "│ "
        
        for (( i=0; i < ${#queue[@]}; i++ )); do
            local proc_id=${queue[$i]}
            local proc_index=-1
            
            for (( j=0; j < no_pro; j++ )); do
                if (( process_id[j] == proc_id )); then
                    proc_index=$j
                    break
                fi
            done
            
            if [[ "$queue_name" == *"Ready Queue"* ]] && (( proc_index != -1 )); then
                echo -n "[P$proc_id:${rem_time[$proc_index]}] "
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
com_algo() {
    echo
    echo "================================================="
    echo "ALGORITHM COMPARISON AND RECOMMENDATION"
    echo "================================================="
    
    local n=${#burst_time[@]}
    local similar=1
    local s_ari=1
    local f_burst=${burst_time[0]}
    local f_ari=${arrival_time[0]}
    
    for (( i=1; i < n; i++ )); do
        if (( ${burst_time[$i]} != f_burst )); then
            similar=0
            break
        fi
    done
    
    for (( i=1; i < n; i++ )); do
        if (( ${arrival_time[$i]} != f_ari )); then
            s_ari=0
            break
        fi
    done
    
    echo
    echo "ANALYSIS OF YOUR PROCESS DATA:"
    echo "----------------------------------"
    
    if (( similar == 1 && s_ari == 1 )); then
        echo "✓ All processes have similar burst times and arrival times"
        echo
        echo "RECOMMENDATION:"
        echo "► Round Robin would be fair and provide good response times"
        echo "► FCFS would also work well in this scenario"
        echo
        echo "BEST CHOICE: Round Robin with an appropriate time quantum"

    elif (( similar == 1 && s_ari == 0 )); then
        echo "✓ All processes have similar burst times but different arrival times"
        echo
        echo "RECOMMENDATION:"
        echo "► FCFS would be simple and fair in this case"
        echo
        echo "BEST CHOICE: FCFS (First-Come, First-Served)"

    elif (( similar == 0 && s_ari == 1 )); then
        echo "✓ Processes have different burst times but arrive at similar times"
        echo
        echo "RECOMMENDATION:"
        echo "► SJF would minimize average waiting time"
        echo
        echo "BEST CHOICE: SJF (Shortest Job First)"

    else
        local sum=0
        local sum_sq=0
        
        for (( i=0; i < n; i++ )); do
            sum=$((sum + ${burst_time[$i]}))
            sum_sq=$((sum_sq + ${burst_time[$i]}*${burst_time[$i]}))
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
    echo "the specific pattern of your process data."
    echo "================================================="
}

about() {
    clear
    echo "================================================="
    echo "          MultiAlgo-CPU-Scheduler"
    echo "================================================="
    echo "               Version 1.0"
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
    menu
    
    case $choice in
        1)
            process
            fcfs_algo $no_pro process_id[@] arrival_time[@] burst_time[@]
            com_algo
            ;;
        2)
            process
            sjf_algo $no_pro process_id[@] arrival_time[@] burst_time[@]
            com_algo
            ;;
        3)
            process
            rr_algo $no_pro process_id[@] arrival_time[@] burst_time[@]
            com_algo
            ;;
        4)
            about
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