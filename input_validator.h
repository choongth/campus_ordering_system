#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "data_parser.h"
#include "setting.h"
#include "enums.h"

// Validation result structure
typedef struct {
    int is_valid;
    char error_message[256];
    int error_code;
} ValidationResult;

// Error codes
#define VALIDATION_SUCCESS 0
#define VALIDATION_ERROR_EMPTY 1
#define VALIDATION_ERROR_TOO_LONG 2
#define VALIDATION_ERROR_TOO_SHORT 3
#define VALIDATION_ERROR_INVALID_FORMAT 4
#define VALIDATION_ERROR_INVALID_RANGE 5
#define VALIDATION_ERROR_ALREADY_EXISTS 6

// Standardized validation functions
ValidationResult validate_name(const char *name);
ValidationResult validate_contact_number(const char *contact);
ValidationResult validate_email_address(const char *email);
ValidationResult validate_password(const char *password);
ValidationResult validate_student_id(const char *student_id);
ValidationResult validate_restaurant_id(const char *restaurant_id);
ValidationResult validate_user_id(const char *user_id);
ValidationResult validate_price(float price);
ValidationResult validate_balance(float balance);
ValidationResult validate_date(const char *date);
ValidationResult validate_time(const char *time);
ValidationResult validate_cuisine_type(int cuisine);
ValidationResult validate_user_role(int role);

// Composite validation functions
ValidationResult validate_user_registration_data(const char *name, const char *contact, 
                                               const char *email, const char *password);
ValidationResult validate_student_registration_data(const char *name, const char *contact,
                                                   const char *email, const char *password);

// Unique constraint validation (checks against files)
ValidationResult validate_email_uniqueness(const char *email, const char *exclude_email);
ValidationResult validate_contact_uniqueness(const char *contact, const char *exclude_contact);

// Input sanitization functions
void sanitize_name(char *name);
void sanitize_email(char *email);
void sanitize_contact(char *contact);

// Validation helper functions
int is_valid_id_format(const char *id, char prefix, int expected_length);
int is_valid_date_format(const char *date);
int is_valid_time_format(const char *time);
int contains_only_digits(const char *str);
int contains_only_alphanumeric(const char *str);

// Display validation error
void display_validation_error(const ValidationResult *result);

// Create validation result
ValidationResult create_validation_result(int is_valid, int error_code, const char *message);

#endif