#!/bin/bash

# Configuration
STUDENT_DB="students.db"
RESULTS_DB="results.db"
QUESTIONS_DIR="questions"
EXAM_CONFIG="exam_config.txt"
ADMIN_PASS="admin123"  # Specific password for Admin login

# Create required files/directories
mkdir -p "$QUESTIONS_DIR"
touch "$STUDENT_DB" "$RESULTS_DB"

# Create default exam durations if config doesn't exist
function create_default_config {
    if [ ! -f "$EXAM_CONFIG" ]; then
        echo "physics_1:300" > "$EXAM_CONFIG"      # 5 minutes
        echo "physics_2:300" >> "$EXAM_CONFIG"     # 5 minutes
        echo "chemistry_1:600" >> "$EXAM_CONFIG"   # 10 minutes
        echo "chemistry_2:600" >> "$EXAM_CONFIG"   # 10 minutes
        echo "higher_math:900" >> "$EXAM_CONFIG"   # 15 minutes
    fi
}

# Function to get exam duration
function get_exam_duration {
    exam_name=$1
    duration=$(grep "^$exam_name:" "$EXAM_CONFIG" | cut -d: -f2)
    if [ -z "$duration" ]; then
        echo "300"  # Default 5 minutes
    else
        echo "$duration"
    fi
}

# Initialize configuration
create_default_config

# Main Menu
function main_menu {
    clear
    echo "===================================="
    echo "  QuickQuiz (Online Exam System)    "
    echo "===================================="
    echo "1. Student Login"
    echo "2. Student Registration"
    echo "3. Admin Login"
    echo "4. Exit"
    read -p "Enter your choice: " choice

    case $choice in
        1) student_login ;;
        2) student_registration ;;
        3) admin_login ;;
        4) exit 0 ;;
        *) echo "Invalid choice"; sleep 1; main_menu ;;
    esac
}

# Student Registration
function student_registration {
    clear
    echo "STUDENT REGISTRATION"
    read -p "Enter username: " username #taking input from the user
    
    # Check if username exists
    if grep -q "^$username:" "$STUDENT_DB"; then
        echo "Username already exists!"
        sleep 2
        main_menu
        return
    fi
    
    read -s -p "Enter password: " password
    echo
    read -s -p "Confirm password: " password2
    echo
    
    if [ "$password" != "$password2" ]; then
        echo "Passwords don't match!"
        sleep 2  # pause for 2 seconds
        main_menu
        return
    fi
    
    # Store hashed password (basic security)
    password_hash=$(echo -n "$password" | sha256sum | cut -d' ' -f1)
    echo "$username:$password_hash" >> "$STUDENT_DB"
    echo "Registration successful!"
    sleep 2
    main_menu
}

# Student Login
function student_login {
    clear
    echo "STUDENT LOGIN"
    read -p "Enter username: " username
    read -s -p "Enter password: " password
    echo
    
    # Verify credentials
    stored_hash=$(grep "^$username:" "$STUDENT_DB" | cut -d: -f2)
    input_hash=$(echo -n "$password" | sha256sum | cut -d' ' -f1)
    
    if [ "$stored_hash" = "$input_hash" ]; then
        student_dashboard "$username"
    else
        echo "Invalid username or password!"
        sleep 2
        main_menu
    fi
}

# Student Dashboard
function student_dashboard {
    username=$1
    clear
    echo "Welcome, $username!"
    echo "1. Take Exam"
    echo "2. View Results Summary"
    echo "3. View Detailed Results"
    echo "4. Logout"
    read -p "Enter your choice: " choice
    
    case $choice in
        1) select_exam "$username" ;;
        2) view_results "$username" ;;
        3) view_detailed_results "$username" ;;
        4) main_menu ;;
        *) echo "Invalid choice"; sleep 1; student_dashboard "$username" ;;
    esac
}

# Select Exam
function select_exam {
    username=$1
    clear
    echo "SELECT EXAM"
    
    # Display exam options with durations
    echo "1. Physics 1st Paper ($(( $(get_exam_duration "physics_1") / 60 )) minutes)"
    echo "2. Physics 2nd Paper ($(( $(get_exam_duration "physics_2") / 60 )) minutes)"
    echo "3. Chemistry 1st Paper ($(( $(get_exam_duration "chemistry_1") / 60 )) minutes)"
    echo "4. Chemistry 2nd Paper ($(( $(get_exam_duration "chemistry_2") / 60 )) minutes)"
    echo "5. Higher Math ($(( $(get_exam_duration "higher_math") / 60 )) minutes)"
    echo "6. Back"
    read -p "Enter exam number: " choice
    
    case $choice in
        1) take_exam "$username" "physics_1" ;;
        2) take_exam "$username" "physics_2" ;;
        3) take_exam "$username" "chemistry_1" ;;
        4) take_exam "$username" "chemistry_2" ;;
        5) take_exam "$username" "higher_math" ;;
        6) student_dashboard "$username" ;;
        *) echo "Invalid choice"; sleep 1; select_exam "$username" ;;
    esac
}

function take_exam {
    username=$1
    exam_name=$2
    exam_file="$QUESTIONS_DIR/$exam_name.txt"
    wrong_answers=()  # initialize an empty array
    
    # Exam file checking
    if [ ! -f "$exam_file" ]; then
        echo "Error: Exam file '$exam_file' not found!"
        sleep 2
        student_dashboard "$username"
        return
    fi

    if [ ! -s "$exam_file" ]; then
        echo "Error: Exam file '$exam_file' is empty!"
        sleep 2
        student_dashboard "$username"
        return
    fi

    # Load questions safely
    questions=() #initializes an empty array
    while IFS= read -r line || [[ -n "$line" ]]; do
        line=$(echo "$line" | tr -d '\r')  # Remove Windows line endings
        [[ -z "$line" ]] && continue
        
        # Count the number of pipes
        pipe_count=$(grep -o "|" <<< "$line" | wc -l) #Q1|OptionA|OptionB|OptionC|OptionD|Answer (count number of pipe character)
        if [ "$pipe_count" -ne 5 ]; then
            echo "Warning: Skipping malformed question (needs 5 pipes): $line"
            continue
        fi
        
        questions+=("$line")
    done < "$exam_file"

    total_questions=${#questions[@]} # length of the question array
    
    if [ "$total_questions" -eq 0 ]; then
        echo "No valid questions found in this exam!"
        sleep 2
        student_dashboard "$username"
        return
    fi

    # ENHANCEMENT: Shuffle questions for random order
    if [ "$total_questions" -gt 1 ]; then
        # Create temporary file for shuffling
        temp_file=$(mktemp)
        printf '%s\n' "${questions[@]}" > "$temp_file"
        
        # Read shuffled questions back
        questions=()
        while IFS= read -r line; do
            questions+=("$line")
        done < <(shuf "$temp_file")
        
        rm "$temp_file"
    fi

    # ENHANCEMENT: Get configurable exam duration
    duration=$(get_exam_duration "$exam_name")
    duration_minutes=$((duration / 60))

    clear
    echo "EXAM: ${exam_name^^}"
    echo "You have $duration_minutes minutes to complete this exam."
    echo "Questions will appear in random order."
    echo "Press Enter to start..."
    read

    # Timer setup
    start_time=$(date +%s)
    end_time=$((start_time + duration))
    score=0
    
    # start questions asking loop
    for ((i=0; i<total_questions; i++)); do
        current_time=$(date +%s)
        if [ $current_time -ge $end_time ]; then
            echo -e "\nTime's up!"
            break
        fi
        
        # Calculate remaining time properly
        remaining_time=$((end_time - current_time))
        minutes=$((remaining_time / 60))
        seconds=$((remaining_time % 60))
        
        IFS='|' read -ra parts <<< "${questions[$i]}" # parts[0] = question text.....parts[5] = correct answer
        # Skip invalied questions (must have 6 parts)
        if [ ${#parts[@]} -ne 6 ]; then
            continue
        fi
        
        q_text="${parts[0]}"
        options=("${parts[@]:1:4}")
        correct=$((parts[5]-1)) # Convert to 0-based index
        
        clear
        # ENHANCEMENT: Show progress and time remaining
        echo "Question $((i+1)) of $total_questions | Time: ${minutes}m ${seconds}s"
        echo "================================================"
        echo "$q_text"
        echo ""
        for o in "${!options[@]}"; do
            echo "$((o+1))) ${options[$o]}"
        done
        echo ""
        read -p "Your answer (1-4): " answer
        
        # Validate answer
        if [[ "$answer" =~ ^[1-4]$ ]]; then
            if [ $((answer-1)) -eq $correct ]; then
                ((score++))
            else
                wrong_answers+=("Q$((i+1))-Your:$answer/Correct:$((correct+1))")
            fi
        fi
    done
    
    # Calculate percentage score(with zero-division check)
    if [ $total_questions -gt 0 ]; then
        percentage=$((score * 100 / total_questions))
    else
        percentage=0
    fi
    
    # Save result with wrong answers
    wrong_answers_str=$(IFS=','; echo "${wrong_answers[*]}")
    echo "$username:$exam_name:$score/$total_questions:$percentage%:$(date +'%Y-%m-%d %H:%M'):$wrong_answers_str" >> "$RESULTS_DB"
    
    # Show result
    clear
    echo "EXAM COMPLETED"
    echo "=============="
    echo "Score: $score/$total_questions ($percentage%)"
    if [ ${#wrong_answers[@]} -gt 0 ]; then
        echo -e "\nIncorrect Answers:"
        for wrong in "${wrong_answers[@]}"; do
            echo " - $wrong"
        done
    else
        echo -e "\nPerfect score! Well done!"
    fi
    read -p "Press Enter to continue..."
    student_dashboard "$username"
}

# View Results Summary
function view_results {
    username=$1
    clear
    echo "YOUR EXAM RESULTS (SUMMARY)"
    grep "^$username:" "$RESULTS_DB" | cut -d: -f1-5 | column -t -s: | nl
    read -p "Press Enter to continue..."
    student_dashboard "$username"
}

# View Detailed Results
function view_detailed_results {
    username=$1
    clear
    echo "YOUR DETAILED EXAM RESULTS"
    echo "=========================="
    
    # Get all attempts by this user
    grep "^$username:" "$RESULTS_DB" | while IFS=: read -r user exam score_pct percentage date wrong; do
        echo -e "\nExam: $exam | Score: $score_pct ($percentage) | Date: $date"
        
        # Show wrong answers if any
        if [ -n "$wrong" ]; then
            echo -e "\nIncorrect Answers:"
            IFS=',' read -ra wrong_items <<< "$wrong"
            for item in "${wrong_items[@]}"; do
                echo " - $item"
            done
        else
            echo -e "\nPerfect score! No incorrect answers."
        fi
        echo "----------------------------------"
    done
    
    read -p "Press Enter to continue..."
    student_dashboard "$username"
}

# Admin Login
function admin_login {
    clear
    echo "ADMIN LOGIN"
    read -s -p "Enter admin password: " password
    echo
    
    if [ "$password" == "$ADMIN_PASS" ]; then
        admin_dashboard
    else
        echo "Invalid password!"
        sleep 2
        main_menu
    fi
}

# Admin Dashboard
function admin_dashboard {
    clear
    echo "ADMIN DASHBOARD"
    echo "1. View All Students"
    echo "2. View All Results"
    echo "3. View Exam Statistics"
    echo "4. Manage Exam Durations"
    echo "5. Change Admin Password"
    echo "6. Logout"
    read -p "Enter your choice: " choice
    
    case $choice in
        1) view_all_students ;;
        2) view_all_results ;;
        3) view_stats ;;
        4) manage_exam_durations ;;
        5) change_admin_pass ;;
        6) main_menu ;;
        *) echo "Invalid choice"; sleep 1; admin_dashboard ;;
    esac
}

# ENHANCEMENT: New function to manage exam durations
function manage_exam_durations {
    clear
    echo "MANAGE EXAM DURATIONS"
    echo "===================="
    echo "Current settings:"
    echo ""
    
    if [ -f "$EXAM_CONFIG" ]; then
        while IFS=: read -r exam duration; do
            minutes=$((duration / 60))
            echo "  $exam: $minutes minutes"
        done < "$EXAM_CONFIG"
    fi
    
    echo ""
    echo "1. Change Physics 1st Paper duration"
    echo "2. Change Physics 2nd Paper duration" 
    echo "3. Change Chemistry 1st Paper duration"
    echo "4. Change Chemistry 2nd Paper duration"
    echo "5. Change Higher Math duration"
    echo "6. Back to admin dashboard"
    
    read -p "Enter choice: " choice
    
    case $choice in
        1) change_duration "physics_1" "Physics 1st Paper" ;;
        2) change_duration "physics_2" "Physics 2nd Paper" ;;
        3) change_duration "chemistry_1" "Chemistry 1st Paper" ;;
        4) change_duration "chemistry_2" "Chemistry 2nd Paper" ;;
        5) change_duration "higher_math" "Higher Math" ;;
        6) admin_dashboard ;;
        *) echo "Invalid choice"; sleep 1; manage_exam_durations ;;
    esac
}

function change_duration {
    exam_name=$1
    exam_display_name=$2
    current_duration=$(get_exam_duration "$exam_name")
    current_minutes=$((current_duration / 60))
    
    echo ""
    echo "Current duration for $exam_display_name: $current_minutes minutes"
    read -p "Enter new duration in minutes: " new_minutes
    
    # Validate input
    if [[ "$new_minutes" =~ ^[0-9]+$ ]] && [ "$new_minutes" -gt 0 ]; then
        new_duration=$((new_minutes * 60))
        
        # Update config file
        if grep -q "^$exam_name:" "$EXAM_CONFIG"; then
            # Replace existing entry
            sed -i "s/^$exam_name:.*/$exam_name:$new_duration/" "$EXAM_CONFIG"
        else
            # Add new entry
            echo "$exam_name:$new_duration" >> "$EXAM_CONFIG"
        fi
        
        echo "Duration updated to $new_minutes minutes!"
    else
        echo "Invalid input! Please enter a positive number."
    fi
    
    sleep 2
    manage_exam_durations
}

# Admin Functions
function view_all_students {
    clear
    echo "REGISTERED STUDENTS"
    cut -d: -f1 "$STUDENT_DB" | nl  # To show a numbered list of registered student
    read -p "Press Enter to continue..."
    admin_dashboard
}

function view_all_results {
    clear
    echo "ALL EXAM RESULTS"
    column -t -s: "$RESULTS_DB" | nl #list of all student results in a table
    read -p "Press Enter to continue..."
    admin_dashboard
}

function view_stats {
    clear
    echo "SYSTEM STATISTICS"
    echo "================="
    echo "Total students: $(wc -l < "$STUDENT_DB")"
    echo "Total exams taken: $(wc -l < "$RESULTS_DB")"
    echo -e "\nExam-wise performance:"
    
    for exam in $(ls "$QUESTIONS_DIR"/*.txt 2>/dev/null | xargs -n1 basename 2>/dev/null | cut -d. -f1); do
        count=$(grep -c ":$exam:" "$RESULTS_DB" 2>/dev/null || echo "0")
        if [ $count -gt 0 ]; then
            avg=$(grep ":$exam:" "$RESULTS_DB" | awk -F: '{split($4,pct,"%"); sum+=pct[1]} END {print sum/NR}') # NR = number of records
            duration_minutes=$(( $(get_exam_duration "$exam") / 60 ))
            echo "- ${exam}: $count attempts, ${avg%.*}% average, ${duration_minutes}min duration"
        fi
    done
    
    read -p "Press Enter to continue..."
    admin_dashboard
}

function change_admin_pass {
    clear
    read -s -p "Enter new admin password: " new_pass
    echo
    read -s -p "Confirm new password: " new_pass2
    echo
    
    if [ "$new_pass" == "$new_pass2" ]; then
        ADMIN_PASS="$new_pass"
        echo "Password changed successfully!"
    else
        echo "Passwords don't match!"
    fi
    sleep 2
    admin_dashboard
}

# Start the system
main_menu


