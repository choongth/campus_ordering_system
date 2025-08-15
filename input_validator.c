#include "input_validator.h"

// Create validation result
ValidationResult create_validation_result(int is_valid, int error_code, const char *message) {
    ValidationResult result;
    result.is_valid = is_valid;
    result.error_code = error_code;
    
    if (message) {
        strncpy(result.error_message, message, sizeof(result.error_message) - 1);
        result.error_message[sizeof(result.error_message) - 1] = '\0';
    } else {
        result.error_message[0] = '\0';
    }
    
    return result;
}

// Validate name
ValidationResult validate_name(const char *name) {
    if (!name) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Name cannot be null");
    }
    
    if (strlen(name) == 0) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Name cannot be empty");
    }
    
    if (strlen(name) >= NAME_LENGTH) {
        return create_validation_result(0, VALIDATION_ERROR_TOO_LONG, 
                                      "Name is too long (maximum 49 characters)");
    }
    
    // Check for valid characters (letters, spaces, hyphens, apostrophes)
    for (int i = 0; name[i]; i++) {
        char c = name[i];
        if (!isalpha(c) && c != ' ' && c != '-' && c != '\'') {
            return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                          "Name can only contain letters, spaces, hyphens, and apostrophes");
        }
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid name");
}

// Validate contact number
ValidationResult validate_contact_number(const char *contact) {
    if (!contact) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Contact number cannot be null");
    }
    
    if (strlen(contact) == 0) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Contact number cannot be empty");
    }
    
    int len = strlen(contact);
    if (len < 10 || len > 11) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Contact number must be 10-11 digits");
    }
    
    if (!contains_only_digits(contact)) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                      "Contact number must contain only digits");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid contact number");
}

// Validate email address
ValidationResult validate_email_address(const char *email) {
    if (!email) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Email cannot be null");
    }
    
    if (strlen(email) == 0) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Email cannot be empty");
    }
    
    if (strlen(email) >= EMAIL_ADDRESS_LENGTH) {
        return create_validation_result(0, VALIDATION_ERROR_TOO_LONG,
                                      "Email is too long (maximum 39 characters)");
    }
    
    // Check for @ symbol
    char *at_pos = strchr(email, '@');
    if (!at_pos) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                      "Email must contain '@' symbol");
    }
    
    // Check for at least one character before and after @
    if (at_pos == email || *(at_pos + 1) == '\0') {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                      "Email format is invalid");
    }
    
    // Check for dot after @
    if (!strchr(at_pos + 1, '.')) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                      "Email must contain domain extension");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid email address");
}

// Validate password
ValidationResult validate_password(const char *password) {
    if (!password) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Password cannot be null");
    }
    
    if (strlen(password) == 0) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Password cannot be empty");
    }
    
    int len = strlen(password);
    if (len < 6) {
        return create_validation_result(0, VALIDATION_ERROR_TOO_SHORT,
                                      "Password must be at least 6 characters");
    }
    
    if (len >= PASSWORD_LENGTH) {
        return create_validation_result(0, VALIDATION_ERROR_TOO_LONG,
                                      "Password is too long (maximum 20 characters)");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid password");
}

// Validate student ID
ValidationResult validate_student_id(const char *student_id) {
    if (!is_valid_id_format(student_id, 'S', STUDENT_ID_LENGTH)) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                      "Student ID must be in format S000001");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid student ID");
}

// Validate restaurant ID
ValidationResult validate_restaurant_id(const char *restaurant_id) {
    if (!is_valid_id_format(restaurant_id, 'R', RESTAURANT_ID_LENGTH)) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_FORMAT,
                                      "Restaurant ID must be in format R01");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid restaurant ID");
}

// Validate price
ValidationResult validate_price(float price) {
    if (price < 0) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Price cannot be negative");
    }
    
    if (price > 9999.99) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Price is too high (maximum RM9999.99)");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid price");
}

// Validate balance
ValidationResult validate_balance(float balance) {
    if (balance < 0) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Balance cannot be negative");
    }
    
    if (balance > 99999.99) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Balance is too high (maximum RM99999.99)");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid balance");
}

// Validate cuisine type
ValidationResult validate_cuisine_type(int cuisine) {
    if (cuisine < CHINESE || cuisine > DESSERTS) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Invalid cuisine type (must be 1-11)");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid cuisine type");
}

// Validate user role
ValidationResult validate_user_role(int role) {
    if (role < ADMIN || role > DELIVERY_PERSONNEL) {
        return create_validation_result(0, VALIDATION_ERROR_INVALID_RANGE,
                                      "Invalid user role (must be 0-3)");
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Valid user role");
}

// Validate email uniqueness
ValidationResult validate_email_uniqueness(const char *email, const char *exclude_email) {
    if (!email) {
        return create_validation_result(0, VALIDATION_ERROR_EMPTY, "Email cannot be null");
    }
    
    User users[1000];
    int count = read_users_from_file(USER_FILE, users, 1000);
    
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].email_address, email) == 0) {
            // If exclude_email is provided and matches, skip this check
            if (exclude_email && strcmp(exclude_email, email) == 0) {
                continue;
            }
            return create_validation_result(0, VALIDATION_ERROR_ALREADY_EXISTS,
                                          "Email address already exists");
        }
    }
    
    return create_validation_result(1, VALIDATION_SUCCESS, "Email is unique");
}

// Helper function: check if ID format is valid
int is_valid_id_format(const char *id, char prefix, int expected_length) {
    if (!id || strlen(id) != expected_length - 1) {
        return 0;
    }
    
    if (id[0] != prefix) {
        return 0;
    }
    
    // Check if remaining characters are digits
    for (int i = 1; id[i]; i++) {
        if (!isdigit(id[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Helper function: check if string contains only digits
int contains_only_digits(const char *str) {
    if (!str) return 0;
    
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Helper function: check if string contains only alphanumeric characters
int contains_only_alphanumeric(const char *str) {
    if (!str) return 0;
    
    for (int i = 0; str[i]; i++) {
        if (!isalnum(str[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Display validation error
void display_validation_error(const ValidationResult *result) {
    if (!result) return;
    
    if (!result->is_valid) {
        printf("Validation Error: %s\n", result->error_message);
    }
}

// Sanitize name (remove extra spaces, capitalize first letters)
void sanitize_name(char *name) {
    if (!name) return;
    
    // Remove leading/trailing spaces
    char *start = name;
    while (isspace(*start)) start++;
    
    char *end = name + strlen(name) - 1;
    while (end > start && isspace(*end)) end--;
    end[1] = '\0';
    
    // Move sanitized string to beginning
    if (start != name) {
        memmove(name, start, strlen(start) + 1);
    }
    
    // Capitalize first letter of each word
    int capitalize_next = 1;
    for (int i = 0; name[i]; i++) {
        if (isspace(name[i])) {
            capitalize_next = 1;
        } else if (capitalize_next && isalpha(name[i])) {
            name[i] = toupper(name[i]);
            capitalize_next = 0;
        } else if (isalpha(name[i])) {
            name[i] = tolower(name[i]);
        }
    }
}

// Sanitize email (convert to lowercase)
void sanitize_email(char *email) {
    if (!email) return;
    
    for (int i = 0; email[i]; i++) {
        email[i] = tolower(email[i]);
    }
}

// Sanitize contact (remove non-digits)
void sanitize_contact(char *contact) {
    if (!contact) return;
    
    char *src = contact;
    char *dst = contact;
    
    while (*src) {
        if (isdigit(*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}