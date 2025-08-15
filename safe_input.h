#ifndef SAFE_INPUT_H
#define SAFE_INPUT_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Safe input functions to prevent buffer overflows
int safe_string_input(const char *prompt, char *buffer, int max_length);
int safe_password_input(const char *prompt, char *buffer, int max_length);
int safe_email_input(const char *prompt, char *buffer, int max_length);
int safe_contact_input(const char *prompt, char *buffer, int max_length);
int safe_filename_input(const char *prompt, char *buffer, int max_length);

// Safe integer input after string input (prevents buffer issues)
int safe_integer_input_after_string(const char *prompt);

// Safe exit choice input (commonly used pattern)
int safe_exit_choice_input(void);

// Input validation functions
int is_valid_email(const char *email);
int is_valid_contact(const char *contact);
int is_valid_filename(const char *filename);
int is_valid_password(const char *password);

// Utility functions
void trim_whitespace(char *str);
void clear_input_buffer_safe(void);

#endif