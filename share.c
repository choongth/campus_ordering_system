#include "share.h"

// LOGOUT FUNCTION
int logout() {
    printf("Logging out...\n");
    Sleep(1000); // sleep for 1000 milliseconds = 1 second
    printf("You have been logged out successfully!\n");
    return -1;
}

// CONFIRMATION FUNCTION - Fixed infinite loop issue
int confirmation(const char *prompt) {
    // Use safe input loop with maximum 5 attempts
    for (int attempt = 0; attempt < 5; attempt++) {
        int confirm = get_integer_input(prompt);
        switch (confirm) {
            case 1: // YES
                return 1;
            case 2: // NO
                return 2;
            default:
                if (attempt < 4) {
                    printf("\nInvalid input! Please enter 1 for 'Yes' or 2 for 'No'.\n");
                }
                break;
        }
    }
    
    printf("Too many invalid attempts. Defaulting to 'No'.\n");
    return 2; // Default to NO for safety
}

// EXIT FUNCTION
int ask_exit() {
    int exit = get_integer_input("\nExit? (1. Yes / 2. No): ");
    switch (exit) {
        case YES:
            return YES;
        case NO:
            return NO;
        default:
            printf("\nInvalid input! Please enter 1 for 'Yes' or 2 for 'No'.");
            return 0;
    }
}

// GET INTEGER INPUT FUNCTION
int get_integer_input(const char *prompt) {
    char buffer[100];
    int value;
    char extra;
    
    // Use safe loop with maximum 50 attempts to prevent infinite loops
    LoopCounter *counter = init_loop_counter("get_integer_input", 50);
    if (!counter) {
        printf("System error: Could not initialize input loop.\n");
        return 0;
    }
    
    while (loop_should_continue(counter)) {
        printf("%s", prompt);
        
        // Clear any remaining input in buffer first
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error. Please try again.\n");
            continue;
        }
        
        // Check if input was truncated (buffer full but no newline)
        if (strlen(buffer) == sizeof(buffer) - 1 && buffer[sizeof(buffer) - 2] != '\n') {
            clear_input_buffer_safe(); // Clear remaining input
            printf("Input too long. Please enter a shorter number.\n");
            continue;
        }
        
        if (sscanf(buffer, "%d %c", &value, &extra) == 1) {
            free_loop_counter(counter);
            return value;
        }
        
        printf("\nInvalid input. Please enter a valid number.\n");
    }
    
    // If reach here, max attempts exceeded
    free_loop_counter(counter);
    emergency_loop_break("Too many invalid input attempts in get_integer_input");
    return 0; // Return default value
}

bool get_float_input(const char *str) {
    bool has_decimal = false;
    bool has_digit = false;

    if (*str == '\0') return false;

    while (*str) {
        if (isdigit(*str)) {
            has_digit = true;
        } else if (*str == '.') {
            if (has_decimal) return false;
            has_decimal = true;
        } else {
            return false;
        }
        str++;
    }

    return has_digit;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
