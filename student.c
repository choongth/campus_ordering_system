#include "student.h"

#define VIEW_PROFILE 1
#define ADD_PROFILE 2
#define UPDATE_PROFILE 3
#define DELETE_PROFILE 4

extern char current_student_id[STUDENT_ID_LENGTH];

// Forward declarations
void view_student_profile(char *student_id);
int add_student_profile(char *student_id);
int update_student_profile(char *student_id);
int delete_student_profile(char *student_id);
int find_student(char *student_id, Student *students, int *count);
void display_student_info(Student *students, int count);
void top_up_account_balance(int top_up_amount);
void update_account_balance(char *student_id, float amount);
int check_account_activation_status(char *student_id);
const char* get_activation_status_text(AccountActivateStatus status);
extern void get_restaurant_info(char *restaurant_id, char *restaurant_name, char *restaurant_contact);
extern void get_order_items_summary(char *order_id, char *summary, int summary_size);

int profile_management(void) {
    int choice;
    while (1) {
        char prompt[] = "\n----- PROFILE MANAGEMENT -----\n"
                        "1. View profile\n"
                        "2. Add personal information\n"
                        "3. Update profile\n"
                        "4. Delete personal information\n"
                        "0. Exit\n"
                        "What would you like to do?: ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case VIEW_PROFILE:
                view_student_profile(current_student_id);
                break;
            case ADD_PROFILE:
                add_student_profile(current_student_id);
                break;
            case UPDATE_PROFILE:
                update_student_profile(current_student_id);
                break;
            case DELETE_PROFILE:
                delete_student_profile(current_student_id);
                break;
            case EXIT:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}


void view_student_profile(char *student_id) {
    Student students[100];
    int count;
    printf("\n----- VIEW STUDENT PROFILE -----\n");

    if (find_student(student_id, students, &count) == FILE_ERROR) return;

    display_student_info(students, count);
    
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

int add_student_profile(char *student_id) {
    printf("\nProfile information is automatically created during registration.\n");
    printf("Use 'Update profile' to modify your information.\n");
    return 0;
}

int update_student_profile(char *student_id) {
    printf("\n----- UPDATE STUDENT PROFILE -----\n");
    
    Student students[1];
    int count;
    
    // Find current student
    if (find_student(student_id, students, &count) == FILE_ERROR || count == 0) {
        printf("Error: Could not find student profile. Returning to profile management menu.\n");
        return 0;
    }
    
    Student current_student = students[0];
    
    // Display current profile information
    printf("\n--- CURRENT PROFILE INFORMATION ---\n");
    printf("Student ID: %s\n", current_student.student_id);
    printf("Name: %s\n", current_student.user.name);
    printf("Contact Number: %s\n", current_student.user.contact_number);
    printf("Email Address: %s\n", current_student.user.email_address);
    printf("Account Status: %s\n", (current_student.account_activate_status == ACTIVATE) ? "Active" : "Inactive");
    printf("Account Balance: RM%.2f\n", current_student.account_balance);
    
    // Update options
    printf("\n--- UPDATE OPTIONS ---\n");
    printf("1. Update name\n");
    printf("2. Update contact number\n");
    printf("3. Update email address\n");
    printf("4. Change password\n");
    printf("0. Cancel\n");
    
    int choice = get_integer_input("Select field to update: ");
    
    switch (choice) {
        case 1: {
            printf("Current name: %s\n", current_student.user.name);
            printf("Enter new name: ");
            char new_name[NAME_LENGTH];
            getchar(); // Clear buffer
            fgets(new_name, sizeof(new_name), stdin);
            new_name[strcspn(new_name, "\n")] = '\0';
            
            if (strlen(new_name) == 0 || strlen(new_name) >= NAME_LENGTH) {
                printf("Invalid name. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            strcpy(current_student.user.name, new_name);
            printf("Name will be updated to: %s\n", new_name);
            break;
        }
        
        case 2: {
            printf("Current contact: %s\n", current_student.user.contact_number);
            char new_contact[CONTACT_NUMBER_LENGTH];
            
            if (!safe_contact_input("Enter new contact number: ", new_contact, sizeof(new_contact))) {
                printf("Invalid contact number. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            strcpy(current_student.user.contact_number, new_contact);
            printf("Contact number will be updated to: %s\n", new_contact);
            break;
        }
        
        case 3: {
            printf("Current email: %s\n", current_student.user.email_address);
            char new_email[EMAIL_ADDRESS_LENGTH];
            
            if (!safe_email_input("Enter new email address: ", new_email, sizeof(new_email))) {
                printf("Invalid email address. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            // Check if email already exists (excluding current user)
            FILE *user_fp = fopen(USER_FILE, "r");
            if (user_fp != NULL) {
                char line[MAX_LINE_LENGTH];
                int email_exists = 0;
                
                while (fgets(line, sizeof(line), user_fp)) {
                    char uid[USER_ID_LENGTH], email[EMAIL_ADDRESS_LENGTH];
                    int user_type;
                    char name[NAME_LENGTH], contact[CONTACT_NUMBER_LENGTH], password[PASSWORD_LENGTH];
                    
                    sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%s",
                           uid, &user_type, name, contact, email, password);
                    
                    if (strcmp(email, new_email) == 0 && strcmp(email, current_student.user.email_address) != 0) {
                        email_exists = 1;
                        break;
                    }
                }
                fclose(user_fp);
                
                if (email_exists) {
                    printf("Email address already exists. Update cancelled. Returning to profile management menu.\n");
                    return 0;
                }
            }
            
            strcpy(current_student.user.email_address, new_email);
            printf("Email address will be updated to: %s\n", new_email);
            break;
        }
        
        case 4: {
            char current_password[PASSWORD_LENGTH];
            if (!safe_password_input("Enter current password: ", current_password, sizeof(current_password))) {
                printf("Invalid password format. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            if (strcmp(current_password, current_student.user.password) != 0) {
                printf("Incorrect current password. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            char new_password[PASSWORD_LENGTH];
            if (!safe_password_input("Enter new password: ", new_password, sizeof(new_password))) {
                printf("Invalid password format. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            char confirm_password[PASSWORD_LENGTH];
            if (!safe_password_input("Confirm new password: ", confirm_password, sizeof(confirm_password))) {
                printf("Invalid password format. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            if (strcmp(new_password, confirm_password) != 0) {
                printf("Passwords do not match. Update cancelled. Returning to profile management menu.\n");
                return 0;
            }
            
            strcpy(current_student.user.password, new_password);
            printf("Password will be updated.\n");
            break;
        }
        
        case 0:
            printf("Update cancelled. Returning to profile management menu.\n");
            return 0;
            
        default:
            printf("Invalid choice. Update cancelled. Returning to profile management menu.\n");
            return 0;
    }
    
    // Confirm update
    int confirm = confirmation("Save changes to your profile? (1. Yes / 2. No): ");
    if (confirm != YES) {
        printf("Update cancelled. Returning to profile management menu.\n");
        return 0;
    }
    
    // Update Student.txt file
    FILE *student_fp = fopen(STUDENT_FILE, "r");
    if (student_fp == NULL) {
        printf("Error: Could not open student file for update.\n");
        return 0;
    }
    
    FILE *temp_student_fp = fopen("temp_student.txt", "w");
    if (temp_student_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(student_fp);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int updated = 0;
    
    while (fgets(line, sizeof(line), student_fp)) {
        char sid[STUDENT_ID_LENGTH];
        sscanf(line, "%[^,]", sid);
        
        if (strcmp(sid, student_id) == 0) {
            fprintf(temp_student_fp, "%s,%s,%s,%s,%s,%d,%.2f\n",
                    current_student.student_id, current_student.user.name,
                    current_student.user.contact_number, current_student.user.email_address,
                    current_student.user.password, current_student.account_activate_status,
                    current_student.account_balance);
            updated = 1;
        } else {
            fprintf(temp_student_fp, "%s", line);
        }
    }
    
    fclose(student_fp);
    fclose(temp_student_fp);
    
    if (updated) {
        remove(STUDENT_FILE);
        rename("temp_student.txt", STUDENT_FILE);
        
        // Update User.txt file
        FILE *user_fp = fopen(USER_FILE, "r");
        if (user_fp != NULL) {
            FILE *temp_user_fp = fopen("temp_user.txt", "w");
            if (temp_user_fp != NULL) {
                char user_line[MAX_LINE_LENGTH];
                
                while (fgets(user_line, sizeof(user_line), user_fp)) {
                    char uid[USER_ID_LENGTH], email[EMAIL_ADDRESS_LENGTH];
                    int user_type;
                    char name[NAME_LENGTH], contact[CONTACT_NUMBER_LENGTH], password[PASSWORD_LENGTH];
                    
                    sscanf(user_line, "%[^,],%d,%[^,],%[^,],%[^,],%s",
                           uid, &user_type, name, contact, email, password);
                    
                    if (strcmp(email, current_student.user.email_address) == 0 || 
                        (choice == 3 && strstr(user_line, current_student.user.name) != NULL)) {
                        fprintf(temp_user_fp, "%s,%d,%s,%s,%s,%s\n",
                                uid, user_type, current_student.user.name,
                                current_student.user.contact_number, current_student.user.email_address,
                                current_student.user.password);
                    } else {
                        fprintf(temp_user_fp, "%s", user_line);
                    }
                }
                
                fclose(user_fp);
                fclose(temp_user_fp);
                remove(USER_FILE);
                rename("temp_user.txt", USER_FILE);
            } else {
                fclose(user_fp);
            }
        }
        
        printf("\nProfile updated successfully!\n");
        printf("Your changes have been saved.\n");
        return 1;
    } else {
        remove("temp_student.txt");
        printf("Error: Failed to update profile.\n");
        return 0;
    }
}

int delete_student_profile(char *student_id) {
    printf("\n----- DELETE STUDENT PROFILE -----\n");
    printf("WARNING: This action will permanently delete your account!\n");
    printf("This action cannot be undone!\n");
    
    Student students[1];
    int count;
    
    // Find current student
    if (find_student(student_id, students, &count) == FILE_ERROR || count == 0) {
        printf("Error: Could not find student profile.\n");
        return 0;
    }
    
    Student current_student = students[0];
    
    // Display profile information
    printf("\n--- ACCOUNT TO BE DELETED ---\n");
    printf("Student ID: %s\n", current_student.student_id);
    printf("Name: %s\n", current_student.user.name);
    printf("Email: %s\n", current_student.user.email_address);
    printf("Account Balance: RM%.2f\n", current_student.account_balance);
    
    // Check if student has pending orders
    FILE *order_check_fp = fopen(ORDER_FILE, "r");
    int pending_orders = 0;
    
    if (order_check_fp != NULL) {
        char order_line[MAX_LINE_LENGTH];
        
        while (fgets(order_line, sizeof(order_line), order_check_fp)) {
            char order_id[ORDER_ID_LENGTH], sid[STUDENT_ID_LENGTH];
            char restaurant_id[RESTAURANT_ID_LENGTH], status[20];
            float total_price;
            char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
            
            sscanf(order_line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                   order_id, sid, restaurant_id, &total_price,
                   order_date, order_time, status);
            
            if (strcmp(sid, student_id) == 0 && 
                (strcmp(status, "PREPARING") == 0 || 
                 strcmp(status, "READY") == 0 || 
                 strcmp(status, "DELIVERING") == 0)) {
                pending_orders++;
            }
        }
        fclose(order_check_fp);
        
        if (pending_orders > 0) {
            printf("\nWARNING: You have %d pending order(s).\n", pending_orders);
            printf("Deleting your account will cancel all pending orders.\n");
        }
    }
    
    if (current_student.account_balance > 0) {
        printf("\nWARNING: Your account has a balance of RM%.2f.\n", current_student.account_balance);
        printf("This balance will be lost if you delete your account.\n");
    }
    
    printf("\nDeletion will remove:\n");
    printf("1. Your student profile and account\n");
    printf("2. All order history\n");
    printf("3. Account balance (if any)\n");
    printf("4. Access to the system\n");
    
    // First confirmation
    int confirm1 = confirmation("\nAre you sure you want to delete your account? (1. Yes / 2. No): ");
    if (confirm1 != YES) {
        printf("Account deletion cancelled.\n");
        return 0;
    }
    
    // Second confirmation for safety
    printf("\nFINAL WARNING: This will permanently delete your account!\n");
    int confirm2 = confirmation("Type 1 again to confirm permanent deletion: ");
    if (confirm2 != YES) {
        printf("Account deletion cancelled.\n");
        return 0;
    }
    
    // Password verification for security
    char password_verify[PASSWORD_LENGTH];
    if (!safe_password_input("\nFor security, please enter your password: ", password_verify, sizeof(password_verify))) {
        printf("Invalid password format. Account deletion cancelled.\n");
        return 0;
    }
    
    if (strcmp(password_verify, current_student.user.password) != 0) {
        printf("Incorrect password. Account deletion cancelled.\n");
        return 0;
    }
    
    printf("\nDeleting account...\n");
    
    // Delete from Student.txt
    FILE *student_fp = fopen(STUDENT_FILE, "r");
    if (student_fp == NULL) {
        printf("Error: Could not open student file.\n");
        return 0;
    }
    
    FILE *temp_student_fp = fopen("temp_student.txt", "w");
    if (temp_student_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(student_fp);
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int deleted_from_student = 0;
    
    while (fgets(line, sizeof(line), student_fp)) {
        char sid[STUDENT_ID_LENGTH];
        sscanf(line, "%[^,]", sid);
        
        if (strcmp(sid, student_id) != 0) {
            fprintf(temp_student_fp, "%s", line);
        } else {
            deleted_from_student = 1;
        }
    }
    
    fclose(student_fp);
    fclose(temp_student_fp);
    
    if (deleted_from_student) {
        remove(STUDENT_FILE);
        rename("temp_student.txt", STUDENT_FILE);
        printf("Removed from student records\n");
    } else {
        remove("temp_student.txt");
        printf("Failed to remove from student records\n");
        return 0;
    }
    
    // Delete from User.txt
    FILE *user_fp = fopen(USER_FILE, "r");
    if (user_fp != NULL) {
        FILE *temp_user_fp = fopen("temp_user.txt", "w");
        if (temp_user_fp != NULL) {
            char user_line[MAX_LINE_LENGTH];
            int deleted_from_user = 0;
            
            while (fgets(user_line, sizeof(user_line), user_fp)) {
                char uid[USER_ID_LENGTH], email[EMAIL_ADDRESS_LENGTH];
                int user_type;
                char name[NAME_LENGTH], contact[CONTACT_NUMBER_LENGTH], password[PASSWORD_LENGTH];
                
                sscanf(user_line, "%[^,],%d,%[^,],%[^,],%[^,],%s",
                       uid, &user_type, name, contact, email, password);
                
                if (strcmp(email, current_student.user.email_address) != 0) {
                    fprintf(temp_user_fp, "%s", user_line);
                } else {
                    deleted_from_user = 1;
                }
            }
            
            fclose(user_fp);
            fclose(temp_user_fp);
            
            if (deleted_from_user) {
                remove(USER_FILE);
                rename("temp_user.txt", USER_FILE);
                printf("Removed from user records\n");
            } else {
                remove("temp_user.txt");
                printf("Warning: Could not remove from user records\n");
            }
        } else {
            fclose(user_fp);
        }
    }
    
    // Cancel pending orders (update Order.txt)
    FILE *order_fp = fopen(ORDER_FILE, "r");
    if (order_fp != NULL) {
        FILE *temp_order_fp = fopen("temp_order.txt", "w");
        if (temp_order_fp != NULL) {
            char order_line[MAX_LINE_LENGTH];
            int cancelled_orders = 0;
            
            while (fgets(order_line, sizeof(order_line), order_fp)) {
                char order_id[ORDER_ID_LENGTH], sid[STUDENT_ID_LENGTH];
                char restaurant_id[RESTAURANT_ID_LENGTH], status[20];
                float total_price;
                char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
                
                sscanf(order_line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                       order_id, sid, restaurant_id, &total_price,
                       order_date, order_time, status);
                
                if (strcmp(sid, student_id) == 0 && 
                    (strcmp(status, "PREPARING") == 0 || strcmp(status, "READY") == 0 || strcmp(status, "DELIVERING") == 0)) {
                    // Cancel the order
                    fprintf(temp_order_fp, "%s,%s,%s,%.2f,%s,%s,CANCELLED\n",
                            order_id, sid, restaurant_id, total_price, order_date, order_time);
                    cancelled_orders++;
                } else {
                    fprintf(temp_order_fp, "%s", order_line);
                }
            }
            
            fclose(order_fp);
            fclose(temp_order_fp);
            
            if (cancelled_orders > 0) {
                remove(ORDER_FILE);
                rename("temp_order.txt", ORDER_FILE);
                printf("Cancelled %d pending order(s)\n", cancelled_orders);
            } else {
                remove("temp_order.txt");
            }
        } else {
            fclose(order_fp);
        }
    }
    
    printf("\nAccount deleted successfully!\n");
    printf("Your student account has been permanently removed from the system.\n");
    printf("Thank you for using the Campus Food Delivery System.\n");
    
    // Clear current student ID to prevent further operations
    strcpy(current_student_id, "");
    
    printf("\nYou will be logged out automatically.\n");
    printf("Press Enter to continue...");
    getchar();
    getchar();
    
    return 1;
}

int find_student(char *student_id, Student *students, int *count) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", STUDENT_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        Student temp_student;
        char *token;
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy));

        token = strtok(line_copy, ",");
        if (!token) continue;
        strncpy(temp_student.student_id, token, STUDENT_ID_LENGTH);
        temp_student.student_id[STUDENT_ID_LENGTH-1] = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(temp_student.user.name, token, NAME_LENGTH);
        temp_student.user.name[NAME_LENGTH-1] = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(temp_student.user.contact_number, token, CONTACT_NUMBER_LENGTH);
        temp_student.user.contact_number[CONTACT_NUMBER_LENGTH-1] = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(temp_student.user.email_address, token, EMAIL_ADDRESS_LENGTH);
        temp_student.user.email_address[EMAIL_ADDRESS_LENGTH-1] = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(temp_student.user.password, token, PASSWORD_LENGTH);
        temp_student.user.password[PASSWORD_LENGTH-1] = '\0';

        token = strtok(NULL, ",");
        if (!token) continue;
        temp_student.account_activate_status = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        temp_student.account_balance = atof(token);

        if (strcmp(student_id, temp_student.student_id) == 0) {
            students[*count] = temp_student;
            (*count)++;
            break;
        }
    }

    fclose(fp);
    return *count;
}

void display_student_info(Student *students, int count) {
    if (count == 0) {
        printf("No student found.\n");
        return;
    }

    printf("\n--------------------------------------- STUDENT INFORMATION ---------------------------------------\n");
    printf("%-10s %-20s %-25s %-15s %-12s %-10s\n", "ID", "Name", "Email Address", "Contact Number", "Status", "Balance (RM)");
    printf("---------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-10s %-20s %-25s %-15s %-12s %.2f\n", 
               students[i].student_id, 
               students[i].user.name, 
               students[i].user.email_address, 
               students[i].user.contact_number,
               get_activation_status_text(students[i].account_activate_status),
               students[i].account_balance);
    }
    printf("---------------------------------------------------------------------------------------------------\n");
}

const char* get_activation_status_text(AccountActivateStatus status) {
    switch (status) {
        case ACTIVATE: return "Active";
        case DEACTIVATE: return "Deactivated";
        default: return "Unknown";
    }
}

int check_account_activation_status(char *student_id) {
    Student students[10]; // increase array size to prevent out-of-bound
    int count;
    if (find_student(student_id, students, &count) != FILE_ERROR && count > 0) {
        return students[0].account_activate_status;
    }
    return DEACTIVATE; // Default to deactivated if not found
}



// --------------------------------------------------------------------------------------------

// Forward declarations for order placement
void browse_menu_items(MenuItemDisplay *menu_items, int *item_count);
void display_menu_for_ordering(MenuItemDisplay *menu_items, int item_count);
int select_items_for_order(MenuItemDisplay *menu_items, int item_count, OrderItem *order_items, int *order_count);
float calculate_total_cost(OrderItem *order_items, int order_count);
int process_payment(float total_cost);
void generate_order_confirmation(OrderItem *order_items, int order_count, float total_cost);
const char* get_cuisine_name_from_type(int cuisine_type);

int order_placement(void) {
    printf("\n----- ORDER PLACEMENT -----\n");
    
    // Check if student account is activated
    int activation_status = check_account_activation_status(current_student_id);
    if (activation_status == DEACTIVATE) {
        printf("Your account is deactivated. Please contact administrator to activate your account.\n");
        printf("You cannot place orders until your account is activated.\n");
        return 0;
    }
    
    // Check current balance
    Student students[10]; // increase array size to prevent out-of-bounds
    int count;
    float current_balance = 0.0;
    if (find_student(current_student_id, students, &count) != FILE_ERROR && count > 0) {
        current_balance = students[0].account_balance;
        printf("Your current account balance: RM%.2f\n", current_balance);
    }
    
    if (current_balance <= 0) {
        printf("Insufficient balance. Please top up your account before placing an order.\n");
        return 0;
    }
    
    MenuItemDisplay menu_items[200];
    int item_count = 0;
    
    // Browse available menu items
    browse_menu_items(menu_items, &item_count);
    
    if (item_count == 0) {
        printf("No menu items available at the moment.\n");
        return 0;
    }
    
    // Display menu and allow selection
    OrderItem order_items[50];
    int order_count = 0;
    
    if (!select_items_for_order(menu_items, item_count, order_items, &order_count)) {
        printf("Order cancelled.\n");
        return 0;
    }
    
    if (order_count == 0) {
        printf("No items selected. Order cancelled.\n");
        return 0;
    }
    
    // Calculate total cost
    float total_cost = calculate_total_cost(order_items, order_count);
    
    // Process payment
    if (process_payment(total_cost)) {
        generate_order_confirmation(order_items, order_count, total_cost);
        printf("Order placed successfully!\n");
    } else {
        printf("Payment failed. Order cancelled.\n");
    }
    
    return 0;
}

// Helper function to get cuisine name from type
const char* get_cuisine_name_from_type(int cuisine_type) {
    switch (cuisine_type) {
        case 1: return "Chinese";
        case 2: return "Western";
        case 3: return "Arabic";
        case 4: return "Japanese";
        case 5: return "Korean";
        case 6: return "Asian";
        case 7: return "Indian";
        case 8: return "Malay";
        case 9: return "African";
        case 10: return "Fast Food";
        case 11: return "Desserts";
        default: return "Unknown";
    }
}

// Function to browse and load all menu items
void browse_menu_items(MenuItemDisplay *menu_items, int *item_count) {
    *item_count = 0;

    // First, load restaurant information
    FILE *restaurant_fp = fopen(RESTAURANT_FILE, "r");
    if (restaurant_fp == NULL) {
        printf("Error: Could not open restaurant file.\n");
        return;
    }
    
    // Load menu items
    FILE *menu_fp = fopen(MENU_ITEM_FILE, "r");
    if (menu_fp == NULL) {
        printf("Error: Could not open menu item file.\n");
        fclose(restaurant_fp);
        return;
    }
    
    char menu_line[MAX_LINE_LENGTH];
    while (fgets(menu_line, sizeof(menu_line), menu_fp) && *item_count < 200) {
        menu_line[strcspn(menu_line, "\n")] = '\0';
        
        char restaurant_id[RESTAURANT_ID_LENGTH];
        char menu_item_id[MENU_ITEM_ID_LENGTH];
        char item_name[NAME_LENGTH];
        float price;
        
        sscanf(menu_line, "%[^,],%[^,],%[^,],%f", 
               restaurant_id, menu_item_id, item_name, &price);
        
        // Find restaurant details
        rewind(restaurant_fp);
        char restaurant_line[MAX_LINE_LENGTH];
        while (fgets(restaurant_line, sizeof(restaurant_line), restaurant_fp)) {
            restaurant_line[strcspn(restaurant_line, "\n")] = '\0';
            
            char rid[RESTAURANT_ID_LENGTH], rname[NAME_LENGTH];
            char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
            int cuisine_type;
            
            sscanf(restaurant_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d", 
                   rid, rname, contact, email, password, &cuisine_type);
            
            if (strcmp(rid, restaurant_id) == 0) {
                // Store menu item with restaurant details
                strcpy(menu_items[*item_count].menu_item_id, menu_item_id);
                strcpy(menu_items[*item_count].restaurant_id, restaurant_id);
                strcpy(menu_items[*item_count].restaurant_name, rname);
                strcpy(menu_items[*item_count].cuisine_name, get_cuisine_name_from_type(cuisine_type));
                strcpy(menu_items[*item_count].item_name, item_name);
                menu_items[*item_count].item_price = price;
                (*item_count)++;
                break;
            }
        }
    }
    
    fclose(menu_fp);
    fclose(restaurant_fp);
}

// Function to display menu items for ordering
void display_menu_for_ordering(MenuItemDisplay *menu_items, int item_count) {
    printf("\n============================= AVAILABLE MENU ITEMS =============================\n");
    printf("%-4s %-8s %-20s %-12s %-20s %-10s\n", 
           "No.", "Item ID", "Restaurant", "Cuisine", "Item Name", "Price (RM)");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < item_count; i++) {
        printf("%-4d %-8s %-20s %-12s %-20s RM%.2f\n", 
               i + 1, menu_items[i].menu_item_id, menu_items[i].restaurant_name,
               menu_items[i].cuisine_name, menu_items[i].item_name, menu_items[i].item_price);
    }
    printf("--------------------------------------------------------------------------------\n");
    printf("Total available items: %d\n", item_count);
}

// Function to select items for order
int select_items_for_order(MenuItemDisplay *menu_items, int item_count, OrderItem *order_items, int *order_count) {
    *order_count = 0;
    
    while (1) {
        display_menu_for_ordering(menu_items, item_count);
        
        printf("\nCurrent order items: %d\n", *order_count);
        if (*order_count > 0) {
            printf("Current order:\n");
            for (int i = 0; i < *order_count; i++) {
                printf("- %s x%d (RM%.2f each) = RM%.2f\n", 
                       order_items[i].item_name, order_items[i].quantity,
                       order_items[i].item_price, order_items[i].subtotal);
            }
        }
        
        printf("\nOptions:\n");
        printf("1. Add item to order\n");
        printf("2. Remove item from order\n");
        printf("3. Proceed to checkout\n");
        printf("0. Cancel order\n");
        
        int choice = get_integer_input("Enter your choice: ");
        
        switch (choice) {
            case 1: {
                if (*order_count >= 50) {
                    printf("Maximum order items reached (50).\n");
                    break;
                }
                
                int item_num = get_integer_input("Enter item number to add: ");
                if (item_num < 1 || item_num > item_count) {
                    printf("Invalid item number.\n");
                    break;
                }
                
                int quantity = get_integer_input("Enter quantity: ");
                if (quantity <= 0) {
                    printf("Invalid quantity.\n");
                    break;
                }
                
                // Check if item already in order
                int found = -1;
                for (int i = 0; i < *order_count; i++) {
                    if (strcmp(order_items[i].menu_item_id, menu_items[item_num - 1].menu_item_id) == 0) {
                        found = i;
                        break;
                    }
                }
                
                if (found >= 0) {
                    // Update existing item
                    order_items[found].quantity += quantity;
                    order_items[found].subtotal = order_items[found].quantity * order_items[found].item_price;
                    printf("Updated quantity for %s\n", order_items[found].item_name);
                } else {
                    // Add new item
                    MenuItemDisplay *selected = &menu_items[item_num - 1];
                    strcpy(order_items[*order_count].menu_item_id, selected->menu_item_id);
                    strcpy(order_items[*order_count].restaurant_id, selected->restaurant_id);
                    strcpy(order_items[*order_count].restaurant_name, selected->restaurant_name);
                    strcpy(order_items[*order_count].cuisine_name, selected->cuisine_name);
                    strcpy(order_items[*order_count].item_name, selected->item_name);
                    order_items[*order_count].item_price = selected->item_price;
                    order_items[*order_count].quantity = quantity;
                    order_items[*order_count].subtotal = quantity * selected->item_price;
                    (*order_count)++;
                    printf("Added %s to your order\n", selected->item_name);
                }
                break;
            }
            case 2: {
                if (*order_count == 0) {
                    printf("No items in order to remove.\n");
                    break;
                }
                
                printf("Current order items:\n");
                for (int i = 0; i < *order_count; i++) {
                    printf("%d. %s x%d\n", i + 1, order_items[i].item_name, order_items[i].quantity);
                }
                
                int remove_num = get_integer_input("Enter item number to remove: ");
                if (remove_num < 1 || remove_num > *order_count) {
                    printf("Invalid item number.\n");
                    break;
                }
                
                // Remove item by shifting array
                for (int i = remove_num - 1; i < *order_count - 1; i++) {
                    order_items[i] = order_items[i + 1];
                }
                (*order_count)--;
                printf("Item removed from order.\n");
                break;
            }
            case 3:
                if (*order_count == 0) {
                    printf("No items in order. Please add items first.\n");
                    break;
                }
                return 1; // Proceed to checkout
            case 0:
                return 0; // Cancel order
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

// Function to calculate total cost
float calculate_total_cost(OrderItem *order_items, int order_count) {
    float total = 0.0;
    for (int i = 0; i < order_count; i++) {
        total += order_items[i].subtotal;
    }
    return total;
}

// Function to process payment
int process_payment(float total_cost) {
    Student students[1];
    int count;
    
    if (find_student(current_student_id, students, &count) == FILE_ERROR || count == 0) {
        printf("Error: Could not find student account.\n");
        return 0;
    }
    
    float current_balance = students[0].account_balance;
    
    printf("\n----- PAYMENT PROCESSING -----\n");
    printf("Order Total: RM%.2f\n", total_cost);
    printf("Current Balance: RM%.2f\n", current_balance);
    
    if (current_balance < total_cost) {
        printf("Insufficient balance! You need RM%.2f more.\n", total_cost - current_balance);
        printf("Please top up your account and try again.\n");
        return 0;
    }
    
    char prompt[100];
    snprintf(prompt, sizeof(prompt), "Confirm payment of RM%.2f? (1. Yes / 2. No): ", total_cost);
    int confirm = confirmation(prompt);
    
    if (confirm == YES) {
        // Deduct amount from balance
        update_account_balance(current_student_id, -total_cost);
        printf("Payment successful! RM%.2f deducted from your account.\n", total_cost);
        printf("Remaining balance: RM%.2f\n", current_balance - total_cost);
        return 1;
    } else {
        printf("Payment cancelled.\n");
        return 0;
    }
}

// Function to generate order confirmation
void generate_order_confirmation(OrderItem *order_items, int order_count, float total_cost) {
    printf("\n==================== ORDER CONFIRMATION ====================\n");
    printf("Order placed successfully!\n");
    printf("Student ID: %s\n", current_student_id);
    
    // Get current date/time (simplified)
    printf("Order Date: Today\n");
    printf("Order Time: Now\n");
    
    printf("\nOrder Details:\n");
    printf("%-20s %-15s %-8s %-10s %-10s\n", "Item", "Restaurant", "Qty", "Unit Price", "Subtotal");
    printf("-----------------------------------------------------------------------\n");
    
    for (int i = 0; i < order_count; i++) {
        printf("%-20s %-15s %-8d RM%-8.2f RM%.2f\n",
               order_items[i].item_name, order_items[i].restaurant_name,
               order_items[i].quantity, order_items[i].item_price, order_items[i].subtotal);
    }
    
    printf("-----------------------------------------------------------------------\n");
    printf("Total Amount: RM%.2f\n", total_cost);
    printf("Payment Status: PAID\n");
    printf("Order Status: CONFIRMED\n");
    printf("\nThank you for your order! Your food will be prepared and delivered soon.\n");
    printf("=============================================================\n");
}

// --------------------------------------------------------------------------------------------

// Forward declarations for order history tracking

int find_student_orders(char *student_id, OrderHistory *orders, int *count);
void display_order_history(OrderHistory *orders, int count);
void view_all_orders(void);
void view_current_orders(void);
void view_student_order_details(void);
void track_specific_order(void);
int get_order_history_by_number(OrderHistory *orders, int count, int order_number, OrderHistory *selected_order);
void get_delivery_info(char *order_id, char *delivery_id, char *delivery_status, char *eta, char *delivered_time);
const char* get_order_status_display(const char *status);
void display_order_receipt(OrderHistory *order);

#define VIEW_ALL_ORDERS 1
#define VIEW_CURRENT_ORDERS 2
#define VIEW_ORDER_DETAILS 3
#define TRACK_SPECIFIC_ORDER 4

int track_order_history(void) {
    int choice;
    while (1) {
        printf("\n----- TRACK ORDER HISTORY -----\n");
        printf("1. View all orders\n");
        printf("2. View current orders\n");
        printf("3. View order details\n");
        printf("4. Track specific order\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_ALL_ORDERS:
                view_all_orders();
                break;
            case VIEW_CURRENT_ORDERS:
                view_current_orders();
                break;
            case VIEW_ORDER_DETAILS:
                view_student_order_details();
                break;
            case TRACK_SPECIFIC_ORDER:
                track_specific_order();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get order status display text
const char* get_order_status_display(const char *status) {
    if (strcmp(status, "CONFIRMED") == 0) return "Confirmed";
    if (strcmp(status, "PREPARING") == 0) return "Preparing";
    if (strcmp(status, "READY") == 0) return "Ready";
    if (strcmp(status, "DELIVERED") == 0) return "Delivered";
    if (strcmp(status, "CANCELLED") == 0) return "Cancelled";
    return status;
}

// Function to get delivery information for an order
void get_delivery_info(char *order_id, char *delivery_id, char *delivery_status, char *eta, char *delivered_time) {
    strcpy(delivery_id, "N/A");
    strcpy(delivery_status, "Not Assigned");
    strcpy(eta, "N/A");
    strcpy(delivered_time, "N/A");
    
    FILE *fp = fopen(DELIVERY_FILE, "r");
    if (fp == NULL) {
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char did[DELIVERY_ID_LENGTH], oid[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], estimated_time[TIME_LENGTH], delivered[TIME_LENGTH];
        int del_status, punctuality;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               did, oid, delivery_date, estimated_time, delivered,
               &del_status, &punctuality);
        
        if (strcmp(oid, order_id) == 0) {
            strcpy(delivery_id, did);
            strcpy(eta, estimated_time);
            strcpy(delivered_time, strlen(delivered) > 0 ? delivered : "Pending");
            
            switch (del_status) {
                case 1: strcpy(delivery_status, "Delivered"); break;
                case 2: strcpy(delivery_status, "Delivering"); break;
                case 3: strcpy(delivery_status, "Undelivered"); break;
                default: strcpy(delivery_status, "Unknown"); break;
            }
            break;
        }
    }
    
    fclose(fp);
}

// Function to find orders for a specific student
int find_student_orders(char *student_id, OrderHistory *orders, int *count) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", ORDER_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';

        char order_id[ORDER_ID_LENGTH], sid[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];

        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, sid, restaurant_id, &total_price,
               order_date, order_time, status);

        if (strcmp(student_id, sid) == 0) {
            strcpy(orders[*count].order_id, order_id);
            strcpy(orders[*count].restaurant_id, restaurant_id);
            orders[*count].total_price = total_price;
            strcpy(orders[*count].order_date, order_date);
            strcpy(orders[*count].order_time, order_time);
            strcpy(orders[*count].status, status);
            
            // Get restaurant name
            get_restaurant_info(restaurant_id, orders[*count].restaurant_name, NULL);
            
            // Get items summary
            get_order_items_summary(order_id, orders[*count].items_summary, sizeof(orders[*count].items_summary));
            
            // Get delivery information
            get_delivery_info(order_id, orders[*count].delivery_id, orders[*count].delivery_status,
                            orders[*count].estimated_arrival, orders[*count].delivered_time);
            
            (*count)++;
        }
    }

    fclose(fp);
    return *count;
}

// Function to display order history
void display_order_history(OrderHistory *orders, int count) {
    if (count == 0) {
        printf("No orders found.\n");
        return;
    }

    printf("\n============================= ORDER HISTORY ============================\n");
    printf("%-4s %-8s %-15s %-8s %-12s %-10s %-12s\n", 
           "No.", "Order ID", "Restaurant", "Total", "Date", "Time", "Status");
    printf("------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-4d %-8s %-15s RM%-6.2f %-12s %-10s %-12s\n", 
               i + 1, orders[i].order_id, orders[i].restaurant_name,
               orders[i].total_price, orders[i].order_date, orders[i].order_time,
               get_order_status_display(orders[i].status));
    }
    printf("------------------------------------------------------------------------\n");
}

// Function to view all orders
void view_all_orders(void) {
    OrderHistory orders[100];
    int count;
    
    printf("\n----- VIEW ALL ORDERS -----\n");

    if (find_student_orders(current_student_id, orders, &count) == FILE_ERROR) return;

    display_order_history(orders, count);
    printf("Total orders: %d\n", count);
    
    // Calculate total spending
    float total_spent = 0.0;
    int delivered_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].status, "DELIVERED") == 0) {
            total_spent += orders[i].total_price;
            delivered_count++;
        }
    }
    
    printf("\n--- ORDER SUMMARY ---\n");
    printf("Total Orders Placed: %d\n", count);
    printf("Successfully Delivered: %d\n", delivered_count);
    printf("Total Amount Spent: RM%.2f\n", total_spent);
    
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

// Function to view current orders (non-delivered)
void view_current_orders(void) {
    OrderHistory orders[100];
    int count;
    
    printf("\n----- VIEW CURRENT ORDERS -----\n");

    if (find_student_orders(current_student_id, orders, &count) == FILE_ERROR) return;

    // Filter current orders (not delivered or cancelled)
    OrderHistory current_orders[100];
    int current_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].status, "DELIVERED") != 0 && strcmp(orders[i].status, "CANCELLED") != 0) {
            current_orders[current_count] = orders[i];
            current_count++;
        }
    }

    if (current_count == 0) {
        printf("No current orders found.\n");
        printf("All your orders have been delivered or cancelled.\n");
    } else {
        display_order_history(current_orders, current_count);
        printf("Current active orders: %d\n", current_count);
        
        // Show detailed status for current orders
        printf("\n--- CURRENT ORDER STATUS ---\n");
        for (int i = 0; i < current_count; i++) {
            printf("Order %s: %s", current_orders[i].order_id, get_order_status_display(current_orders[i].status));
            
            if (strcmp(current_orders[i].delivery_status, "Not Assigned") != 0) {
                printf(" | Delivery: %s", current_orders[i].delivery_status);
                if (strcmp(current_orders[i].estimated_arrival, "N/A") != 0) {
                    printf(" | ETA: %s", current_orders[i].estimated_arrival);
                }
            }
            printf("\n");
        }
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

// Function to get order by number
int get_order_history_by_number(OrderHistory *orders, int count, int order_number, OrderHistory *selected_order) {
    if (order_number < 1 || order_number > count) {
        return 0;
    }
    
    *selected_order = orders[order_number - 1];
    return 1;
}

// Function to display detailed order receipt
void display_order_receipt(OrderHistory *order) {
    printf("\n==================== ORDER RECEIPT ====================\n");
    printf("Order ID: %s\n", order->order_id);
    printf("Restaurant: %s\n", order->restaurant_name);
    printf("Order Date: %s\n", order->order_date);
    printf("Order Time: %s\n", order->order_time);
    printf("Order Status: %s\n", get_order_status_display(order->status));
    
    printf("\nItems Ordered:\n");
    printf("- %s\n", order->items_summary);
    
    printf("\nPayment Information:\n");
    printf("Total Amount: RM%.2f\n", order->total_price);
    printf("Payment Method: Account Balance\n");
    printf("Payment Status: %s\n", strcmp(order->status, "CANCELLED") == 0 ? "Refunded" : "Paid");
    
    if (strcmp(order->delivery_id, "N/A") != 0) {
        printf("\nDelivery Information:\n");
        printf("Delivery ID: %s\n", order->delivery_id);
        printf("Delivery Status: %s\n", order->delivery_status);
        printf("Estimated Arrival: %s\n", order->estimated_arrival);
        printf("Delivered Time: %s\n", order->delivered_time);
    } else {
        printf("\nDelivery Information:\n");
        printf("Delivery Status: Not yet assigned\n");
    }
    
    printf("=======================================================\n");
}

// Function to view order details
void view_student_order_details(void) {
    OrderHistory orders[100];
    int count;
    
    printf("\n----- VIEW ORDER DETAILS -----\n");

    if (find_student_orders(current_student_id, orders, &count) == FILE_ERROR) return;

    if (count == 0) {
        printf("No orders found.\n");
        return;
    }

    display_order_history(orders, count);
    
    int detail_choice;
    while (1) {
        detail_choice = get_integer_input("Select order number to view details (enter '0' to exit): ");
        if (detail_choice >= 0 && detail_choice <= count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", count);
    }
    
    if (detail_choice == 0) {
        return;
    }
    
    OrderHistory selected_order;
    get_order_history_by_number(orders, count, detail_choice, &selected_order);
    
    display_order_receipt(&selected_order);
    
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

// Function to track specific order
void track_specific_order(void) {
    printf("\n----- TRACK SPECIFIC ORDER -----\n");
    
    char order_id[ORDER_ID_LENGTH];
    printf("Enter Order ID to track: ");
    fgets(order_id, sizeof(order_id), stdin);
    order_id[strcspn(order_id, "\n")] = '\0';
    
    // Find the specific order
    OrderHistory orders[100];
    int count;
    
    if (find_student_orders(current_student_id, orders, &count) == FILE_ERROR) return;
    
    int found = 0;
    OrderHistory tracked_order;
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].order_id, order_id) == 0) {
            tracked_order = orders[i];
            found = 1;
            break;
        }
    }
    
    if (!found) {
        printf("Order ID '%s' not found in your order history.\n", order_id);
        printf("Please check the Order ID and try again.\n");
        return;
    }
    
    printf("\n--- ORDER TRACKING ---\n");
    printf("Order ID: %s\n", tracked_order.order_id);
    printf("Restaurant: %s\n", tracked_order.restaurant_name);
    printf("Items: %s\n", tracked_order.items_summary);
    printf("Total: RM%.2f\n", tracked_order.total_price);
    printf("Order Date: %s at %s\n", tracked_order.order_date, tracked_order.order_time);
    
    printf("\n--- ORDER STATUS TIMELINE ---\n");
    printf("Order Placed: %s %s\n", tracked_order.order_date, tracked_order.order_time);
    
    if (strcmp(tracked_order.status, "CONFIRMED") == 0) {
        printf("Order Confirmed: Waiting for restaurant to prepare\n");
        printf("Preparing: Pending\n");
        printf("Ready for Delivery: Pending\n");
        printf("Out for Delivery: Pending\n");
        printf("Delivered: Pending\n");
    } else if (strcmp(tracked_order.status, "PREPARING") == 0) {
        printf("Order Confirmed: Complete\n");
        printf("Preparing: Restaurant is preparing your order\n");
        printf("Ready for Delivery: Pending\n");
        printf("Out for Delivery: Pending\n");
        printf("Delivered: Pending\n");
    } else if (strcmp(tracked_order.status, "READY") == 0) {
        printf("Order Confirmed: Complete\n");
        printf("Preparing: Complete\n");
        printf("Ready for Delivery: Order is ready for pickup\n");
        if (strcmp(tracked_order.delivery_status, "Not Assigned") == 0) {
            printf("Out for Delivery: Waiting for delivery personnel\n");
        } else {
            printf("Out for Delivery: Delivery personnel assigned\n");
        }
        printf("Delivered: Pending\n");
    } else if (strcmp(tracked_order.status, "DELIVERED") == 0) {
        printf("Order Confirmed: Complete\n");
        printf("Preparing: Complete\n");
        printf("Ready for Delivery: Complete\n");
        printf("Out for Delivery: Complete\n");
        printf("Delivered: %s\n", tracked_order.delivered_time);
    } else if (strcmp(tracked_order.status, "CANCELLED") == 0) {
        printf("Order Cancelled\n");
        printf("Refund processed to your account\n");
    }
    
    if (strcmp(tracked_order.delivery_id, "N/A") != 0) {
        printf("\n--- DELIVERY INFORMATION ---\n");
        printf("Delivery ID: %s\n", tracked_order.delivery_id);
        printf("Delivery Status: %s\n", tracked_order.delivery_status);
        if (strcmp(tracked_order.estimated_arrival, "N/A") != 0) {
            printf("Estimated Arrival: %s\n", tracked_order.estimated_arrival);
        }
        if (strcmp(tracked_order.delivered_time, "N/A") != 0 && strcmp(tracked_order.delivered_time, "Pending") != 0) {
            printf("Delivered At: %s\n", tracked_order.delivered_time);
        }
    }
    
    // Properly clear input buffer and wait for exit
    clear_input_buffer_safe(); // Clear any remaining characters
    
    int exit_choice;
    while (1) {
        printf("\nEnter 0 to exit: ");
        fflush(stdout);
        
        char input_line[100];
        if (fgets(input_line, sizeof(input_line), stdin) != NULL) {
            // Remove newline character
            input_line[strcspn(input_line, "\n")] = '\0';
            
            // Try to parse as integer
            char *endptr;
            long val = strtol(input_line, &endptr, 10);
            
            // Check if entire string was converted and value is 0
            if (*endptr == '\0' && val == 0) {
                break; // Valid input: 0
            } else {
                printf("Invalid input! Please enter 0 to exit.\n");
            }
        } else {
            printf("Input error. Please try again.\n");
        }
    }
}

// --------------------------------------------------------------------------------------------

#define TOP_UP_TEN 1
#define TOP_UP_TWENTY 2
#define TOP_UP_FIFTY 3
#define TOP_UP_ONE_HUNDRED 4
#define TOP_UP_TWO_HUNDRED 5

int account_balance_management(void) {
    // Check if current_student_id is set
    if (strlen(current_student_id) == 0) {
        printf("Error: Student ID not set. Please login again.\n");
        return 0;
    }
    
    // Check account activation status first
    int activation_status = check_account_activation_status(current_student_id);
    
    while (1) {
        // First show current balance and account status
        Student students[10]; // 增加数组大小以防止越界
        int count;
        int find_result = find_student(current_student_id, students, &count);
        
        if (find_result == FILE_ERROR) {
            printf("Error: Could not access student data.\n");
            return 0;
        }
        
        if (count == 0) {
            printf("Error: Student not found with ID: %s\n", current_student_id);
            return 0;
        }
        
        printf("\nCurrent Account Balance: RM%.2f\n", students[0].account_balance);
        printf("Account Status: %s\n", get_activation_status_text(students[0].account_activate_status));
        
        // Check if account is deactivated
        if (activation_status == DEACTIVATE) {
            printf("\n****** ACCOUNT DEACTIVATED ******\n");
            printf("Your account is currently deactivated.\n");
            printf("Please approach the administrator to activate your account before you can top up.\n");
            printf("You cannot perform any top-up operations until your account is activated.\n");
            
            int exit_choice;
            while (1) {
                exit_choice = get_integer_input("\nEnter 0 to exit: ");
                if (exit_choice == 0) {
                    return 0;
                } else {
                    printf("Invalid input! Please enter 0 to exit.\n");
                }
            }
        }
        
        printf("\n----- TOP UP ACCOUNT BALANCE -----\n");
        printf("1. RM10\n");
        printf("2. RM20\n");
        printf("3. RM50\n");
        printf("4. RM100\n");
        printf("5. RM200\n");
        printf("0. Cancel top up\n");
        
        int choice;
        while (1) {
            choice = get_integer_input("Enter your choice (0 - 5): ");
            if (choice >= 0 && choice <= 5) {
                break;
            } else {
                printf("Invalid choice. Please try again.\n");
                printf("\n----- TOP UP ACCOUNT BALANCE -----\n");
                printf("1. RM10\n");
                printf("2. RM20\n");
                printf("3. RM50\n");
                printf("4. RM100\n");
                printf("5. RM200\n");
                printf("0. Cancel top up\n");
            }
        }

        switch (choice) {
            case TOP_UP_TEN:
                top_up_account_balance(10);
                break;
            case TOP_UP_TWENTY:
                top_up_account_balance(20);
                break;
            case TOP_UP_FIFTY:
                top_up_account_balance(50);
                break;
            case TOP_UP_ONE_HUNDRED:
                top_up_account_balance(100);
                break;
            case TOP_UP_TWO_HUNDRED:
                top_up_account_balance(200);
                break;
            case 0:
                printf("Account balance top up has been cancelled!\n");
                return 0;
        }
        
        // Refresh activation status after each operation
        activation_status = check_account_activation_status(current_student_id);
    }
    
    return 0;
}

void top_up_account_balance(int top_up_amount) {
    char prompt[100];
    snprintf(prompt, sizeof(prompt), "Are you sure you want to top up RM%d? (1. Yes / 2. No): ", top_up_amount);
    int confirm = confirmation(prompt);
    if (confirm == YES) {
        update_account_balance(current_student_id, (float)top_up_amount);
        printf("Successfully topped up RM%d to your account!\n", top_up_amount);
    } else {
        printf("Top up cancelled.\n");
    }
}

void update_account_balance(char *student_id, float amount) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", STUDENT_FILE);
        return;
    }

    FILE *temp_fp = fopen("temp_student.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        char sid[STUDENT_ID_LENGTH], name[NAME_LENGTH];
        char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
        int status;
        float balance;

        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f", 
               sid, name, contact, email, password, &status, &balance);

        if (strcmp(sid, student_id) == 0) {
            balance += amount;
        }

        fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%.2f\n", 
                sid, name, contact, email, password, status, balance);
    }

    fclose(fp);
    fclose(temp_fp);

    remove(STUDENT_FILE);
    rename("temp_student.txt", STUDENT_FILE);
}