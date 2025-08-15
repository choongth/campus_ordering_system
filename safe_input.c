#include "safe_input.h"
#include "setting.h"

// Clear input buffer safely
void clear_input_buffer_safe(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Trim leading and trailing whitespace
void trim_whitespace(char *str) {
    char *end;
    
    // Trim leading space
    while (isspace((unsigned char)*str)) str++;
    
    // All spaces?
    if (*str == 0) return;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    // Write new null terminator
    end[1] = '\0';
}

// Safe string input with length validation
int safe_string_input(const char *prompt, char *buffer, int max_length) {
    if (!prompt || !buffer || max_length <= 0) {
        return 0; // Invalid parameters
    }
    
    printf("%s", prompt);
    
    if (fgets(buffer, max_length, stdin) == NULL) {
        return 0; // Input error
    }
    
    // Remove newline if present
    buffer[strcspn(buffer, "\n")] = '\0';
    
    // Check if input was truncated (buffer full but no newline)
    if (strlen(buffer) == max_length - 1 && buffer[max_length - 2] != '\n') {
        clear_input_buffer_safe(); // Clear remaining input
        printf("Input too long. Maximum %d characters allowed.\n", max_length - 1);
        return 0;
    }
    
    trim_whitespace(buffer);
    
    // Check if empty after trimming
    if (strlen(buffer) == 0) {
        printf("Input cannot be empty.\n");
        return 0;
    }
    
    return 1; // Success
}

// Safe password input with validation
int safe_password_input(const char *prompt, char *buffer, int max_length) {
    if (!safe_string_input(prompt, buffer, max_length)) {
        return 0;
    }
    
    if (!is_valid_password(buffer)) {
        return 0;
    }
    
    return 1;
}

// Safe email input with validation
int safe_email_input(const char *prompt, char *buffer, int max_length) {
    if (!safe_string_input(prompt, buffer, max_length)) {
        return 0;
    }
    
    if (!is_valid_email(buffer)) {
        return 0;
    }
    
    return 1;
}

// Safe contact input with validation
int safe_contact_input(const char *prompt, char *buffer, int max_length) {
    if (!safe_string_input(prompt, buffer, max_length)) {
        return 0;
    }
    
    if (!is_valid_contact(buffer)) {
        return 0;
    }
    
    return 1;
}

// Safe filename input with validation
int safe_filename_input(const char *prompt, char *buffer, int max_length) {
    if (!safe_string_input(prompt, buffer, max_length)) {
        return 0;
    }
    
    if (!is_valid_filename(buffer)) {
        return 0;
    }
    
    return 1;
}

// Validate email format
int is_valid_email(const char *email) {
    if (!email || strlen(email) == 0) {
        printf("Email cannot be empty.\n");
        return 0;
    }
    
    if (strlen(email) >= EMAIL_ADDRESS_LENGTH) {
        printf("Email too long. Maximum %d characters allowed.\n", EMAIL_ADDRESS_LENGTH - 1);
        return 0;
    }
    
    // Check for @ symbol
    char *at_pos = strchr(email, '@');
    if (!at_pos) {
        printf("Invalid email format. Must contain '@' symbol.\n");
        return 0;
    }
    
    // Check for at least one character before and after @
    if (at_pos == email || *(at_pos + 1) == '\0') {
        printf("Invalid email format.\n");
        return 0;
    }
    
    // Check for dot after @
    if (!strchr(at_pos + 1, '.')) {
        printf("Invalid email format. Must contain domain extension.\n");
        return 0;
    }
    
    return 1;
}

// Validate contact number
int is_valid_contact(const char *contact) {
    if (!contact || strlen(contact) == 0) {
        printf("Contact number cannot be empty.\n");
        return 0;
    }
    
    int len = strlen(contact);
    if (len < 10 || len > 11) {
        printf("Contact number must be 10-11 digits.\n");
        return 0;
    }
    
    // Check if all characters are digits
    for (int i = 0; i < len; i++) {
        if (!isdigit(contact[i])) {
            printf("Contact number must contain only digits.\n");
            return 0;
        }
    }
    
    return 1;
}

// Validate filename (alphanumeric, underscore, hyphen, dot)
int is_valid_filename(const char *filename) {
    if (!filename || strlen(filename) == 0) {
        printf("Filename cannot be empty.\n");
        return 0;
    }
    
    int len = strlen(filename);
    if (len > 100) {
        printf("Filename too long. Maximum 100 characters allowed.\n");
        return 0;
    }
    
    // Check for valid characters
    for (int i = 0; i < len; i++) {
        char c = filename[i];
        if (!isalnum(c) && c != '_' && c != '-' && c != '.') {
            printf("Filename can only contain letters, numbers, underscore, hyphen, and dot.\n");
            return 0;
        }
    }
    
    return 1;
}

// Validate password
int is_valid_password(const char *password) {
    if (!password || strlen(password) == 0) {
        printf("Password cannot be empty.\n");
        return 0;
    }
    
    int len = strlen(password);
    if (len < 6 || len >= PASSWORD_LENGTH) {
        printf("Password must be 6-%d characters.\n", PASSWORD_LENGTH - 1);
        return 0;
    }
    
    return 1;
}

// Safe integer input after string input (prevents buffer issues)
int safe_integer_input_after_string(const char *prompt) {
    char buffer[100];
    int value;
    char extra;
    
    // Clear any remaining input buffer first
    fflush(stdout);
    
    for (int attempt = 0; attempt < 5; attempt++) {
        printf("%s", prompt);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error. Please try again.\n");
            continue;
        }
        
        // Check if input was truncated
        if (strlen(buffer) == sizeof(buffer) - 1 && buffer[sizeof(buffer) - 2] != '\n') {
            clear_input_buffer_safe();
            printf("Input too long. Please enter a shorter number.\n");
            continue;
        }
        
        if (sscanf(buffer, "%d %c", &value, &extra) == 1) {
            return value;
        }
        
        printf("Invalid input. Please enter a valid number.\n");
    }
    
    printf("Too many invalid attempts. Returning to previous menu.\n");
    return 0; // Default safe value
}

// Safe exit choice input (commonly used pattern)
int safe_exit_choice_input(void) {
    char buffer[100];
    int value;
    char extra;
    
    // Clear any remaining input buffer first - more aggressive approach
    fflush(stdout);
    fflush(stdin);  // Try to flush input buffer too
    
    for (int attempt = 0; attempt < 5; attempt++) {
        printf("\nEnter 0 to exit: ");
        fflush(stdout);  // Ensure prompt is displayed
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Input error. Please try again.\n");
            continue;
        }
        
        // Debug: Let's see what we actually read
        // printf("DEBUG: Read buffer: '%s' (length: %d)\n", buffer, (int)strlen(buffer));
        
        // Check if we got an empty line or just newline
        if (strlen(buffer) <= 1) {
            printf("Invalid input! Please enter 0 to exit.\n");
            continue;
        }
        
        // Check if input was truncated
        if (strlen(buffer) == sizeof(buffer) - 1 && buffer[sizeof(buffer) - 2] != '\n') {
            clear_input_buffer_safe();
            printf("Input too long. Please enter 0.\n");
            continue;
        }
        
        if (sscanf(buffer, "%d %c", &value, &extra) == 1) {
            if (value == 0) {
                return 1; // User wants to exit
            } else {
                printf("Invalid input! Please enter 0 to exit.\n");
                continue;
            }
        }
        
        printf("Invalid input! Please enter 0 to exit.\n");
    }
    
    printf("Too many invalid attempts. Exiting automatically.\n");
    return 1; // Force exit for safety
}