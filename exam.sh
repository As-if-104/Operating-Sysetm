#!/bin/bash
# QuickQuiz: Online Exam System (Multi-Subject)
# Author: [Your Name]
# Features: Registration, Login, Timed Exams, Auto-Grading, Admin Dashboard

# --- Configuration ---
STUDENT_DB="students.db"
RESULTS_DB="results.db"
QUESTIONS_DIR="questions"
ADMIN_PASS="admin123"  # Default admin password

# --- Setup ---
mkdir -p "$QUESTIONS_DIR"
touch "$STUDENT_DB" "$RESULTS_DB"

# --- Color Codes ---
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# --- Main Menu ---
function main_menu {
    clear
    echo -e "${CYAN}===================================="
    echo "  QuickQuiz (Online Exam System)    "
    echo -e "====================================${NC}"
    echo "1. Admin Login"
    echo "2. Student Registration"
    echo "3. Student Login"
    echo "4. Student Profile"
    echo "5. Exit"
    echo -e "${CYAN}====================================${NC}"
    read -p "Enter your choice: " choice
    case "$choice" in
        1) admin_login ;;
        2) student_registration ;;
        3) student_login ;;
        4) student_profile ;;
        5) exit 0 ;;
        *) echo -e "${RED}Invalid choice!${NC}"; sleep 1; main_menu ;;
    esac
}

# --- Student Registration ---
function student_registration {
    clear
    echo -e "${CYAN}STUDENT REGISTRATION${NC}"

    read -p "Enter Your Name: " name
    if [[ -z "$name" ]]; then
        echo -e "${RED}Name cannot be empty!${NC}"
        sleep 2
        main_menu
        return
    fi

    read -p "Enter Your Username: " username
    if grep -q "^$username:" "$STUDENT_DB"; then
        echo -e "${RED}Username already exists!${NC}"
        sleep 2
        main_menu
        return
    fi

    read -p "Enter Your Phone Number: " phone
    if [[ -z "$phone" || ! "$phone" =~ ^[0-9]{10,15}$ ]]; then
        echo -e "${RED}Invalid phone number!${NC}"
        sleep 2
        main_menu
        return
    fi

    read -p "Enter Your Email: " email
    if [[ -z "$email" || ! "$email" =~ ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-Z]{2,}$ ]]; then
        echo -e "${RED}Invalid email format!${NC}"
        sleep 2
        main_menu
        return
    fi

    read -s -p "Enter password: " password; echo
    read -s -p "Confirm password: " password2; echo

    if [[ "$password" != "$password2" ]]; then
        echo -e "${RED}Passwords don't match!${NC}"
        sleep 2
        main_menu
        return
    fi

    # Store hashed password
    password_hash=$(echo -n "$password" | sha256sum | cut -d' ' -f1)
    echo "$username:$password_hash" >> "$STUDENT_DB"
    echo -e "${GREEN}Registration successful!${NC}"
    sleep 2
    main_menu
}

# --- Student Login ---
function student_login {
    clear
    echo -e "${CYAN}STUDENT LOGIN${NC}"
    read -p "Enter username: " username
    read -s -p "Enter password: " password; echo

    stored_hash=$(grep "^$username:" "$STUDENT_DB" | cut -d: -f2)
    input_hash=$(echo -n "$password" | sha256sum | cut -d' ' -f1)

    if [[ "$stored_hash" == "$input_hash" && -n "$stored_hash" ]]; then
        student_dashboard "$username"
    else
        echo -e "${RED}Invalid username or password!${NC}"
        sleep 2
        main_menu
    fi
}

# --- Student Dashboard ---
function student_dashboard {
    local username="$1"
    clear
    echo -e "${CYAN}Welcome, $username!${NC}"
    echo "1. Take Exam"
    echo "2. View Results Summary"
    echo "3. View Detailed Results"
    echo "4. Logout"
    read -p "Enter your choice: " choice
    case "$choice" in
        1) select_exam "$username" ;;
        2) view_results "$username" ;;
        3) view_detailed_results "$username" ;;
        4) main_menu ;;
        *) echo -e "${RED}Invalid choice!${NC}"; sleep 1; student_dashboard "$username" ;;
    esac
}

# --- Exam Selection ---
function select_exam {
    local username="$1"
    clear
    echo -e "${CYAN}SELECT EXAM${NC}"
    local exams=("Physics 1st Paper" "Physics 2nd Paper" "Chemistry 1st Paper" "Chemistry 2nd Paper" "Higher Math" "Back")
    for i in "${!exams[@]}"; do
        echo "$((i+1)). ${exams[$i]}"
    done
    read -p "Enter exam number: " choice
    case "$choice" in
        1) take_exam "$username" "physics_1" ;;
        2) take_exam "$username" "physics_2" ;;
        3) take_exam "$username" "chemistry_1" ;;
        4) take_exam "$username" "chemistry_2" ;;
        5) take_exam "$username" "higher_math" ;;
        6) student_dashboard "$username" ;;
        *) echo -e "${RED}Invalid choice!${NC}"; sleep 1; select_exam "$username" ;;
    esac
}

# --- Take Exam ---
function take_exam {
    local username="$1"
    local exam_name="$2"
    local exam_file="$QUESTIONS_DIR/$exam_name.txt"
    local wrong_answers=()

    if [[ ! -f "$exam_file" ]]; then
        echo -e "${RED}Error: Exam file '$exam_file' not found!${NC}"
        sleep 2
        student_dashboard "$username"
        return
    fi
    if [[ ! -s "$exam_file" ]]; then
        echo -e "${RED}Error: Exam file '$exam_file' is empty!${NC}"
        sleep 2
        student_dashboard "$username"
        return
    fi

    # Load questions
    local questions=()
    while IFS= read -r line || [[ -n "$line" ]]; do
        line=$(echo "$line" | tr -d '\r')
        [[ -z "$line" ]] && continue
        [[ $(grep -o "|" <<< "$line" | wc -l) -ne 5 ]] && continue
        questions+=("$line")
    done < "$exam_file"
    local total_questions="${#questions[@]}"
    if [[ "$total_questions" -eq 0 ]]; then
        echo -e "${RED}No valid questions found in this exam!${NC}"
        sleep 2
        student_dashboard "$username"
        return
    fi

    clear
    echo -e "${CYAN}EXAM: ${exam_name^^}${NC}"
    echo "You have 5 minutes to complete this exam."
    read -p "Press Enter to start..."

    local duration=300
    local start_time end_time current_time remaining_time minutes seconds
    start_time=$(date +%s)
    end_time=$((start_time + duration))
    local score=0

    for ((i=0; i<total_questions; i++)); do
        current_time=$(date +%s)
        if (( current_time >= end_time )); then
            echo -e "\n${YELLOW}Time's up!${NC}"
            break
        fi
        remaining_time=$((end_time - current_time))
        minutes=$((remaining_time / 60))
        seconds=$((remaining_time % 60))

        IFS='|' read -ra parts <<< "${questions[$i]}"
        [[ ${#parts[@]} -ne 6 ]] && continue

        local q_text="${parts[0]}"
        local options=("${parts[@]:1:4}")
        local correct=$((parts[5]-1))

        clear
        echo -e "${YELLOW}Time remaining: $minutes min $seconds sec${NC}"
        echo "Question $((i+1)): $q_text"
        for o in "${!options[@]}"; do
            echo "$((o+1))) ${options[$o]}"
        done
        read -p "Your answer (1-4): " answer

        if [[ "$answer" =~ ^[1-4]$ ]]; then
            if (( answer-1 == correct )); then
                ((score++))
            else
                wrong_answers+=("Q$((i+1))-Your answer:$answer/Correct answer:$((correct+1))")
            fi
        fi
    done

    local percentage=0
    if (( total_questions > 0 )); then
        percentage=$((score * 100 / total_questions))
    fi

    local wrong_answers_str
    wrong_answers_str=$(IFS=','; echo "${wrong_answers[*]}")
    echo "$username:$exam_name:$score/$total_questions:$percentage%:$(date +'%Y-%m-%d %H:%M'):$wrong_answers_str" >> "$RESULTS_DB"

    clear
    echo -e "${GREEN}EXAM COMPLETED${NC}"
    echo "Score: $score/$total_questions ($percentage%)"
    if (( ${#wrong_answers[@]} > 0 )); then
        echo -e "\n${RED}Incorrect Answers:${NC}"
        for wrong in "${wrong_answers[@]}"; do
            echo " - $wrong"
        done
    else
        echo -e "\n${GREEN}Perfect score!${NC}"
    fi
    read -p "Press Enter to continue..."
    student_dashboard "$username"
}

# --- View Results Summary ---
function view_results {
    local username="$1"
    clear
    echo -e "${CYAN}YOUR EXAM RESULTS (SUMMARY)${NC}"
    grep "^$username:" "$RESULTS_DB" | cut -d: -f1-5 | column -t -s: | nl
    read -p "Press Enter to continue..."
    student_dashboard "$username"
}

# --- View Detailed Results ---
function view_detailed_results {
    local username="$1"
    clear
    echo -e "${CYAN}YOUR DETAILED EXAM RESULTS${NC}"
    echo "=========================="
    grep "^$username:" "$RESULTS_DB" | while IFS=: read -r user exam score_pct percentage date wrong; do
        echo -e "\nExam: $exam | Score: $score_pct ($percentage) | Date: $date"
        if [[ -n "$wrong" ]]; then
            echo -e "\n${RED}Incorrect Answers:${NC}"
            IFS=',' read -ra wrong_items <<< "$wrong"
            for item in "${wrong_items[@]}"; do
                echo " - $item"
            done
        else
            echo -e "\n${GREEN}Perfect score! No incorrect answers.${NC}"
        fi
        echo "----------------------------------"
    done
    read -p "Press Enter to continue..."
    student_dashboard "$username"
}

# --- Admin Login ---
function admin_login {
    clear
    echo -e "${CYAN}ADMIN LOGIN${NC}"
    read -s -p "Enter admin password: " password; echo
    if [[ "$password" == "$ADMIN_PASS" ]]; then
        admin_dashboard
    else
        echo -e "${RED}Invalid password!${NC}"
        sleep 2
        main_menu
    fi
}

# --- Admin Dashboard ---
function admin_dashboard {
    clear
    echo -e "${CYAN}ADMIN DASHBOARD${NC}"
    echo "1. View All Students"
    echo "2. View All Results"
    echo "3. View Exam Statistics"
    echo "4. Change Admin Password"
    echo "5. Logout"
    read -p "Enter your choice: " choice
    case "$choice" in
        1) view_all_students ;;
        2) view_all_results ;;
        3) view_stats ;;
        4) change_admin_pass ;;
        5) main_menu ;;
        *) echo -e "${RED}Invalid choice!${NC}"; sleep 1; admin_dashboard ;;
    esac
}

# --- Admin: View All Students ---
function view_all_students {
    clear
    echo -e "${CYAN}REGISTERED STUDENTS${NC}"
    cut -d: -f1 "$STUDENT_DB" | nl
    read -p "Press Enter to continue..."
    admin_dashboard
}

# --- Admin: View All Results ---
function view_all_results {
    clear
    echo -e "${CYAN}ALL EXAM RESULTS${NC}"
    column -t -s: "$RESULTS_DB" | nl
    read -p "Press Enter to continue..."
    admin_dashboard
}

# --- Admin: View Statistics ---
function view_stats {
    clear
    echo -e "${CYAN}SYSTEM STATISTICS${NC}"
    echo "Total students: $(wc -l < "$STUDENT_DB")"
    echo "Total exams taken: $(wc -l < "$RESULTS_DB")"
    echo -e "\nExam-wise performance:"
    for exam_file in "$QUESTIONS_DIR"/*.txt; do
        exam=$(basename "$exam_file" .txt)
        count=$(grep -c ":$exam:" "$RESULTS_DB")
        if (( count > 0 )); then
            avg=$(grep ":$exam:" "$RESULTS_DB" | awk -F: '{split($4,pct,"%"); sum+=pct[1]} END {if(NR>0) print int(sum/NR); else print 0}')
            echo "- ${exam}: $count attempts, $avg% average"
        fi
    done
    read -p "Press Enter to continue..."
    admin_dashboard
}

# --- Admin: Change Password (Session Only) ---
function change_admin_pass {
    clear
    read -s -p "Enter new admin password: " new_pass; echo
    read -s -p "Confirm new password: " new_pass2; echo
    if [[ "$new_pass" == "$new_pass2" && -n "$new_pass" ]]; then
        ADMIN_PASS="$new_pass"
        echo -e "${GREEN}Password changed successfully! (Session only)${NC}"
    else
        echo -e "${RED}Passwords don't match or empty!${NC}"
    fi
    sleep 2
    admin_dashboard
}

# --- Start the system ---
main_menu
