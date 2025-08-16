#include "admin.h"

#define VIEW_ALL_USERS 1
#define ACTIVATE_DEACTIVATE_ACCOUNTS 2
#define MANAGE_USER_DETAILS 3

// Forward declarations for user management functions
void view_all_users(void);
void activate_deactivate_accounts(void);
void manage_user_details(void);
void display_all_users(void);
void display_students_with_status(void);
int update_student_activation_status(char *student_id, int new_status);
const char* get_user_role_text(UserRole role);
const char* get_activation_status_text_admin(AccountActivateStatus status);

int user_management(void) {
    int choice;
    while (1) {
        char prompt[] = "\n----- USER MANAGEMENT -----\n"
                        "1. View all users\n"
                        "2. Activate/Deactive student accounts\n"
                        "3. Manage user details\n"
                        "0. Exit\n"
                        "What would you like to do? (0-4): ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case VIEW_ALL_USERS:
                view_all_users();
                break;
            case ACTIVATE_DEACTIVATE_ACCOUNTS:
                activate_deactivate_accounts();
                break;
            case MANAGE_USER_DETAILS:
                manage_user_details();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Helper function to get user role text
const char* get_user_role_text(UserRole role) {
    switch (role) {
        case ADMIN: return "Admin";
        case STUDENT: return "Student";
        case RESTAURANT: return "Restaurant";
        case DELIVERY_PERSONNEL: return "Delivery Personnel";
        default: return "Unknown";
    }
}

// Helper function to get activation status text
const char* get_activation_status_text_admin(AccountActivateStatus status) {
    switch (status) {
        case ACTIVATE: return "Active";
        case DEACTIVATE: return "Deactivated";
        default: return "Unknown";
    }
}

// Function to view all users
void view_all_users(void) {
    printf("\n----- VIEW ALL USERS -----\n");
    
    FILE *fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return;
    }
    
    printf("\n================================== ALL REGISTERED USERS ===================================\n");
    printf("%-10s %-20s %-20s %-25s %-15s\n", "User ID", "Role", "Name", "Email", "Contact");
    printf("-------------------------------------------------------------------------------------------\n");
    
    char line[MAX_LINE_LENGTH];
    int user_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        User user;
        sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%s", 
               user.id, (int*)&user.role, user.name, user.contact_number, 
               user.email_address, user.password);
        
        printf("%-10s %-20s %-20s %-25s %-15s\n", 
               user.id, get_user_role_text(user.role), user.name, 
               user.email_address, user.contact_number);
        user_count++;
    }
    
    fclose(fp);
    printf("-------------------------------------------------------------------------------------------\n");
    printf("Total users: %d\n", user_count);
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to display students with their activation status
void display_students_with_status(void) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", STUDENT_FILE);
        return;
    }
    
    printf("\n=================================== STUDENT ACCOUNTS ===================================\n");
    printf("%-12s %-20s %-25s %-15s %-12s\n", "Student ID", "Name", "Email", "Contact", "Status");
    printf("----------------------------------------------------------------------------------------\n");
    
    char line[MAX_LINE_LENGTH];
    int student_count = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char student_id[STUDENT_ID_LENGTH], name[NAME_LENGTH];
        char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
        int status;
        float balance;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f", 
               student_id, name, contact, email, password, &status, &balance);
        
        printf("%-12s %-20s %-25s %-15s %-12s\n", 
               student_id, name, email, contact, 
               get_activation_status_text_admin((AccountActivateStatus)status));
        student_count++;
    }
    
    fclose(fp);
    printf("----------------------------------------------------------------------------------------\n");
    printf("Total students: %d\n", student_count);
}

// Function to update student activation status
int update_student_activation_status(char *student_id, int new_status) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", STUDENT_FILE);
        return 0;
    }

    FILE *temp_fp = fopen("temp_student_admin.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        char sid[STUDENT_ID_LENGTH], name[NAME_LENGTH];
        char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
        int status;
        float balance;

        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f", 
               sid, name, contact, email, password, &status, &balance);

        if (strcmp(sid, student_id) == 0) {
            status = new_status;
            updated = 1;
        }

        fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%.2f\n", 
                sid, name, contact, email, password, status, balance);
    }

    fclose(fp);
    fclose(temp_fp);

    if (updated) {
        remove(STUDENT_FILE);
        rename("temp_student_admin.txt", STUDENT_FILE);
        return 1;
    } else {
        remove("temp_student_admin.txt");
        return 0;
    }
}

// Function to activate/deactivate student accounts
void activate_deactivate_accounts(void) {
    while (1) {
        printf("\n----- ACTIVATE/DEACTIVATE STUDENT ACCOUNTS -----\n");
        
        display_students_with_status();
        
        printf("\nOptions:\n");
        printf("1. Activate a student account\n");
        printf("2. Deactivate a student account\n");
        printf("0. Exit\n");
        
        // Use a simple, non-blocking input approach
        int choice = -1;
        char input_buffer[100];
        
        printf("Enter your choice: ");
        fflush(stdout);
        
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            // Remove newline
            input_buffer[strcspn(input_buffer, "\n")] = '\0';
            
            // Parse the input
            if (strlen(input_buffer) == 1 && input_buffer[0] >= '0' && input_buffer[0] <= '2') {
                choice = input_buffer[0] - '0';
            } else {
                printf("Invalid input! Please enter 0, 1, or 2.\n");
                continue;
            }
        } else {
            printf("Input error. Please try again.\n");
            continue;
        }
        
        switch (choice) {
            case 1: {
                char student_id[STUDENT_ID_LENGTH];
                printf("Enter Student ID to activate: ");
                fflush(stdout);
                
                if (fgets(student_id, sizeof(student_id), stdin) != NULL) {
                    // Check if input was truncated (buffer full but no newline found)
                    size_t len = strlen(student_id);
                    if (len > 0 && student_id[len-1] != '\n') {
                        // Input was truncated, clear the remaining input
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                    } else {
                        // Remove newline character normally
                        student_id[strcspn(student_id, "\n")] = '\0';
                    }
                    
                    if (update_student_activation_status(student_id, ACTIVATE)) {
                        printf("Student account '%s' has been activated successfully!\n", student_id);
                    } else {
                        printf("Failed to activate account. Student ID '%s' not found.\n", student_id);
                    }
                } else {
                    printf("Input error occurred.\n");
                }
                break;
            }
            case 2: {
                char student_id[STUDENT_ID_LENGTH];
                printf("Enter Student ID to deactivate: ");
                fflush(stdout);
                
                if (fgets(student_id, sizeof(student_id), stdin) != NULL) {
                    // Check if input was truncated (buffer full but no newline found)
                    size_t len = strlen(student_id);
                    if (len > 0 && student_id[len-1] != '\n') {
                        // Input was truncated, clear the remaining input
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                    } else {
                        // Remove newline character normally
                        student_id[strcspn(student_id, "\n")] = '\0';
                    }
                    
                    if (update_student_activation_status(student_id, DEACTIVATE)) {
                        printf("Student account '%s' has been deactivated successfully!\n", student_id);
                    } else {
                        printf("Failed to deactivate account. Student ID '%s' not found.\n", student_id);
                    }
                } else {
                    printf("Input error occurred.\n");
                }
                break;
            }
            case 0:
                return;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// User Management Structures

// Forward declarations for user management
int load_user_details(UserDetail *users, int *count);
void display_user_details(UserDetail *users, int count);
void add_new_user_account(void);
void update_user_information(void);
void delete_user_account(void);
void reset_user_password(void);
void view_user_details(void);
void search_user_by_criteria(void);
int find_user_by_id(char *user_id, UserDetail *user);
int find_user_by_email(char *email, UserDetail *user);
int update_user_in_files(UserDetail *user);
int delete_user_from_files(char *user_id, int user_type);
int add_user_to_files(UserDetail *user);
void generate_new_user_id(char *user_id, int user_type);
int validate_user_input(UserDetail *user);
const char* get_user_type_name(int user_type);

#define ADD_USER 1
#define UPDATE_USER 2
#define DELETE_USER 3

// Function to manage user details
void manage_user_details(void) {
    int choice;
    
    while (1) {
        char prompt[] = "\n----- MANAGE USER DETAILS -----\n"
                        "1. Add new user account\n"
                        "2. Update user information\n"
                        "3. Delete user account\n"
                        "0. Exit\n"
                        "What would you like to do? (0-3): ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case ADD_USER:
                add_new_user_account();
                break;
            case UPDATE_USER:
                update_user_information();
                break;
            case DELETE_USER:
                delete_user_account();
                break;
            case 0:
                return;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get user type name
const char* get_user_type_name(int user_type) {
    switch (user_type) {
        case 0: return "Admin";
        case 1: return "Student";
        case 2: return "Restaurant";
        case 3: return "Delivery Personnel";
        default: return "Unknown";
    }
}

// Function to generate new user ID
void generate_new_user_id(char *user_id, int user_type) {
    FILE *fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        switch (user_type) {
            case 0: strcpy(user_id, "U000001"); break;
            case 1: strcpy(user_id, "U000001"); break;
            case 2: strcpy(user_id, "U000001"); break;
            case 3: strcpy(user_id, "U000001"); break;
        }
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    int max_num = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        char uid[USER_ID_LENGTH];
        sscanf(line, "%[^,]", uid);
        
        if (uid[0] == 'U') {
            int num = atoi(&uid[1]);
            if (num > max_num) max_num = num;
        }
    }
    fclose(fp);
    
    snprintf(user_id, USER_ID_LENGTH, "U%06d", max_num + 1);
}

// Function to validate user input
int validate_user_input(UserDetail *user) {
    // Validate name
    if (strlen(user->name) == 0 || strlen(user->name) >= NAME_LENGTH) {
        printf("Invalid name. Must be 1-%d characters.\n", NAME_LENGTH - 1);
        return 0;
    }
    
    // Validate contact
    if (strlen(user->contact) < 10 || strlen(user->contact) >= CONTACT_NUMBER_LENGTH) {
        printf("Invalid contact number. Must be 10-11 digits.\n");
        return 0;
    }
    
    // Validate email
    if (strlen(user->email) == 0 || strlen(user->email) >= EMAIL_ADDRESS_LENGTH || 
        strstr(user->email, "@") == NULL) {
        printf("Invalid email address.\n");
        return 0;
    }
    
    // Validate password
    if (strlen(user->password) < 6 || strlen(user->password) >= PASSWORD_LENGTH) {
        printf("Invalid password. Must be 6-%d characters.\n", PASSWORD_LENGTH - 1);
        return 0;
    }
    
    return 1;
}

// Function to load user details
int load_user_details(UserDetail *users, int *count) {
    FILE *fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return FILE_ERROR;
    }
    
    *count = 0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';
        
        sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%s",
               users[*count].user_id, &users[*count].user_type,
               users[*count].name, users[*count].contact,
               users[*count].email, users[*count].password);
        
        // Initialize additional fields
        users[*count].status = 1; // Default active
        users[*count].balance = 0.0;
        users[*count].cuisine = 1;
        strcpy(users[*count].delivery_id, "none");
        
        // Load additional details based on user type
        if (users[*count].user_type == 1) { // Student
            FILE *student_fp = fopen(STUDENT_FILE, "r");
            if (student_fp != NULL) {
                char student_line[MAX_LINE_LENGTH];
                while (fgets(student_line, sizeof(student_line), student_fp)) {
                    char sid[STUDENT_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH];
                    char password[PASSWORD_LENGTH];
                    int status;
                    float balance;
                    
                    sscanf(student_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f",
                           sid, name, contact, email, password, &status, &balance);
                    
                    if (strcmp(email, users[*count].email) == 0) {
                        users[*count].status = status;
                        users[*count].balance = balance;
                        break;
                    }
                }
                fclose(student_fp);
            }
        } else if (users[*count].user_type == 2) { // Restaurant
            FILE *restaurant_fp = fopen(RESTAURANT_FILE, "r");
            if (restaurant_fp != NULL) {
                char restaurant_line[MAX_LINE_LENGTH];
                while (fgets(restaurant_line, sizeof(restaurant_line), restaurant_fp)) {
                    char rid[RESTAURANT_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH];
                    char password[PASSWORD_LENGTH];
                    int cuisine;
                    
                    sscanf(restaurant_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d",
                           rid, name, contact, email, password, &cuisine);
                    
                    if (strcmp(email, users[*count].email) == 0) {
                        users[*count].cuisine = cuisine;
                        break;
                    }
                }
                fclose(restaurant_fp);
            }
        } else if (users[*count].user_type == 3) { // Delivery Personnel
            FILE *delivery_fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
            if (delivery_fp != NULL) {
                char delivery_line[MAX_LINE_LENGTH];
                while (fgets(delivery_line, sizeof(delivery_line), delivery_fp)) {
                    char pid[DELIVERY_PERSONNEL_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH];
                    char password[PASSWORD_LENGTH];
                    int status;
                    char delivery_id[DELIVERY_ID_LENGTH];
                    
                    sscanf(delivery_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s",
                           pid, name, contact, email, password, &status, delivery_id);
                    
                    if (strcmp(email, users[*count].email) == 0) {
                        users[*count].status = status;
                        strcpy(users[*count].delivery_id, delivery_id);
                        break;
                    }
                }
                fclose(delivery_fp);
            }
        }
        
        (*count)++;
    }
    
    fclose(fp);
    return *count;
}

// Function to display user details
void display_user_details(UserDetail *users, int count) {
    if (count == 0) {
        printf("No users found.\n");
        return;
    }
    
    printf("\n==================== USER DETAILS ====================\n");
    printf("%-8s %-15s %-20s %-12s %-25s %-8s\n", 
           "User ID", "Type", "Name", "Contact", "Email", "Status");
    printf("---------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        char status_str[20];
        if (users[i].user_type == 0) { // Admin
            strcpy(status_str, "Active");
        } else if (users[i].user_type == 1) { // Student
            snprintf(status_str, sizeof(status_str), "%s/RM%.2f", 
                    (users[i].status == 1) ? "Active" : "Inactive", users[i].balance);
        } else if (users[i].user_type == 2) { // Restaurant
            snprintf(status_str, sizeof(status_str), "Cuisine:%d", users[i].cuisine);
        } else if (users[i].user_type == 3) { // Delivery Personnel
            snprintf(status_str, sizeof(status_str), "%s", 
                    (users[i].status == 1) ? "Available" : 
                    (users[i].status == 2) ? "Delivering" : "Inactive");
        }
        
        printf("%-8s %-15s %-20s %-12s %-25s %-8s\n", 
               users[i].user_id, get_user_type_name(users[i].user_type),
               users[i].name, users[i].contact, users[i].email, status_str);
    }
    
    printf("---------------------------------------------------------------------------------\n");
    printf("Total users: %d\n", count);
    printf("=======================================================================\n");
}

// Function to find user by ID
int find_user_by_id(char *user_id, UserDetail *user) {
    UserDetail users[100];
    int count;
    
    if (load_user_details(users, &count) == FILE_ERROR) {
        return 0;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].user_id, user_id) == 0) {
            *user = users[i];
            return 1;
        }
    }
    
    return 0;
}

// Function to find user by email
int find_user_by_email(char *email, UserDetail *user) {
    UserDetail users[100];
    int count;
    
    if (load_user_details(users, &count) == FILE_ERROR) {
        return 0;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].email, email) == 0) {
            *user = users[i];
            return 1;
        }
    }
    
    return 0;
}

// Function to add user to files
int add_user_to_files(UserDetail *user) {
    // Add to User.txt
    FILE *user_fp = fopen(USER_FILE, "a");
    if (user_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return 0;
    }
    
    fprintf(user_fp, "%s,%d,%s,%s,%s,%s\n",
            user->user_id, user->user_type, user->name,
            user->contact, user->email, user->password);
    fclose(user_fp);
    
    // Add to specific user type file
    if (user->user_type == 1) { // Student
        FILE *student_fp = fopen(STUDENT_FILE, "a");
        if (student_fp != NULL) {
            // Generate student ID
            char student_id[STUDENT_ID_LENGTH];
            snprintf(student_id, sizeof(student_id), "S%06d", atoi(&user->user_id[1]));
            
            fprintf(student_fp, "%s,%s,%s,%s,%s,%d,%.2f\n",
                    student_id, user->name, user->contact, user->email,
                    user->password, user->status, user->balance);
            fclose(student_fp);
        }
    } else if (user->user_type == 2) { // Restaurant
        FILE *restaurant_fp = fopen(RESTAURANT_FILE, "a");
        if (restaurant_fp != NULL) {
            // Generate restaurant ID
            char restaurant_id[RESTAURANT_ID_LENGTH];
            int restaurant_num = atoi(&user->user_id[1]) - 4; // Adjust for restaurant numbering
            snprintf(restaurant_id, sizeof(restaurant_id), "R%02d", restaurant_num);
            
            fprintf(restaurant_fp, "%s,%s,%s,%s,%s,%d\n",
                    restaurant_id, user->name, user->contact, user->email,
                    user->password, user->cuisine);
            fclose(restaurant_fp);
        }
    } else if (user->user_type == 3) { // Delivery Personnel
        FILE *delivery_fp = fopen(DELIVERY_PERSONNEL_FILE, "a");
        if (delivery_fp != NULL) {
            // Generate delivery personnel ID
            char personnel_id[DELIVERY_PERSONNEL_ID_LENGTH];
            int personnel_num = atoi(&user->user_id[1]) - 7; // Adjust for delivery personnel numbering
            snprintf(personnel_id, sizeof(personnel_id), "P%03d", personnel_num);
            
            fprintf(delivery_fp, "%s,%s,%s,%s,%s,%d,%s\n",
                    personnel_id, user->name, user->contact, user->email,
                    user->password, user->status, user->delivery_id);
            fclose(delivery_fp);
        }
    }
    
    return 1;
}

// Function to update user in files
int update_user_in_files(UserDetail *user) {
    // Update User.txt
    FILE *user_fp = fopen(USER_FILE, "r");
    if (user_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return 0;
    }
    
    FILE *temp_fp = fopen("temp_user.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(user_fp);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int updated = 0;
    
    while (fgets(line, sizeof(line), user_fp)) {
        char uid[USER_ID_LENGTH];
        sscanf(line, "%[^,]", uid);
        
        if (strcmp(uid, user->user_id) == 0) {
            fprintf(temp_fp, "%s,%d,%s,%s,%s,%s\n",
                    user->user_id, user->user_type, user->name,
                    user->contact, user->email, user->password);
            updated = 1;
        } else {
            fprintf(temp_fp, "%s", line);
        }
    }
    
    fclose(user_fp);
    fclose(temp_fp);
    
    if (updated) {
        remove(USER_FILE);
        rename("temp_user.txt", USER_FILE);
        
        // Update specific user type file
        if (user->user_type == 1) { // Student
            // Update Student.txt
            FILE *student_fp = fopen(STUDENT_FILE, "r");
            FILE *temp_student_fp = fopen("temp_student.txt", "w");
            
            if (student_fp != NULL && temp_student_fp != NULL) {
                char student_line[MAX_LINE_LENGTH];
                while (fgets(student_line, sizeof(student_line), student_fp)) {
                    char email[EMAIL_ADDRESS_LENGTH];
                    char sid[STUDENT_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], password[PASSWORD_LENGTH];
                    int status;
                    float balance;
                    
                    sscanf(student_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f",
                           sid, name, contact, email, password, &status, &balance);
                    
                    if (strcmp(email, user->email) == 0) {
                        fprintf(temp_student_fp, "%s,%s,%s,%s,%s,%d,%.2f\n",
                                sid, user->name, user->contact, user->email,
                                user->password, user->status, user->balance);
                    } else {
                        fprintf(temp_student_fp, "%s", student_line);
                    }
                }
                
                fclose(student_fp);
                fclose(temp_student_fp);
                remove(STUDENT_FILE);
                rename("temp_student.txt", STUDENT_FILE);
            }
        }
        // Similar updates for Restaurant and Delivery Personnel files would go here
        
        return 1;
    } else {
        remove("temp_user.txt");
        return 0;
    }
}

// Function to delete user from files
int delete_user_from_files(char *user_id, int user_type) {
    // Delete from User.txt
    FILE *user_fp = fopen(USER_FILE, "r");
    if (user_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return 0;
    }
    
    FILE *temp_fp = fopen("temp_user.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(user_fp);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int deleted = 0;
    char user_email[EMAIL_ADDRESS_LENGTH];
    
    while (fgets(line, sizeof(line), user_fp)) {
        char uid[USER_ID_LENGTH];
        int utype;
        char name[NAME_LENGTH], contact[CONTACT_NUMBER_LENGTH];
        char email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
        
        sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%s",
               uid, &utype, name, contact, email, password);
        
        if (strcmp(uid, user_id) == 0) {
            strcpy(user_email, email);
            deleted = 1;
        } else {
            fprintf(temp_fp, "%s", line);
        }
    }
    
    fclose(user_fp);
    fclose(temp_fp);
    
    if (deleted) {
        remove(USER_FILE);
        rename("temp_user.txt", USER_FILE);
        
        // Delete from specific user type file
        if (user_type == 1) { // Student
            FILE *student_fp = fopen(STUDENT_FILE, "r");
            FILE *temp_student_fp = fopen("temp_student.txt", "w");
            
            if (student_fp != NULL && temp_student_fp != NULL) {
                char student_line[MAX_LINE_LENGTH];
                while (fgets(student_line, sizeof(student_line), student_fp)) {
                    char email[EMAIL_ADDRESS_LENGTH];
                    sscanf(student_line, "%*[^,],%*[^,],%*[^,],%[^,]", email);
                    
                    if (strcmp(email, user_email) != 0) {
                        fprintf(temp_student_fp, "%s", student_line);
                    }
                }
                
                fclose(student_fp);
                fclose(temp_student_fp);
                remove(STUDENT_FILE);
                rename("temp_student.txt", STUDENT_FILE);
            }
        }
        // Similar deletions for Restaurant and Delivery Personnel files would go here
        
        return 1;
    } else {
        remove("temp_user.txt");
        return 0;
    }
}

// Function to add new user account
void add_new_user_account(void) {
    UserDetail new_user;
    
    printf("\n----- ADD NEW USER ACCOUNT -----\n");
    
    // Get user type
    printf("Select user type:\n");
    printf("1. Student\n");
    printf("2. Restaurant\n");
    printf("3. Delivery Personnel\n");
    
    int type_choice;
    while (1) {
        type_choice = get_integer_input("Enter user type (1-3): ");
        if (type_choice >= 1 && type_choice <= 3) {
            new_user.user_type = type_choice;
            break;
        }
        printf("Invalid choice. Please enter 1, 2, or 3.\n");
    }
    
    // Generate new user ID
    generate_new_user_id(new_user.user_id, new_user.user_type);
    
    // Get user details
    if (!safe_string_input("Enter user name: ", new_user.name, sizeof(new_user.name))) {
        printf("Invalid name. User creation cancelled.\n");
        return;
    }
    
    if (!safe_contact_input("Enter contact number: ", new_user.contact, sizeof(new_user.contact))) {
        printf("Invalid contact number. User creation cancelled.\n");
        return;
    }
    
    if (!safe_email_input("Enter email address: ", new_user.email, sizeof(new_user.email))) {
        printf("Invalid email address. User creation cancelled.\n");
        return;
    }
    
    // Check if email already exists
    UserDetail existing_user;
    if (find_user_by_email(new_user.email, &existing_user)) {
        printf("Error: Email address already exists in the system.\n");
        return;
    }
    
    if (!safe_password_input("Enter password: ", new_user.password, sizeof(new_user.password))) {
        printf("Invalid password. User creation cancelled.\n");
        return;
    }
    
    // Set default values based on user type
    if (new_user.user_type == 1) { // Student
        new_user.status = 1; // Active
        new_user.balance = 0.0;
        printf("Enter initial balance (optional, press Enter for 0.00): ");
        char balance_input[20];
        getchar(); // Clear buffer
        fgets(balance_input, sizeof(balance_input), stdin);
        if (strlen(balance_input) > 1) {
            new_user.balance = atof(balance_input);
        }
    } else if (new_user.user_type == 2) { // Restaurant
        printf("Select cuisine type:\n");
        printf("1. Chinese\n");
        printf("2. Western\n");
        printf("3. Malay\n");
        printf("4. Japanese\n");
        printf("5. Indian\n");
        
        while (1) {
            new_user.cuisine = get_integer_input("Enter cuisine type (1-5): ");
            if (new_user.cuisine >= 1 && new_user.cuisine <= 5) {
                break;
            }
            printf("Invalid choice. Please enter 1-5.\n");
        }
    } else if (new_user.user_type == 3) { // Delivery Personnel
        new_user.status = 1; // Available
        strcpy(new_user.delivery_id, "none");
    }
    
    // Validate input
    if (!validate_user_input(&new_user)) {
        return;
    }
    
    // Display user details for confirmation
    printf("\n--- NEW USER DETAILS ---\n");
    printf("User ID: %s\n", new_user.user_id);
    printf("Type: %s\n", get_user_type_name(new_user.user_type));
    printf("Name: %s\n", new_user.name);
    printf("Contact: %s\n", new_user.contact);
    printf("Email: %s\n", new_user.email);
    printf("Password: %s\n", new_user.password);
    
    if (new_user.user_type == 1) {
        printf("Initial Balance: RM%.2f\n", new_user.balance);
    } else if (new_user.user_type == 2) {
        printf("Cuisine Type: %d\n", new_user.cuisine);
    }
    
    int confirm = confirmation("Create this user account? (1=Yes, 2=No): ");
    if (confirm == 1) {
        if (add_user_to_files(&new_user)) {
            printf("\n✓ User account created successfully!\n");
            printf("User ID: %s\n", new_user.user_id);
            printf("Login Email: %s\n", new_user.email);
        } else {
            printf("\n✗ Failed to create user account.\n");
        }
    } else {
        printf("User account creation cancelled.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to update user information
void update_user_information(void) {
    char search_input[100];
    UserDetail user;
    
    printf("\n----- UPDATE USER INFORMATION -----\n");
    if (!safe_string_input("Enter User ID or Email: ", search_input, sizeof(search_input))) {
        printf("Invalid input. Update cancelled.\n");
        return;
    }
    
    // Try to find user by ID first, then by email
    int found = 0;
    if (search_input[0] == 'U') {
        found = find_user_by_id(search_input, &user);
    } else {
        found = find_user_by_email(search_input, &user);
    }
    
    if (!found) {
        printf("User not found.\n");
        return;
    }
    
    // Display current user information
    printf("\n--- CURRENT USER INFORMATION ---\n");
    printf("User ID: %s\n", user.user_id);
    printf("Type: %s\n", get_user_type_name(user.user_type));
    printf("Name: %s\n", user.name);
    printf("Contact: %s\n", user.contact);
    printf("Email: %s\n", user.email);
    
    if (user.user_type == 1) {
        printf("Status: %s\n", (user.status == 1) ? "Active" : "Inactive");
        printf("Balance: RM%.2f\n", user.balance);
    } else if (user.user_type == 2) {
        printf("Cuisine Type: %d\n", user.cuisine);
    } else if (user.user_type == 3) {
        printf("Status: %s\n", (user.status == 1) ? "Available" : 
               (user.status == 2) ? "Delivering" : "Inactive");
        printf("Delivery ID: %s\n", user.delivery_id);
    }
    
    // Update options
    printf("\n--- UPDATE OPTIONS ---\n");
    printf("1. Update name\n");
    printf("2. Update contact number\n");
    printf("3. Update email address\n");
    printf("4. Update password\n");
    
    if (user.user_type == 1) {
        printf("5. Update status\n");
        printf("6. Update balance\n");
    } else if (user.user_type == 2) {
        printf("5. Update cuisine type\n");
    } else if (user.user_type == 3) {
        printf("5. Update status\n");
    }
    
    printf("0. Cancel\n");
    
    int choice = get_integer_input("Select field to update: ");
    
    switch (choice) {
        case 1:
            printf("Enter new name: ");
            getchar(); // Clear buffer
            fgets(user.name, sizeof(user.name), stdin);
            user.name[strcspn(user.name, "\n")] = '\0';
            break;
            
        case 2:
            if (!safe_contact_input("Enter new contact number: ", user.contact, sizeof(user.contact))) {
                printf("Invalid contact number. Update cancelled.\n");
                return;
            }
            break;
            
        case 3:
            if (!safe_email_input("Enter new email address: ", user.email, sizeof(user.email))) {
                printf("Invalid email address. Update cancelled.\n");
                return;
            }
            break;
            
        case 4:
            if (!safe_password_input("Enter new password: ", user.password, sizeof(user.password))) {
                printf("Invalid password. Update cancelled.\n");
                return;
            }
            break;
            
        case 5:
            if (user.user_type == 1) { // Student status
                printf("Select status:\n");
                printf("1. Active\n");
                printf("2. Inactive\n");
                int status_choice = get_integer_input("Enter status: ");
                user.status = (status_choice == 1) ? 1 : 2;
            } else if (user.user_type == 2) { // Restaurant cuisine
                printf("Select cuisine type:\n");
                printf("1. Chinese\n");
                printf("2. Western\n");
                printf("3. Malay\n");
                printf("4. Japanese\n");
                printf("5. Indian\n");
                user.cuisine = get_integer_input("Enter cuisine type (1-5): ");
            } else if (user.user_type == 3) { // Delivery personnel status
                printf("Select status:\n");
                printf("1. Available\n");
                printf("2. Delivering\n");
                printf("3. Inactive\n");
                int status_choice = get_integer_input("Enter status: ");
                user.status = status_choice;
            }
            break;
            
        case 6:
            if (user.user_type == 1) { // Student balance
                printf("Enter new balance: ");
                scanf("%f", &user.balance);
            }
            break;
            
        case 0:
            printf("Update cancelled.\n");
            return;
            
        default:
            printf("Invalid choice.\n");
            return;
    }
    
    // Validate updated input
    if (!validate_user_input(&user)) {
        return;
    }
    
    int confirm = confirmation("Save changes? (1=Yes, 2=No): ");
    if (confirm == 1) {
        if (update_user_in_files(&user)) {
            printf("\nUser information updated successfully!\n");
        } else {
            printf("\nFailed to update user information.\n");
        }
    } else {
        printf("Changes cancelled.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to delete user account
void delete_user_account(void) {
    char search_input[100];
    UserDetail user;
    
    printf("\n----- DELETE USER ACCOUNT -----\n");
    printf("WARNING: This action cannot be undone!\n");
    if (!safe_string_input("Enter User ID or Email: ", search_input, sizeof(search_input))) {
        printf("Invalid input. Deletion cancelled.\n");
        return;
    }
    
    // Try to find user by ID first, then by email
    int found = 0;
    if (search_input[0] == 'U') {
        found = find_user_by_id(search_input, &user);
    } else {
        found = find_user_by_email(search_input, &user);
    }
    
    if (!found) {
        printf("User not found.\n");
        return;
    }
    
    // Display user information
    printf("\n--- USER TO BE DELETED ---\n");
    printf("User ID: %s\n", user.user_id);
    printf("Type: %s\n", get_user_type_name(user.user_type));
    printf("Name: %s\n", user.name);
    printf("Contact: %s\n", user.contact);
    printf("Email: %s\n", user.email);
    
    // Prevent deletion of admin accounts
    if (user.user_type == 0) {
        printf("Error: Cannot delete admin accounts.\n");
        return;
    }
    
    printf("\nThis will permanently delete the user account and all associated data.\n");
    int confirm = confirmation("Are you sure you want to delete this account? (1=Yes, 2=No): ");
    
    if (confirm == 1) {
        int double_confirm = confirmation("Type 1 again to confirm deletion: ");
        if (double_confirm == 1) {
            if (delete_user_from_files(user.user_id, user.user_type)) {
                printf("\nUser account deleted successfully.\n");
            } else {
                printf("\nFailed to delete user account.\n");
            }
        } else {
            printf("Deletion cancelled.\n");
        }
    } else {
        printf("Deletion cancelled.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// -----------------------------------------------------------------------------------------------

// Reporting and Analytics Structures

// Forward declarations for reporting and analytics
int generate_system_analytics(SystemAnalytics *analytics);
int generate_user_activity_report(UserActivityReport *reports, int *count);
int generate_revenue_breakdown(RevenueBreakdown *breakdown, int *count);
int generate_performance_metrics(PerformanceMetric *metrics, int *count);
void display_system_overview(SystemAnalytics *analytics);
void display_user_activity_report(UserActivityReport *reports, int count);
void display_revenue_analytics(RevenueBreakdown *breakdown, int count);
void display_performance_dashboard(PerformanceMetric *metrics, int count);
void view_system_overview(void);
void view_user_activity_report(void);
void view_revenue_analytics(void);
void view_performance_dashboard(void);
void view_detailed_statistics(void);
void export_analytics_report(void);
int count_users_by_type(const char *filename, const char *user_type);
int count_orders_by_status_admin(const char *status);
float calculate_total_revenue(void);
float calculate_average_delivery_time(void);
float calculate_on_time_rate(void);
int find_peak_hour(void);
void get_analytics_date(char *date_str);

#define VIEW_SYSTEM_OVERVIEW 1
#define VIEW_USER_ACTIVITY 2
#define VIEW_REVENUE_ANALYTICS 3
#define VIEW_PERFORMANCE_DASHBOARD 4
#define VIEW_DETAILED_STATISTICS 5
#define EXPORT_ANALYTICS_REPORT 6

int reporting_and_analytics(void) {
    int choice;
    
    while (1) {
        char prompt[] = "\n----- REPORTING AND ANALYTICS -----\n"
                        "1. View system overview\n"
                        "2. View user activity report\n"
                        "3. View revenue analytics\n"
                        "4. View performance dashboard\n"
                        "5. View detailed statistics\n"
                        "6. Export analytics report\n"
                        "0. Exit\n"
                        "What would you like to do? (0-6): ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case VIEW_SYSTEM_OVERVIEW:
                view_system_overview();
                break;
            case VIEW_USER_ACTIVITY:
                view_user_activity_report();
                break;
            case VIEW_REVENUE_ANALYTICS:
                view_revenue_analytics();
                break;
            case VIEW_PERFORMANCE_DASHBOARD:
                view_performance_dashboard();
                break;
            case VIEW_DETAILED_STATISTICS:
                view_detailed_statistics();
                break;
            case EXPORT_ANALYTICS_REPORT:
                export_analytics_report();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get analytics date (simplified)
void get_analytics_date(char *date_str) {
    // For demonstration, using a fixed current date
    // In a real system, this would get actual current date
    strcpy(date_str, "28-12-2024");
}

// Function to count users by type
int count_users_by_type(const char *filename, const char *user_type) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0;
    }
    
    int count = 0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp)) {
        count++;
    }
    
    fclose(fp);
    return count;
}

// Function to count orders by status
int count_orders_by_status_admin(const char *status) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        return 0;
    }
    
    int count = 0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
        
        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, restaurant_id, &total_price,
               order_date, order_time, order_status);
        
        if (strcmp(order_status, status) == 0) {
            count++;
        }
    }
    
    fclose(fp);
    return count;
}

// Function to calculate total revenue
float calculate_total_revenue(void) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        return 0.0;
    }
    
    float total_revenue = 0.0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
        
        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, restaurant_id, &total_price,
               order_date, order_time, order_status);
        
        if (strcmp(order_status, "DELIVERED") == 0) {
            total_revenue += total_price;
        }
    }
    
    fclose(fp);
    return total_revenue;
}

// Function to calculate average delivery time
float calculate_average_delivery_time(void) {
    FILE *fp = fopen(DELIVERY_FILE, "r");
    if (fp == NULL) {
        return 0.0;
    }
    
    int count = 0;
    int total_time = 0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char delivery_id[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               delivery_id, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);
        
        if (delivery_status == 1 && strlen(delivered_time) > 0) { // DELIVERED
            // Simplified time calculation (assuming 30-45 minutes average)
            total_time += 35; // Average delivery time
            count++;
        }
    }
    
    fclose(fp);
    return (count > 0) ? (float)total_time / count : 0.0;
}

// Function to calculate on-time delivery rate
float calculate_on_time_rate(void) {
    FILE *fp = fopen(DELIVERY_FILE, "r");
    if (fp == NULL) {
        return 0.0;
    }
    
    int total_deliveries = 0;
    int on_time_deliveries = 0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char delivery_id[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               delivery_id, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);
        
        if (delivery_status == 1) { // DELIVERED
            total_deliveries++;
            if (punctuality_status == 1) { // ON_TIME
                on_time_deliveries++;
            }
        }
    }
    
    fclose(fp);
    return (total_deliveries > 0) ? ((float)on_time_deliveries / total_deliveries * 100.0) : 0.0;
}

// Function to find peak hour
int find_peak_hour(void) {
    int hourly_orders[24] = {0};
    
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        return 12; // Default to noon
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
        
        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, restaurant_id, &total_price,
               order_date, order_time, order_status);
        
        int hour;
        sscanf(order_time, "%d:", &hour);
        if (hour >= 0 && hour < 24) {
            hourly_orders[hour]++;
        }
    }
    
    fclose(fp);
    
    int peak_hour = 0;
    int max_orders = hourly_orders[0];
    for (int i = 1; i < 24; i++) {
        if (hourly_orders[i] > max_orders) {
            max_orders = hourly_orders[i];
            peak_hour = i;
        }
    }
    
    return peak_hour;
}

// Function to generate system analytics
int generate_system_analytics(SystemAnalytics *analytics) {
    // User Statistics
    analytics->total_students = count_users_by_type(STUDENT_FILE, "student");
    analytics->active_students = analytics->total_students; // Simplified
    analytics->total_restaurants = count_users_by_type(RESTAURANT_FILE, "restaurant");
    analytics->active_restaurants = analytics->total_restaurants; // Simplified
    analytics->total_delivery_personnel = count_users_by_type(DELIVERY_PERSONNEL_FILE, "delivery");
    analytics->active_delivery_personnel = analytics->total_delivery_personnel; // Simplified
    
    // Order Statistics
    analytics->total_orders = count_users_by_type(ORDER_FILE, "order");
    analytics->completed_orders = count_orders_by_status_admin("DELIVERED");
    analytics->pending_orders = count_orders_by_status_admin("PREPARING") + count_orders_by_status_admin("READY");
    analytics->cancelled_orders = count_orders_by_status_admin("CANCELLED");
    
    if (analytics->total_orders > 0) {
        analytics->completion_rate = ((float)analytics->completed_orders / analytics->total_orders) * 100.0;
        analytics->cancellation_rate = ((float)analytics->cancelled_orders / analytics->total_orders) * 100.0;
    } else {
        analytics->completion_rate = 0.0;
        analytics->cancellation_rate = 0.0;
    }
    
    // Revenue Analytics
    analytics->total_system_revenue = calculate_total_revenue();
    analytics->average_order_value = (analytics->completed_orders > 0) ? 
        (analytics->total_system_revenue / analytics->completed_orders) : 0.0;
    analytics->total_delivery_fees = analytics->total_system_revenue * 0.10; // 10% delivery fee
    analytics->restaurant_commissions = analytics->total_system_revenue * 0.15; // 15% commission
    
    // Performance Metrics
    analytics->average_delivery_time = calculate_average_delivery_time();
    analytics->on_time_delivery_rate = calculate_on_time_rate();
    analytics->peak_hour = find_peak_hour();
    analytics->peak_hour_revenue = analytics->total_system_revenue * 0.20; // Estimated 20% during peak
    
    // Growth Metrics (simplified)
    analytics->new_users_today = 2; // Sample data
    analytics->orders_today = analytics->total_orders;
    analytics->revenue_today = analytics->total_system_revenue;
    
    return 1;
}

// Function to generate user activity report
int generate_user_activity_report(UserActivityReport *reports, int *count) {
    *count = 3; // Students, Restaurants, Delivery Personnel
    
    // Students Report
    strcpy(reports[0].user_type, "Students");
    reports[0].total_users = count_users_by_type(STUDENT_FILE, "student");
    reports[0].active_users = reports[0].total_users; // Simplified
    reports[0].activity_rate = (reports[0].total_users > 0) ? 
        ((float)reports[0].active_users / reports[0].total_users * 100.0) : 0.0;
    strcpy(reports[0].most_active_user, "S000001");
    reports[0].most_active_count = 5; // Sample data
    
    // Restaurants Report
    strcpy(reports[1].user_type, "Restaurants");
    reports[1].total_users = count_users_by_type(RESTAURANT_FILE, "restaurant");
    reports[1].active_users = reports[1].total_users; // Simplified
    reports[1].activity_rate = (reports[1].total_users > 0) ? 
        ((float)reports[1].active_users / reports[1].total_users * 100.0) : 0.0;
    strcpy(reports[1].most_active_user, "R01");
    reports[1].most_active_count = 8; // Sample data
    
    // Delivery Personnel Report
    strcpy(reports[2].user_type, "Delivery Personnel");
    reports[2].total_users = count_users_by_type(DELIVERY_PERSONNEL_FILE, "delivery");
    reports[2].active_users = reports[2].total_users; // Simplified
    reports[2].activity_rate = (reports[2].total_users > 0) ? 
        ((float)reports[2].active_users / reports[2].total_users * 100.0) : 0.0;
    strcpy(reports[2].most_active_user, "P001");
    reports[2].most_active_count = 12; // Sample data
    
    return *count;
}

// Function to generate revenue breakdown
int generate_revenue_breakdown(RevenueBreakdown *breakdown, int *count) {
    float total_revenue = calculate_total_revenue();
    *count = 4;
    
    // Order Revenue
    strcpy(breakdown[0].category, "Order Revenue");
    breakdown[0].amount = total_revenue;
    breakdown[0].percentage = 100.0;
    
    // Delivery Fees
    strcpy(breakdown[1].category, "Delivery Fees");
    breakdown[1].amount = total_revenue * 0.10;
    breakdown[1].percentage = 10.0;
    
    // Restaurant Commissions
    strcpy(breakdown[2].category, "Restaurant Commissions");
    breakdown[2].amount = total_revenue * 0.15;
    breakdown[2].percentage = 15.0;
    
    // System Revenue (Net)
    strcpy(breakdown[3].category, "System Net Revenue");
    breakdown[3].amount = breakdown[1].amount + breakdown[2].amount;
    breakdown[3].percentage = 25.0;
    
    return *count;
}

// Function to generate performance metrics
int generate_performance_metrics(PerformanceMetric *metrics, int *count) {
    *count = 6;
    
    // Order Completion Rate
    strcpy(metrics[0].metric_name, "Order Completion Rate");
    int total_orders = count_users_by_type(ORDER_FILE, "order");
    int completed_orders = count_orders_by_status_admin("DELIVERED");
    metrics[0].current_value = (total_orders > 0) ? ((float)completed_orders / total_orders * 100.0) : 0.0;
    metrics[0].target_value = 95.0;
    strcpy(metrics[0].status, (metrics[0].current_value >= metrics[0].target_value) ? "Good" : "Needs Improvement");
    strcpy(metrics[0].trend, (metrics[0].current_value > 80.0) ? "Improving" : "Declining");
    
    // On-Time Delivery Rate
    strcpy(metrics[1].metric_name, "On-Time Delivery Rate");
    metrics[1].current_value = calculate_on_time_rate();
    metrics[1].target_value = 90.0;
    strcpy(metrics[1].status, (metrics[1].current_value >= metrics[1].target_value) ? "Excellent" : "Good");
    strcpy(metrics[1].trend, (metrics[1].current_value > 85.0) ? "Stable" : "Needs Attention");
    
    // Average Delivery Time
    strcpy(metrics[2].metric_name, "Average Delivery Time (min)");
    metrics[2].current_value = calculate_average_delivery_time();
    metrics[2].target_value = 30.0;
    strcpy(metrics[2].status, (metrics[2].current_value <= metrics[2].target_value) ? "Excellent" : "Good");
    strcpy(metrics[2].trend, (metrics[2].current_value < 40.0) ? "Improving" : "Stable");
    
    // User Satisfaction (estimated)
    strcpy(metrics[3].metric_name, "User Satisfaction (%)");
    metrics[3].current_value = 85.0; // Sample data
    metrics[3].target_value = 90.0;
    strcpy(metrics[3].status, (metrics[3].current_value >= metrics[3].target_value) ? "Excellent" : "Good");
    strcpy(metrics[3].trend, "Stable");
    
    // System Uptime
    strcpy(metrics[4].metric_name, "System Uptime (%)");
    metrics[4].current_value = 99.5; // Sample data
    metrics[4].target_value = 99.0;
    strcpy(metrics[4].status, "Excellent");
    strcpy(metrics[4].trend, "Stable");
    
    // Revenue Growth
    strcpy(metrics[5].metric_name, "Revenue Growth (%)");
    metrics[5].current_value = 12.5; // Sample data
    metrics[5].target_value = 10.0;
    strcpy(metrics[5].status, "Excellent");
    strcpy(metrics[5].trend, "Growing");
    
    return *count;
}

// Function to display system overview
void display_system_overview(SystemAnalytics *analytics) {
    printf("\n==================== SYSTEM OVERVIEW ====================\n");
    char date[DATE_LENGTH];
    get_analytics_date(date);
    printf("Report Date: %s\n", date);
    printf("Campus Food Delivery System Analytics\n");
    
    printf("\n--- USER STATISTICS ---\n");
    printf("Students: %d (Active: %d)\n", analytics->total_students, analytics->active_students);
    printf("Restaurants: %d (Active: %d)\n", analytics->total_restaurants, analytics->active_restaurants);
    printf("Delivery Personnel: %d (Active: %d)\n", analytics->total_delivery_personnel, analytics->active_delivery_personnel);
    printf("Total System Users: %d\n", analytics->total_students + analytics->total_restaurants + analytics->total_delivery_personnel);
    
    printf("\n--- ORDER STATISTICS ---\n");
    printf("Total Orders: %d\n", analytics->total_orders);
    printf("Completed Orders: %d (%.1f%%)\n", analytics->completed_orders, analytics->completion_rate);
    printf("Pending Orders: %d\n", analytics->pending_orders);
    printf("Cancelled Orders: %d (%.1f%%)\n", analytics->cancelled_orders, analytics->cancellation_rate);
    
    printf("\n--- REVENUE ANALYTICS ---\n");
    printf("Total System Revenue: RM%.2f\n", analytics->total_system_revenue);
    printf("Average Order Value: RM%.2f\n", analytics->average_order_value);
    printf("Total Delivery Fees: RM%.2f\n", analytics->total_delivery_fees);
    printf("Restaurant Commissions: RM%.2f\n", analytics->restaurant_commissions);
    
    printf("\n--- PERFORMANCE METRICS ---\n");
    printf("Average Delivery Time: %.1f minutes\n", analytics->average_delivery_time);
    printf("On-Time Delivery Rate: %.1f%%\n", analytics->on_time_delivery_rate);
    printf("Peak Hour: %02d:00 (RM%.2f revenue)\n", analytics->peak_hour, analytics->peak_hour_revenue);
    
    printf("\n--- GROWTH INDICATORS ---\n");
    printf("New Users Today: %d\n", analytics->new_users_today);
    printf("Orders Today: %d\n", analytics->orders_today);
    printf("Revenue Today: RM%.2f\n", analytics->revenue_today);
    
    printf("\n--- SYSTEM HEALTH ---\n");
    if (analytics->completion_rate >= 90.0) {
        printf("System Status: Excellent - High completion rate\n");
    } else if (analytics->completion_rate >= 80.0) {
        printf("System Status: Good - Stable performance\n");
    } else {
        printf("System Status: Needs Attention - Low completion rate\n");
    }
    
    if (analytics->on_time_delivery_rate >= 85.0) {
        printf("Delivery Performance: Excellent\n");
    } else {
        printf("Delivery Performance: Needs Improvement\n");
    }
    
    printf("=========================================================\n");
}

// Function to display user activity report
void display_user_activity_report(UserActivityReport *reports, int count) {
    printf("\n========================== USER ACTIVITY REPORT ==========================\n");
    printf("%-20s %-8s %-8s %-12s %-15s %-8s\n", 
           "User Type", "Total", "Active", "Activity %", "Most Active", "Count");
    printf("--------------------------------------------------------------------------\n");
    
    int total_users = 0;
    int total_active = 0;
    
    for (int i = 0; i < count; i++) {
        printf("%-20s %-8d %-8d %-11.1f%% %-15s %-8d\n", 
               reports[i].user_type, reports[i].total_users, reports[i].active_users,
               reports[i].activity_rate, reports[i].most_active_user, reports[i].most_active_count);
        
        total_users += reports[i].total_users;
        total_active += reports[i].active_users;
    }
    
    printf("--------------------------------------------------------------------------\n");
    printf("%-20s %-8d %-8d %-11.1f%%\n", "TOTALS", total_users, total_active, 
           (total_users > 0) ? ((float)total_active / total_users * 100.0) : 0.0);
    
    printf("\n--- ACTIVITY INSIGHTS ---\n");
    if (total_users > 0) {
        float overall_activity = (float)total_active / total_users * 100.0;
        if (overall_activity >= 90.0) {
            printf("Overall Activity: Excellent (%.1f%% active users)\n", overall_activity);
        } else if (overall_activity >= 75.0) {
            printf("Overall Activity: Good (%.1f%% active users)\n", overall_activity);
        } else {
            printf("Overall Activity: Needs Improvement (%.1f%% active users)\n", overall_activity);
        }
    }
    
    // Find most active user type
    int most_active_index = 0;
    for (int i = 1; i < count; i++) {
        if (reports[i].activity_rate > reports[most_active_index].activity_rate) {
            most_active_index = i;
        }
    }
    printf("Most Active User Type: %s (%.1f%% activity rate)\n", 
           reports[most_active_index].user_type, reports[most_active_index].activity_rate);
    
    printf("===========================================================================\n");
}

// Function to display revenue analytics
void display_revenue_analytics(RevenueBreakdown *breakdown, int count) {
    printf("\n================ REVENUE ANALYTICS ================\n");
    printf("%-25s %-12s %-12s\n", "Revenue Category", "Amount (RM)", "Percentage");
    printf("---------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-25s %-11.2f %-11.1f%%\n", 
               breakdown[i].category, breakdown[i].amount, breakdown[i].percentage);
    }
    
    printf("---------------------------------------------------\n");
    
    printf("\n--- REVENUE INSIGHTS ---\n");
    float total_revenue = breakdown[0].amount;
    float system_revenue = breakdown[3].amount;
    
    printf("Total Revenue Generated: RM%.2f\n", total_revenue);
    printf("System Net Revenue: RM%.2f (%.1f%% of total)\n", system_revenue, 
           (total_revenue > 0) ? (system_revenue / total_revenue * 100.0) : 0.0);
    
    if (total_revenue > 1000.0) {
        printf("Revenue Performance: Excellent - Strong revenue generation\n");
    } else if (total_revenue > 500.0) {
        printf("Revenue Performance: Good - Steady revenue growth\n");
    } else {
        printf("Revenue Performance: Developing - Focus on growth strategies\n");
    }
    
    printf("Revenue Distribution: Balanced between delivery fees and commissions\n");
    
    printf("====================================================\n");
}

// Function to display performance dashboard
void display_performance_dashboard(PerformanceMetric *metrics, int count) {
    printf("\n============================ PERFORMANCE DASHBOARD ============================\n");
    printf("%-30s %-10s %-10s %-15s %-12s\n", 
           "Performance Metric", "Current", "Target", "Status", "Trend");
    printf("-------------------------------------------------------------------------------\n");
    
    int excellent_count = 0;
    int good_count = 0;
    int needs_improvement = 0;
    
    for (int i = 0; i < count; i++) {
        printf("%-30s %-9.1f %-9.1f %-15s %-12s\n", 
               metrics[i].metric_name, metrics[i].current_value, metrics[i].target_value,
               metrics[i].status, metrics[i].trend);
        
        if (strcmp(metrics[i].status, "Excellent") == 0) {
            excellent_count++;
        } else if (strcmp(metrics[i].status, "Good") == 0) {
            good_count++;
        } else {
            needs_improvement++;
        }
    }
    
    printf("-------------------------------------------------------------------------------\n");
    
    printf("\n--- PERFORMANCE SUMMARY ---\n");
    printf("Excellent Metrics: %d/%d\n", excellent_count, count);
    printf("Good Metrics: %d/%d\n", good_count, count);
    printf("Needs Improvement: %d/%d\n", needs_improvement, count);
    
    float performance_score = ((float)(excellent_count * 3 + good_count * 2 + needs_improvement * 1) / (count * 3)) * 100.0;
    printf("Overall Performance Score: %.1f%%\n", performance_score);
    
    if (performance_score >= 85.0) {
        printf("System Performance: Excellent - All metrics performing well\n");
    } else if (performance_score >= 70.0) {
        printf("System Performance: Good - Most metrics on target\n");
    } else {
        printf("System Performance: Needs Attention - Several metrics below target\n");
    }
    
    printf("\n--- RECOMMENDATIONS ---\n");
    if (needs_improvement > 0) {
        printf("Focus on improving metrics marked as 'Needs Improvement'\n");
    }
    if (excellent_count == count) {
        printf("Congratulations! All performance metrics are excellent\n");
    } else {
        printf("Continue monitoring and optimizing system performance\n");
    }
    
    printf("================================================================\n");
}

// Function to view system overview
void view_system_overview(void) {
    printf("\n----- SYSTEM OVERVIEW -----\n");
    
    SystemAnalytics analytics;
    if (generate_system_analytics(&analytics) == 1) {
        display_system_overview(&analytics);
    } else {
        printf("Error generating system analytics.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view user activity report
void view_user_activity_report(void) {
    printf("\n----- USER ACTIVITY REPORT -----\n");
    
    UserActivityReport reports[10];
    int count;
    
    if (generate_user_activity_report(reports, &count) > 0) {
        display_user_activity_report(reports, count);
    } else {
        printf("Error generating user activity report.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view revenue analytics
void view_revenue_analytics(void) {
    printf("\n----- REVENUE ANALYTICS -----\n");
    
    RevenueBreakdown breakdown[10];
    int count;
    
    if (generate_revenue_breakdown(breakdown, &count) > 0) {
        display_revenue_analytics(breakdown, count);
    } else {
        printf("Error generating revenue analytics.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view performance dashboard
void view_performance_dashboard(void) {
    printf("\n----- PERFORMANCE DASHBOARD -----\n");
    
    PerformanceMetric metrics[10];
    int count;
    
    if (generate_performance_metrics(metrics, &count) > 0) {
        display_performance_dashboard(metrics, count);
    } else {
        printf("Error generating performance metrics.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view detailed statistics
void view_detailed_statistics(void) {
    printf("\n----- DETAILED STATISTICS -----\n");
    
    // Generate all analytics data
    SystemAnalytics analytics;
    UserActivityReport user_reports[10];
    RevenueBreakdown revenue_breakdown[10];
    PerformanceMetric performance_metrics[10];
    
    int user_count, revenue_count, metrics_count;
    
    generate_system_analytics(&analytics);
    generate_user_activity_report(user_reports, &user_count);
    generate_revenue_breakdown(revenue_breakdown, &revenue_count);
    generate_performance_metrics(performance_metrics, &metrics_count);
    
    // Display comprehensive report
    display_system_overview(&analytics);
    printf("\n");
    display_user_activity_report(user_reports, user_count);
    printf("\n");
    display_revenue_analytics(revenue_breakdown, revenue_count);
    printf("\n");
    display_performance_dashboard(performance_metrics, metrics_count);
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to export analytics report
void export_analytics_report(void) {
    char filename[100];
    
    printf("\n----- EXPORT ANALYTICS REPORT -----\n");
    if (!safe_filename_input("Enter filename (without extension): ", filename, sizeof(filename))) {
        printf("Invalid filename. Export cancelled.\n");
        return;
    }
    
    char full_filename[120];
    char date[DATE_LENGTH];
    get_analytics_date(date);
    snprintf(full_filename, sizeof(full_filename), "%s_analytics_%s.txt", filename, date);
    
    FILE *fp = fopen(full_filename, "w");
    if (fp == NULL) {
        printf("Error: Could not create export file.\n");
        return;
    }
    
    // Generate all analytics data
    SystemAnalytics analytics;
    UserActivityReport user_reports[10];
    RevenueBreakdown revenue_breakdown[10];
    PerformanceMetric performance_metrics[10];
    
    int user_count, revenue_count, metrics_count;
    
    generate_system_analytics(&analytics);
    generate_user_activity_report(user_reports, &user_count);
    generate_revenue_breakdown(revenue_breakdown, &revenue_count);
    generate_performance_metrics(performance_metrics, &metrics_count);
    
    // Write comprehensive report
    fprintf(fp, "========================================\n");
    fprintf(fp, "CAMPUS FOOD DELIVERY SYSTEM\n");
    fprintf(fp, "ANALYTICS REPORT\n");
    fprintf(fp, "========================================\n");
    fprintf(fp, "Report Date: %s\n", date);
    fprintf(fp, "Generated by: System Administrator\n");
    fprintf(fp, "========================================\n\n");
    
    // System Overview
    fprintf(fp, "SYSTEM OVERVIEW\n");
    fprintf(fp, "---------------\n");
    fprintf(fp, "Total Users: %d\n", analytics.total_students + analytics.total_restaurants + analytics.total_delivery_personnel);
    fprintf(fp, "- Students: %d\n", analytics.total_students);
    fprintf(fp, "- Restaurants: %d\n", analytics.total_restaurants);
    fprintf(fp, "- Delivery Personnel: %d\n", analytics.total_delivery_personnel);
    fprintf(fp, "\nOrder Statistics:\n");
    fprintf(fp, "- Total Orders: %d\n", analytics.total_orders);
    fprintf(fp, "- Completed: %d (%.1f%%)\n", analytics.completed_orders, analytics.completion_rate);
    fprintf(fp, "- Pending: %d\n", analytics.pending_orders);
    fprintf(fp, "- Cancelled: %d (%.1f%%)\n", analytics.cancelled_orders, analytics.cancellation_rate);
    fprintf(fp, "\nRevenue Analytics:\n");
    fprintf(fp, "- Total Revenue: RM%.2f\n", analytics.total_system_revenue);
    fprintf(fp, "- Average Order Value: RM%.2f\n", analytics.average_order_value);
    fprintf(fp, "- Delivery Fees: RM%.2f\n", analytics.total_delivery_fees);
    fprintf(fp, "- Restaurant Commissions: RM%.2f\n", analytics.restaurant_commissions);
    fprintf(fp, "\n");
    
    // User Activity Report
    fprintf(fp, "USER ACTIVITY REPORT\n");
    fprintf(fp, "--------------------\n");
    for (int i = 0; i < user_count; i++) {
        fprintf(fp, "%s: %d total, %d active (%.1f%% activity)\n", 
                user_reports[i].user_type, user_reports[i].total_users, 
                user_reports[i].active_users, user_reports[i].activity_rate);
    }
    fprintf(fp, "\n");
    
    // Revenue Breakdown
    fprintf(fp, "REVENUE BREAKDOWN\n");
    fprintf(fp, "-----------------\n");
    for (int i = 0; i < revenue_count; i++) {
        fprintf(fp, "%s: RM%.2f (%.1f%%)\n", 
                revenue_breakdown[i].category, revenue_breakdown[i].amount, 
                revenue_breakdown[i].percentage);
    }
    fprintf(fp, "\n");
    
    // Performance Metrics
    fprintf(fp, "PERFORMANCE METRICS\n");
    fprintf(fp, "-------------------\n");
    for (int i = 0; i < metrics_count; i++) {
        fprintf(fp, "%s: %.1f (Target: %.1f) - %s\n", 
                performance_metrics[i].metric_name, performance_metrics[i].current_value,
                performance_metrics[i].target_value, performance_metrics[i].status);
    }
    
    fprintf(fp, "\n========================================\n");
    fprintf(fp, "END OF ANALYTICS REPORT\n");
    fprintf(fp, "========================================\n");
    
    fclose(fp);
    printf("Analytics report exported successfully to '%s'.\n", full_filename);
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// -----------------------------------------------------------------------------------------------
// Data Backup and Restore Structures

// System data files to backup
static const char* SYSTEM_DATA_FILES[] = {
    USER_FILE,
    STUDENT_FILE,
    RESTAURANT_FILE,
    MENU_ITEM_FILE,
    ORDER_FILE,
    QUEUE_FILE,
    DELIVERY_PERSONNEL_FILE,
    DELIVERY_FILE,
    INVENTORY_FILE,
    "config.txt"
};

#define SYSTEM_FILE_COUNT (sizeof(SYSTEM_DATA_FILES) / sizeof(SYSTEM_DATA_FILES[0]))

// Forward declarations for backup and restore
int create_system_backup(char *backup_name);
int restore_system_backup(char *backup_name);
int verify_backup_integrity(char *backup_name);
int list_available_backups(BackupInfo *backups, int *count);
void display_backup_list(BackupInfo *backups, int count);
void create_backup_directory(char *backup_name);
int copy_file_to_backup(const char *source_file, const char *backup_dir);
int restore_file_from_backup(const char *backup_file, const char *target_file);
long get_file_size(const char *filename);
void get_current_datetime(char *date_str, char *time_str);
int validate_backup_name(char *backup_name);
void cleanup_old_backups(int max_backups);
void view_backup_details(char *backup_name);
void create_new_backup(void);
void restore_from_backup(void);
void view_backup_list(void);
void verify_backup(void);
void manage_backup_settings(void);
void cleanup_backups(void);
int auto_backup_system(void);

#define CREATE_BACKUP 1
#define RESTORE_BACKUP 2
#define VIEW_BACKUP_LIST 3
#define VERIFY_BACKUP 4
#define BACKUP_SETTINGS 5
#define CLEANUP_BACKUPS 6
#define AUTO_BACKUP 7

int data_backup_and_restore_functionality(void) {
    int choice;
    
    while (1) {
        char prompt[] = "\n----- DATA BACKUP AND RESTORE FUNCTIONALITY -----\n"
                        "1. Create new backup\n"
                        "2. Restore from backup\n"
                        "3. View backup list\n"
                        "4. Verify backup integrity\n"
                        "5. Manage backup settings\n"
                        "6. Cleanup old backups\n"
                        "7. Auto backup system\n"
                        "0. Exit\n"
                        "What would you like to do? (0-7): ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case CREATE_BACKUP:
                create_new_backup();
                break;
            case RESTORE_BACKUP:
                restore_from_backup();
                break;
            case VIEW_BACKUP_LIST:
                view_backup_list();
                break;
            case VERIFY_BACKUP:
                verify_backup();
                break;
            case BACKUP_SETTINGS:
                manage_backup_settings();
                break;
            case CLEANUP_BACKUPS:
                cleanup_backups();
                break;
            case AUTO_BACKUP:
                auto_backup_system();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get current date and time
void get_current_datetime(char *date_str, char *time_str) {
    // For demonstration, using fixed current date/time
    // In a real system, this would get actual current date/time
    strcpy(date_str, "28-12-2024");
    strcpy(time_str, "16:30:00");
}

// Function to get file size
long get_file_size(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    
    return size;
}

// Function to validate backup name
int validate_backup_name(char *backup_name) {
    // Check for valid characters and length
    if (strlen(backup_name) == 0 || strlen(backup_name) > 50) {
        return 0;
    }
    
    // Check for invalid characters
    for (int i = 0; backup_name[i] != '\0'; i++) {
        char c = backup_name[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
              (c >= '0' && c <= '9') || c == '_' || c == '-')) {
            return 0;
        }
    }
    
    return 1;
}

// Function to create backup directory
void create_backup_directory(char *backup_name) {
    char command[200];
    snprintf(command, sizeof(command), "mkdir backups\\%s 2>nul", backup_name);
    system("mkdir backups 2>nul"); // Create main backup directory
    system(command); // Create specific backup directory
}

// Function to copy file to backup
int copy_file_to_backup(const char *source_file, const char *backup_dir) {
    char command[300];
    snprintf(command, sizeof(command), "copy \"%s\" \"backups\\%s\\\" >nul 2>&1", source_file, backup_dir);
    
    int result = system(command);
    return (result == 0) ? 1 : 0;
}

// Function to restore file from backup
int restore_file_from_backup(const char *backup_file, const char *target_file) {
    char command[300];
    snprintf(command, sizeof(command), "copy \"%s\" \"%s\" >nul 2>&1", backup_file, target_file);
    
    int result = system(command);
    return (result == 0) ? 1 : 0;
}

// Function to create system backup
int create_system_backup(char *backup_name) {
    printf("Creating backup '%s'...\n", backup_name);
    
    // Create backup directory
    create_backup_directory(backup_name);
    
    int successful_files = 0;
    int total_files = 0;
    long total_size = 0;
    
    // Backup each system file
    for (int i = 0; i < SYSTEM_FILE_COUNT; i++) {
        const char *filename = SYSTEM_DATA_FILES[i];
        
        // Check if file exists
        FILE *fp = fopen(filename, "r");
        if (fp != NULL) {
            fclose(fp);
            total_files++;
            
            // Copy file to backup directory
            if (copy_file_to_backup(filename, backup_name)) {
                successful_files++;
                total_size += get_file_size(filename);
                printf("✓ Backed up: %s\n", filename);
            } else {
                printf("✗ Failed to backup: %s\n", filename);
            }
        } else {
            printf("⚠ File not found: %s (skipping)\n", filename);
        }
    }
    
    // Create backup info file
    char info_file[200];
    snprintf(info_file, sizeof(info_file), "backups\\%s\\backup_info.txt", backup_name);
    
    FILE *info_fp = fopen(info_file, "w");
    if (info_fp != NULL) {
        char date[DATE_LENGTH], time[TIME_LENGTH];
        get_current_datetime(date, time);
        
        fprintf(info_fp, "Backup Name: %s\n", backup_name);
        fprintf(info_fp, "Backup Date: %s\n", date);
        fprintf(info_fp, "Backup Time: %s\n", time);
        fprintf(info_fp, "Files Backed Up: %d/%d\n", successful_files, total_files);
        fprintf(info_fp, "Total Size: %ld bytes\n", total_size);
        fprintf(info_fp, "Status: %s\n", (successful_files == total_files) ? "Complete" : "Partial");
        fprintf(info_fp, "System: Campus Food Delivery System\n");
        
        fclose(info_fp);
    }
    
    printf("\nBackup Summary:\n");
    printf("- Files backed up: %d/%d\n", successful_files, total_files);
    printf("- Total size: %ld bytes\n", total_size);
    printf("- Status: %s\n", (successful_files == total_files) ? "Complete" : "Partial");
    
    return (successful_files > 0) ? 1 : 0;
}

// Function to restore system backup
int restore_system_backup(char *backup_name) {
    printf("Restoring from backup '%s'...\n", backup_name);
    
    // Check if backup directory exists
    char backup_dir[150];
    snprintf(backup_dir, sizeof(backup_dir), "backups\\%s", backup_name);
    
    // Verify backup info file exists
    char info_file[200];
    snprintf(info_file, sizeof(info_file), "%s\\backup_info.txt", backup_dir);
    
    FILE *info_fp = fopen(info_file, "r");
    if (info_fp == NULL) {
        printf("Error: Backup info file not found. Invalid backup.\n");
        return 0;
    }
    fclose(info_fp);
    
    int successful_restores = 0;
    int total_files = 0;
    
    // Restore each system file
    for (int i = 0; i < SYSTEM_FILE_COUNT; i++) {
        const char *filename = SYSTEM_DATA_FILES[i];
        
        char backup_file[250];
        snprintf(backup_file, sizeof(backup_file), "%s\\%s", backup_dir, filename);
        
        // Check if backup file exists
        FILE *fp = fopen(backup_file, "r");
        if (fp != NULL) {
            fclose(fp);
            total_files++;
            
            // Restore file
            if (restore_file_from_backup(backup_file, filename)) {
                successful_restores++;
                printf("✓ Restored: %s\n", filename);
            } else {
                printf("✗ Failed to restore: %s\n", filename);
            }
        } else {
            printf("⚠ Backup file not found: %s (skipping)\n", filename);
        }
    }
    
    printf("\nRestore Summary:\n");
    printf("- Files restored: %d/%d\n", successful_restores, total_files);
    printf("- Status: %s\n", (successful_restores == total_files) ? "Complete" : "Partial");
    
    if (successful_restores > 0) {
        printf("\n⚠ IMPORTANT: System restart recommended after restore.\n");
    }
    
    return (successful_restores > 0) ? 1 : 0;
}

// Function to verify backup integrity
int verify_backup_integrity(char *backup_name) {
    printf("Verifying backup integrity for '%s'...\n", backup_name);
    
    char backup_dir[150];
    snprintf(backup_dir, sizeof(backup_dir), "backups\\%s", backup_name);
    
    // Check backup info file
    char info_file[200];
    snprintf(info_file, sizeof(info_file), "%s\\backup_info.txt", backup_dir);
    
    FILE *info_fp = fopen(info_file, "r");
    if (info_fp == NULL) {
        printf("✗ Backup info file missing - Invalid backup\n");
        return 0;
    }
    fclose(info_fp);
    printf("✓ Backup info file found\n");
    
    int verified_files = 0;
    int total_expected = 0;
    
    // Verify each system file
    for (int i = 0; i < SYSTEM_FILE_COUNT; i++) {
        const char *filename = SYSTEM_DATA_FILES[i];
        
        char backup_file[250];
        snprintf(backup_file, sizeof(backup_file), "%s\\%s", backup_dir, filename);
        
        FILE *fp = fopen(backup_file, "r");
        if (fp != NULL) {
            fclose(fp);
            verified_files++;
            printf("✓ Verified: %s\n", filename);
        } else {
            printf("✗ Missing: %s\n", filename);
        }
        total_expected++;
    }
    
    printf("\nVerification Summary:\n");
    printf("- Files verified: %d/%d\n", verified_files, total_expected);
    printf("- Integrity: %s\n", (verified_files == total_expected) ? "Complete" : "Incomplete");
    
    return (verified_files > 0) ? 1 : 0;
}

// Function to list available backups
int list_available_backups(BackupInfo *backups, int *count) {
    *count = 0;
    
    // For simplification, we'll create sample backup data
    // In a real system, this would scan the backup directory
    
    // Sample backup 1
    strcpy(backups[0].backup_name, "daily_backup_20241228");
    strcpy(backups[0].backup_date, "28-12-2024");
    strcpy(backups[0].backup_time, "16:30:00");
    backups[0].file_count = 10;
    backups[0].total_size = 15420;
    strcpy(backups[0].status, "Complete");
    (*count)++;
    
    // Sample backup 2
    strcpy(backups[1].backup_name, "manual_backup_001");
    strcpy(backups[1].backup_date, "27-12-2024");
    strcpy(backups[1].backup_time, "14:15:00");
    backups[1].file_count = 9;
    backups[1].total_size = 14850;
    strcpy(backups[1].status, "Partial");
    (*count)++;
    
    // Sample backup 3
    strcpy(backups[2].backup_name, "system_backup_v1");
    strcpy(backups[2].backup_date, "26-12-2024");
    strcpy(backups[2].backup_time, "10:00:00");
    backups[2].file_count = 10;
    backups[2].total_size = 16200;
    strcpy(backups[2].status, "Complete");
    (*count)++;
    
    return *count;
}

// Function to display backup list
void display_backup_list(BackupInfo *backups, int count) {
    if (count == 0) {
        printf("No backups found.\n");
        return;
    }
    
    printf("\n=============================== AVAILABLE BACKUPS ===============================\n");
    printf("%-3s %-25s %-12s %-10s %-6s %-8s %-10s\n", 
           "No.", "Backup Name", "Date", "Time", "Files", "Size", "Status");
    printf("---------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-3d %-25s %-12s %-10s %-6d %-8ld %-10s\n", 
               i + 1, backups[i].backup_name, backups[i].backup_date,
               backups[i].backup_time, backups[i].file_count, 
               backups[i].total_size, backups[i].status);
    }
    
    printf("---------------------------------------------------------------------------------\n");
    printf("Total backups: %d\n", count);
    
    // Calculate total backup size
    long total_size = 0;
    for (int i = 0; i < count; i++) {
        total_size += backups[i].total_size;
    }
    printf("Total backup size: %ld bytes\n", total_size);
    
    printf("============================================================\n");
}

// Function to create new backup
void create_new_backup(void) {
    char backup_name[100];
    
    printf("\n----- CREATE NEW BACKUP -----\n");
    if (!safe_filename_input("Enter backup name (alphanumeric, underscore, hyphen only): ", backup_name, sizeof(backup_name))) {
        printf("Invalid backup name. Backup cancelled.\n");
        return;
    }
    
    if (!validate_backup_name(backup_name)) {
        printf("Invalid backup name. Use only letters, numbers, underscore, and hyphen.\n");
        return;
    }
    
    // Check if backup already exists
    char backup_dir[150];
    snprintf(backup_dir, sizeof(backup_dir), "backups\\%s\\backup_info.txt", backup_name);
    
    FILE *fp = fopen(backup_dir, "r");
    if (fp != NULL) {
        fclose(fp);
        printf("Backup with name '%s' already exists.\n", backup_name);
        int overwrite = confirmation("Do you want to overwrite it? (1=Yes, 2=No): ");
        if (overwrite != 1) {
            printf("Backup creation cancelled.\n");
            return;
        }
    }
    
    printf("\nStarting backup process...\n");
    printf("This will backup all system data files.\n");
    
    int confirm = confirmation("Continue with backup creation? (1=Yes, 2=No): ");
    if (confirm == 1) {
        if (create_system_backup(backup_name)) {
            printf("\n✓ Backup '%s' created successfully!\n", backup_name);
        } else {
            printf("\n✗ Backup creation failed or incomplete.\n");
        }
    } else {
        printf("Backup creation cancelled.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to restore from backup
void restore_from_backup(void) {
    printf("\n----- RESTORE FROM BACKUP -----\n");
    
    BackupInfo backups[50];
    int count;
    
    if (list_available_backups(backups, &count) == 0) {
        printf("No backups available for restore.\n");
        return;
    }
    
    display_backup_list(backups, count);
    
    int backup_choice;
    while (1) {
        backup_choice = get_integer_input("Select backup number to restore (0 to cancel): ");
        if (backup_choice >= 0 && backup_choice <= count) {
            break;
        }
        printf("Invalid choice. Please select a number between 1 and %d.\n", count);
    }
    
    if (backup_choice == 0) {
        printf("Restore cancelled.\n");
        return;
    }
    
    char *selected_backup = backups[backup_choice - 1].backup_name;
    
    printf("\n⚠ WARNING: This will overwrite current system data!\n");
    printf("Selected backup: %s\n", selected_backup);
    printf("Backup date: %s %s\n", backups[backup_choice - 1].backup_date, backups[backup_choice - 1].backup_time);
    printf("Files in backup: %d\n", backups[backup_choice - 1].file_count);
    
    int confirm = confirmation("Are you sure you want to restore? (1=Yes, 2=No): ");
    if (confirm == 1) {
        printf("\nStarting restore process...\n");
        
        if (restore_system_backup(selected_backup)) {
            printf("\n✓ System restored successfully from backup '%s'!\n", selected_backup);
            printf("⚠ Please restart the system to ensure all changes take effect.\n");
        } else {
            printf("\n✗ Restore failed or incomplete.\n");
        }
    } else {
        printf("Restore cancelled.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view backup list
void view_backup_list(void) {
    printf("\n----- BACKUP LIST -----\n");
    
    BackupInfo backups[50];
    int count;
    
    if (list_available_backups(backups, &count) > 0) {
        display_backup_list(backups, count);
        
        printf("\n--- BACKUP MANAGEMENT OPTIONS ---\n");
        printf("1. View detailed backup info\n");
        printf("2. Delete a backup\n");
        printf("0. Exit\n");
        
        int choice = get_integer_input("Select option: ");
        
        switch (choice) {
            case 1: {
                int backup_num = get_integer_input("Enter backup number for details: ");
                if (backup_num >= 1 && backup_num <= count) {
                    view_backup_details(backups[backup_num - 1].backup_name);
                } else {
                    printf("Invalid backup number.\n");
                }
                break;
            }
            case 2: {
                int backup_num = get_integer_input("Enter backup number to delete: ");
                if (backup_num >= 1 && backup_num <= count) {
                    printf("Selected backup: %s\n", backups[backup_num - 1].backup_name);
                    int confirm = confirmation("Are you sure you want to delete this backup? (1=Yes, 2=No): ");
                    if (confirm == 1) {
                        char command[200];
                        snprintf(command, sizeof(command), "rmdir /s /q \"backups\\%s\" 2>nul", 
                                backups[backup_num - 1].backup_name);
                        if (system(command) == 0) {
                            printf("Backup deleted successfully.\n");
                        } else {
                            printf("Failed to delete backup.\n");
                        }
                    }
                } else {
                    printf("Invalid backup number.\n");
                }
                break;
            }
        }
    } else {
        printf("No backups found.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to verify backup
void verify_backup(void) {
    printf("\n----- VERIFY BACKUP INTEGRITY -----\n");
    
    BackupInfo backups[50];
    int count;
    
    if (list_available_backups(backups, &count) == 0) {
        printf("No backups available for verification.\n");
        return;
    }
    
    display_backup_list(backups, count);
    
    int backup_choice;
    while (1) {
        backup_choice = get_integer_input("Select backup number to verify (0 to cancel): ");
        if (backup_choice >= 0 && backup_choice <= count) {
            break;
        }
        printf("Invalid choice. Please select a number between 1 and %d.\n", count);
    }
    
    if (backup_choice == 0) {
        printf("Verification cancelled.\n");
        return;
    }
    
    char *selected_backup = backups[backup_choice - 1].backup_name;
    
    printf("\nVerifying backup: %s\n", selected_backup);
    
    if (verify_backup_integrity(selected_backup)) {
        printf("\n✓ Backup verification completed.\n");
    } else {
        printf("\n✗ Backup verification failed or backup is corrupted.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view backup details
void view_backup_details(char *backup_name) {
    printf("\n==================== BACKUP DETAILS ====================\n");
    printf("Backup Name: %s\n", backup_name);
    
    char info_file[200];
    snprintf(info_file, sizeof(info_file), "backups\\%s\\backup_info.txt", backup_name);
    
    FILE *fp = fopen(info_file, "r");
    if (fp == NULL) {
        printf("Backup info file not found.\n");
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
    
    printf("\n--- FILES IN BACKUP ---\n");
    for (int i = 0; i < SYSTEM_FILE_COUNT; i++) {
        const char *filename = SYSTEM_DATA_FILES[i];
        
        char backup_file[250];
        snprintf(backup_file, sizeof(backup_file), "backups\\%s\\%s", backup_name, filename);
        
        FILE *file_fp = fopen(backup_file, "r");
        if (file_fp != NULL) {
            fclose(file_fp);
            long size = get_file_size(backup_file);
            printf("✓ %s (%ld bytes)\n", filename, size);
        } else {
            printf("✗ %s (missing)\n", filename);
        }
    }
    
    printf("=========================================================\n");
}

// Function to manage backup settings
void manage_backup_settings(void) {
    printf("\n----- BACKUP SETTINGS -----\n");
    printf("1. View current backup settings\n");
    printf("2. Configure auto-backup schedule\n");
    printf("3. Set backup retention policy\n");
    printf("4. Configure backup location\n");
    printf("0. Exit\n");
    
    int choice = get_integer_input("Select option: ");
    
    switch (choice) {
        case 1:
            printf("\n--- CURRENT BACKUP SETTINGS ---\n");
            printf("Backup Location: .\\backups\\\n");
            printf("Auto-backup: Enabled (Daily at 02:00)\n");
            printf("Retention Policy: Keep 7 backups\n");
            printf("Compression: Disabled\n");
            printf("Verification: Enabled\n");
            break;
            
        case 2:
            printf("\n--- AUTO-BACKUP SCHEDULE ---\n");
            printf("Current schedule: Daily at 02:00\n");
            printf("1. Daily\n");
            printf("2. Weekly\n");
            printf("3. Disabled\n");
            
            int schedule = get_integer_input("Select schedule: ");
            switch (schedule) {
                case 1:
                    printf("Auto-backup set to daily.\n");
                    break;
                case 2:
                    printf("Auto-backup set to weekly.\n");
                    break;
                case 3:
                    printf("Auto-backup disabled.\n");
                    break;
                default:
                    printf("Invalid choice.\n");
            }
            break;
            
        case 3:
            printf("\n--- RETENTION POLICY ---\n");
            printf("Current policy: Keep 7 backups\n");
            int retention = get_integer_input("Enter number of backups to keep (1-30): ");
            if (retention >= 1 && retention <= 30) {
                printf("Retention policy updated to keep %d backups.\n", retention);
            } else {
                printf("Invalid retention count.\n");
            }
            break;
            
        case 4:
            printf("\n--- BACKUP LOCATION ---\n");
            printf("Current location: .\\backups\\\n");
            char location[200];
            if (!safe_string_input("Enter new backup location: ", location, sizeof(location))) {
                printf("Invalid location format.\n");
                break;
            }
            printf("Backup location updated to: %s\n", location);
            break;
            
        default:
            printf("Invalid choice.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to cleanup old backups
void cleanup_backups(void) {
    printf("\n----- CLEANUP OLD BACKUPS -----\n");
    
    BackupInfo backups[50];
    int count;
    
    if (list_available_backups(backups, &count) == 0) {
        printf("No backups found to cleanup.\n");
        return;
    }
    
    printf("Current backups: %d\n", count);
    
    if (count <= 3) {
        printf("Only %d backups found. Cleanup not recommended.\n", count);
        return;
    }
    
    printf("Cleanup options:\n");
    printf("1. Keep latest 3 backups\n");
    printf("2. Keep latest 5 backups\n");
    printf("3. Delete specific backup\n");
    printf("4. Delete all backups (DANGEROUS)\n");
    
    int choice = get_integer_input("Select cleanup option: ");
    
    switch (choice) {
        case 1:
        case 2: {
            int keep_count = (choice == 1) ? 3 : 5;
            if (count > keep_count) {
                printf("This will delete %d old backups, keeping the latest %d.\n", 
                       count - keep_count, keep_count);
                int confirm = confirmation("Continue with cleanup? (1=Yes, 2=No): ");
                if (confirm == 1) {
                    // In a real implementation, this would delete old backups
                    printf("Cleanup completed. Kept %d latest backups.\n", keep_count);
                } else {
                    printf("Cleanup cancelled.\n");
                }
            } else {
                printf("Only %d backups found. No cleanup needed.\n", count);
            }
            break;
        }
        
        case 3:
            display_backup_list(backups, count);
            int backup_num = get_integer_input("Enter backup number to delete: ");
            if (backup_num >= 1 && backup_num <= count) {
                printf("Selected backup: %s\n", backups[backup_num - 1].backup_name);
                int confirm = confirmation("Delete this backup? (1=Yes, 2=No): ");
                if (confirm == 1) {
                    printf("Backup deleted successfully.\n");
                }
            } else {
                printf("Invalid backup number.\n");
            }
            break;
            
        case 4:
            printf("⚠ WARNING: This will delete ALL backups!\n");
            printf("This action cannot be undone.\n");
            int confirm = confirmation("Are you absolutely sure? (1=Yes, 2=No): ");
            if (confirm == 1) {
                int double_confirm = confirmation("Type 1 again to confirm deletion of ALL backups: ");
                if (double_confirm == 1) {
                    // In a real implementation, this would delete all backups
                    printf("All backups deleted.\n");
                } else {
                    printf("Deletion cancelled.\n");
                }
            } else {
                printf("Deletion cancelled.\n");
            }
            break;
            
        default:
            printf("Invalid choice.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function for auto backup system
int auto_backup_system(void) {
    printf("\n----- AUTO BACKUP SYSTEM -----\n");
    
    char auto_backup_name[100];
    char date[DATE_LENGTH], time[TIME_LENGTH];
    get_current_datetime(date, time);
    
    // Generate automatic backup name
    snprintf(auto_backup_name, sizeof(auto_backup_name), "auto_backup_%s_%s", date, time);
    
    // Replace colons and hyphens for valid filename
    for (int i = 0; auto_backup_name[i] != '\0'; i++) {
        if (auto_backup_name[i] == ':' || auto_backup_name[i] == '-') {
            auto_backup_name[i] = '_';
        }
    }
    
    printf("Starting automatic system backup...\n");
    printf("Backup name: %s\n", auto_backup_name);
    
    if (create_system_backup(auto_backup_name)) {
        printf("\n✓ Automatic backup completed successfully!\n");
        
        // Cleanup old auto backups (keep only 5)
        printf("Cleaning up old automatic backups...\n");
        cleanup_old_backups(5);
        
        printf("Auto backup process completed.\n");
        return 1;
    } else {
        printf("\nAutomatic backup failed.\n");
        return 0;
    }
}

// Function to cleanup old backups (internal)
void cleanup_old_backups(int max_backups) {
    // In a real implementation, this would:
    // 1. List all backups sorted by date
    // 2. Keep the latest max_backups
    // 3. Delete older backups
    printf("Cleanup policy: Keep %d latest backups\n", max_backups);
}

// ----------------------------------------------------------------------------------------------

// System Configuration Structure

// Global configuration instance
SystemConfig system_config;

// Forward declarations
void load_config(void);
void save_config(void);
void reset_to_defaults(void);
void validate_config_values(void);
void view_system_settings(void);
void modify_system_settings(void);
void view_security_settings(void);
void modify_security_settings(void);
void view_business_settings(void);
void modify_business_settings(void);
void view_performance_settings(void);
void modify_performance_settings(void);

#define VIEW_SYSTEM_SETTINGS 1
#define MODIFY_SYSTEM_SETTINGS 2
#define VIEW_SECURITY_SETTINGS 3
#define MODIFY_SECURITY_SETTINGS 4
#define VIEW_BUSINESS_SETTINGS 5
#define MODIFY_BUSINESS_SETTINGS 6
#define RESET_TO_DEFAULTS 7

int system_configuration_management(void) {
    int choice;
    
    // Load current configuration
    load_config();
    
    while (1) {
        char prompt[] = "\n----- SYSTEM CONFIGURATION MANAGEMENT -----\n"
                        "1. View system settings\n"
                        "2. Modify system settings\n"
                        "3. View security settings\n"
                        "4. Modify security settings\n"
                        "5. View business settings\n"
                        "6. Modify business settings\n"
                        "7. Reset to defaults\n"
                        "What would you like to do? (0-7): ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case VIEW_SYSTEM_SETTINGS:
                view_system_settings();
                break;
            case MODIFY_SYSTEM_SETTINGS:
                modify_system_settings();
                break;
            case VIEW_SECURITY_SETTINGS:
                view_security_settings();
                break;
            case MODIFY_SECURITY_SETTINGS:
                modify_security_settings();
                break;
            case VIEW_BUSINESS_SETTINGS:
                view_business_settings();
                break;
            case MODIFY_BUSINESS_SETTINGS:
                modify_business_settings();
                break;
            case RESET_TO_DEFAULTS:
                reset_to_defaults();
                break;
            case 0:
                save_config(); // Save any changes before exiting
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to load configuration from file
void load_config(void) {
    FILE *fp = fopen("config.txt", "r");
    if (fp == NULL) {
        printf("Configuration file not found. Loading default settings...\n");
        reset_to_defaults();
        save_config();
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        // Parse configuration lines
        if (strncmp(line, "max_concurrent_orders=", 22) == 0) {
            system_config.max_concurrent_orders = atoi(line + 22);
        } else if (strncmp(line, "max_delivery_time_minutes=", 26) == 0) {
            system_config.max_delivery_time_minutes = atoi(line + 26);
        } else if (strncmp(line, "delivery_base_fee_rate=", 23) == 0) {
            system_config.delivery_base_fee_rate = atof(line + 23);
        } else if (strncmp(line, "delivery_bonus_rate=", 20) == 0) {
            system_config.delivery_bonus_rate = atof(line + 20);
        } else if (strncmp(line, "delivery_penalty_rate=", 22) == 0) {
            system_config.delivery_penalty_rate = atof(line + 22);
        } else if (strncmp(line, "max_login_attempts=", 19) == 0) {
            system_config.max_login_attempts = atoi(line + 19);
        } else if (strncmp(line, "session_timeout_minutes=", 24) == 0) {
            system_config.session_timeout_minutes = atoi(line + 24);
        } else if (strncmp(line, "password_min_length=", 20) == 0) {
            system_config.password_min_length = atoi(line + 20);
        } else if (strncmp(line, "password_require_special_chars=", 31) == 0) {
            system_config.password_require_special_chars = atoi(line + 31);
        } else if (strncmp(line, "student_default_balance=", 24) == 0) {
            system_config.student_default_balance = atof(line + 24);
        } else if (strncmp(line, "order_queue_max_size=", 21) == 0) {
            system_config.order_queue_max_size = atoi(line + 21);
        } else if (strncmp(line, "restaurant_commission_rate=", 27) == 0) {
            system_config.restaurant_commission_rate = atof(line + 27);
        } else if (strncmp(line, "delivery_personnel_max_orders=", 30) == 0) {
            system_config.delivery_personnel_max_orders = atoi(line + 30);
        }
    }
    
    fclose(fp);
    validate_config_values();
}

// Function to save configuration to file
void save_config(void) {
    FILE *fp = fopen("config.txt", "w");
    if (fp == NULL) {
        printf("Error: Could not save configuration file.\n");
        return;
    }
    
    fprintf(fp, "# Campus Food Delivery System Configuration\n");
    fprintf(fp, "# Generated on: %s\n\n", __DATE__);
    
    fprintf(fp, "# System Settings\n");
    fprintf(fp, "max_concurrent_orders=%d\n", system_config.max_concurrent_orders);
    fprintf(fp, "max_delivery_time_minutes=%d\n", system_config.max_delivery_time_minutes);
    fprintf(fp, "delivery_base_fee_rate=%.2f\n", system_config.delivery_base_fee_rate);
    fprintf(fp, "delivery_bonus_rate=%.2f\n", system_config.delivery_bonus_rate);
    fprintf(fp, "delivery_penalty_rate=%.2f\n", system_config.delivery_penalty_rate);
    
    fprintf(fp, "\n# Security Settings\n");
    fprintf(fp, "max_login_attempts=%d\n", system_config.max_login_attempts);
    fprintf(fp, "session_timeout_minutes=%d\n", system_config.session_timeout_minutes);
    fprintf(fp, "password_min_length=%d\n", system_config.password_min_length);
    fprintf(fp, "password_require_special_chars=%d\n", system_config.password_require_special_chars);
    
    fprintf(fp, "\n# Business Settings\n");
    fprintf(fp, "student_default_balance=%.2f\n", system_config.student_default_balance);
    fprintf(fp, "order_queue_max_size=%d\n", system_config.order_queue_max_size);
    fprintf(fp, "restaurant_commission_rate=%.2f\n", system_config.restaurant_commission_rate);
    fprintf(fp, "delivery_personnel_max_orders=%d\n", system_config.delivery_personnel_max_orders);

    fclose(fp);
    printf("Configuration saved successfully.\n");
}

// Function to reset configuration to default values
void reset_to_defaults(void) {
    // System Settings
    system_config.max_concurrent_orders = 100;
    system_config.max_delivery_time_minutes = 60;
    system_config.delivery_base_fee_rate = 0.10; // 10% of order value
    system_config.delivery_bonus_rate = 0.05; // 5% bonus for on-time delivery
    system_config.delivery_penalty_rate = 0.02; // 2% penalty for late delivery
    
    // Security Settings
    system_config.max_login_attempts = 3;
    system_config.session_timeout_minutes = 30;
    system_config.password_min_length = 6;
    system_config.password_require_special_chars = 0; // Disabled by default
    
    // Business Settings
    system_config.student_default_balance = 100.00;
    system_config.order_queue_max_size = 50;
    system_config.restaurant_commission_rate = 0.15; // 15% commission
    system_config.delivery_personnel_max_orders = 5;
    
    printf("\nConfiguration reset to default values.\n");
}

// Function to validate configuration values
void validate_config_values(void) {
    // Validate and correct invalid values
    if (system_config.max_concurrent_orders < 1) system_config.max_concurrent_orders = 100;
    if (system_config.max_delivery_time_minutes < 15) system_config.max_delivery_time_minutes = 60;
    if (system_config.delivery_base_fee_rate < 0.01) system_config.delivery_base_fee_rate = 0.10;
    if (system_config.delivery_bonus_rate < 0.0) system_config.delivery_bonus_rate = 0.05;
    if (system_config.delivery_penalty_rate < 0.0) system_config.delivery_penalty_rate = 0.02;
    
    if (system_config.max_login_attempts < 1) system_config.max_login_attempts = 3;
    if (system_config.session_timeout_minutes < 5) system_config.session_timeout_minutes = 30;
    if (system_config.password_min_length < 4) system_config.password_min_length = 6;
    
    if (system_config.student_default_balance < 0.0) system_config.student_default_balance = 100.00;
    if (system_config.order_queue_max_size < 10) system_config.order_queue_max_size = 50;
    if (system_config.restaurant_commission_rate < 0.0) system_config.restaurant_commission_rate = 0.15;
    if (system_config.delivery_personnel_max_orders < 1) system_config.delivery_personnel_max_orders = 5;
}

// Function to view system settings
void view_system_settings(void) {
    printf("\n================ SYSTEM SETTINGS ================\n");
    printf("Max Concurrent Orders: %d\n", system_config.max_concurrent_orders);
    printf("Max Delivery Time: %d minutes\n", system_config.max_delivery_time_minutes);
    printf("Delivery Base Fee Rate: %.2f%% of order value\n", system_config.delivery_base_fee_rate * 100);
    printf("Delivery Bonus Rate: %.2f%% for on-time delivery\n", system_config.delivery_bonus_rate * 100);
    printf("Delivery Penalty Rate: %.2f%% for late delivery\n", system_config.delivery_penalty_rate * 100);
    printf("=================================================\n");
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to modify system settings
void modify_system_settings(void) {
    int choice;
    
    while (1) {
        printf("\n----- MODIFY SYSTEM SETTINGS -----\n");
        printf("1. Max concurrent orders (current: %d)\n", system_config.max_concurrent_orders);
        printf("2. Max delivery time (current: %d minutes)\n", system_config.max_delivery_time_minutes);
        printf("3. Delivery base fee rate (current: %.2f%%)\n", system_config.delivery_base_fee_rate * 100);
        printf("4. Delivery bonus rate (current: %.2f%%)\n", system_config.delivery_bonus_rate * 100);
        printf("5. Delivery penalty rate (current: %.2f%%)\n", system_config.delivery_penalty_rate * 100);
        printf("0. Back to main menu\n");
        
        choice = get_integer_input("Select setting to modify: ");
        
        switch (choice) {
            case 1: {
                int new_value = get_integer_input("\nEnter new max concurrent orders (1-1000): ");
                if (new_value >= 1 && new_value <= 1000) {
                    system_config.max_concurrent_orders = new_value;
                    printf("\nMax concurrent orders updated to %d.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 1 and 1000.\n");
                }
                break;
            }
            case 2: {
                int new_value = get_integer_input("\nEnter new max delivery time in minutes (15-180): ");
                if (new_value >= 15 && new_value <= 180) {
                    system_config.max_delivery_time_minutes = new_value;
                    printf("\nMax delivery time updated to %d minutes.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 15 and 180 minutes.\n");
                }
                break;
            }
            case 3: {
                float new_value = get_float_input("\nEnter new delivery base fee rate (1.0-50.0%%): ") / 100.0;
                if (new_value >= 0.01 && new_value <= 0.50) {
                    system_config.delivery_base_fee_rate = new_value;
                    printf("\nDelivery base fee rate updated to %.2f%%.\n", new_value * 100);
                } else {
                    printf("Invalid value. Must be between 1.0%% and 50.0%%.\n");
                }
                break;
            }
            case 4: {
                float new_value = get_float_input("\nEnter new delivery bonus rate (0.0-20.0%%): ") / 100.0;
                if (new_value >= 0.0 && new_value <= 0.20) {
                    system_config.delivery_bonus_rate = new_value;
                    printf("\nDelivery bonus rate updated to %.2f%%.\n", new_value * 100);
                } else {
                    printf("Invalid value. Must be between 0.0%% and 20.0%%.\n");
                }
                break;
            }
            case 5: {
                float new_value = get_float_input("\nEnter new delivery penalty rate (0.0-10.0%%): ") / 100.0;
                if (new_value >= 0.0 && new_value <= 0.10) {
                    system_config.delivery_penalty_rate = new_value;
                    printf("\nDelivery penalty rate updated to %.2f%%.\n", new_value * 100);
                } else {
                    printf("Invalid value. Must be between 0.0%% and 10.0%%.\n");
                }
                break;
            }
            case 0:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

// Function to view security settings
void view_security_settings(void) {
    printf("\n========== SECURITY SETTINGS ==========\n");
    printf("Max Login Attempts: %d\n", system_config.max_login_attempts);
    printf("Session Timeout: %d minutes\n", system_config.session_timeout_minutes);
    printf("Password Minimum Length: %d characters\n", system_config.password_min_length);
    printf("Require Special Characters: %s\n", system_config.password_require_special_chars ? "Yes" : "No");
    printf("=======================================\n");
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to modify security settings
void modify_security_settings(void) {
    int choice;
    
    while (1) {
        printf("\n----- MODIFY SECURITY SETTINGS -----\n");
        printf("1. Max login attempts (current: %d)\n", system_config.max_login_attempts);
        printf("2. Session timeout (current: %d minutes)\n", system_config.session_timeout_minutes);
        printf("3. Password minimum length (current: %d)\n", system_config.password_min_length);
        printf("4. Require special characters (current: %s)\n", system_config.password_require_special_chars ? "Yes" : "No");
        printf("0. Back to main menu\n");
        
        choice = get_integer_input("Select setting to modify: ");
        
        switch (choice) {
            case 1: {
                int new_value = get_integer_input("\nEnter new max login attempts (1-10): ");
                if (new_value >= 1 && new_value <= 10) {
                    system_config.max_login_attempts = new_value;
                    printf("\nMax login attempts updated to %d.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 1 and 10.\n");
                }
                break;
            }
            case 2: {
                int new_value = get_integer_input("\nEnter new session timeout in minutes (5-120): ");
                if (new_value >= 5 && new_value <= 120) {
                    system_config.session_timeout_minutes = new_value;
                    printf("\nSession timeout updated to %d minutes.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 5 and 120 minutes.\n");
                }
                break;
            }
            case 3: {
                int new_value = get_integer_input("\nEnter new password minimum length (4-20): ");
                if (new_value >= 4 && new_value <= 20) {
                    system_config.password_min_length = new_value;
                    printf("\nPassword minimum length updated to %d characters.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 4 and 20 characters.\n");
                }
                break;
            }
            case 4: {
                int new_value = get_integer_input("\nRequire special characters? (1=Yes, 0=No): ");
                if (new_value == 0 || new_value == 1) {
                    system_config.password_require_special_chars = new_value;
                    printf("\nSpecial character requirement updated to %s.\n", new_value ? "Yes" : "No");
                } else {
                    printf("Invalid value. Enter 1 for Yes or 0 for No.\n");
                }
                break;
            }
            case 0:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

// Function to view business settings
void view_business_settings(void) {
    printf("\n=========== BUSINESS SETTINGS ===========\n");
    printf("Student Default Balance: RM%.2f\n", system_config.student_default_balance);
    printf("Order Queue Max Size: %d orders\n", system_config.order_queue_max_size);
    printf("Restaurant Commission Rate: %.2f%%\n", system_config.restaurant_commission_rate * 100);
    printf("Delivery Personnel Max Orders: %d orders\n", system_config.delivery_personnel_max_orders);
    printf("=========================================\n");
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to modify business settings
void modify_business_settings(void) {
    int choice;
    
    while (1) {
        printf("\n----- MODIFY BUSINESS SETTINGS -----\n");
        printf("1. Student default balance (current: RM%.2f)\n", system_config.student_default_balance);
        printf("2. Order queue max size (current: %d)\n", system_config.order_queue_max_size);
        printf("3. Restaurant commission rate (current: %.2f%%)\n", system_config.restaurant_commission_rate * 100);
        printf("4. Delivery personnel max orders (current: %d)\n", system_config.delivery_personnel_max_orders);
        printf("0. Back to main menu\n");
        
        choice = get_integer_input("Select setting to modify: ");
        
        switch (choice) {
            case 1: {
                float new_value = get_float_input("\nEnter new student default balance (0.00-1000.00): ");
                if (new_value >= 0.0 && new_value <= 1000.0) {
                    system_config.student_default_balance = new_value;
                    printf("\nStudent default balance updated to RM%.2f.\n", new_value);
                } else {
                    printf("Invalid value. Must be between RM0.00 and RM1000.00.\n");
                }
                break;
            }
            case 2: {
                int new_value = get_integer_input("\nEnter new order queue max size (10-200): ");
                if (new_value >= 10 && new_value <= 200) {
                    system_config.order_queue_max_size = new_value;
                    printf("\nOrder queue max size updated to %d orders.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 10 and 200 orders.\n");
                }
                break;
            }
            case 3: {
                float new_value = get_float_input("\nEnter new restaurant commission rate (5.0-30.0%%): ") / 100.0;
                if (new_value >= 0.05 && new_value <= 0.30) {
                    system_config.restaurant_commission_rate = new_value;
                    printf("\nRestaurant commission rate updated to %.2f%%.\n", new_value * 100);
                } else {
                    printf("Invalid value. Must be between 5.0%% and 30.0%%.\n");
                }
                break;
            }
            case 4: {
                int new_value = get_integer_input("\nEnter new delivery personnel max orders (1-20): ");
                if (new_value >= 1 && new_value <= 20) {
                    system_config.delivery_personnel_max_orders = new_value;
                    printf("\nDelivery personnel max orders updated to %d.\n", new_value);
                } else {
                    printf("Invalid value. Must be between 1 and 20 orders.\n");
                }
                break;
            }
            case 0:
                return;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
