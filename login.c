#include "login.h"

#define MAX_LOGIN_ATTEMPT 3
char current_restaurant_id[RESTAURANT_ID_LENGTH];
char current_student_id[STUDENT_ID_LENGTH];
char current_delivery_personnel_id[DELIVERY_PERSONNEL_ID_LENGTH];

MenuOption AdminMenu[] = {
    {1, "User Management", user_management},
    {2, "System-wide reporting and analytics", reporting_and_analytics},
    {3, "Data backup and restore functionality", data_backup_and_restore_functionality},
    {4, "System configuration management", system_configuration_management},
    {5, "Logout", logout}
};

MenuOption RestaurantMenu[] = {
    {1, "Menu item management", menu_item_management},
    {2, "Order queue management", order_queue_management},
    {3, "Daily sales reporting", daily_sales_report},
    {4, "Inventory tracking", inventory_tracking},
    {5, "Logout", logout}
};

MenuOption StudentMenu[] = {
    {1, "Profile management", profile_management},
    {2, "Place order", order_placement},
    {3, "Track order history", track_order_history},
    {4, "Manage account balance", account_balance_management},
    {5, "Logout", logout}
};

MenuOption DeliveryPersonnelMenu[] = {
    {1, "Order assignment", order_assignment},
    {2, "Manage delivery status", delivery_status_management},
    {3, "Earnings calculation and tracking", earnings_calculation_and_tracking},
    {4, "Performance statistics", performance_statistics},
    {5, "Logout", logout}
};

int login(User *u);
static int run_menu(const char *name, const char *role, const struct MenuOption menu[], int menu_size);

int show_main_menu(User *u) {
    switch (u->role) {
        case ADMIN:
            return run_menu(u->name, "admin", AdminMenu, sizeof(AdminMenu) / sizeof(AdminMenu[0]));
        case STUDENT:
            return run_menu(u->name, "student", StudentMenu, sizeof(StudentMenu) / sizeof(StudentMenu[0]));
        case RESTAURANT:
            return run_menu(u->name, "restaurant", RestaurantMenu, sizeof(RestaurantMenu) / sizeof(RestaurantMenu[0]));
        case DELIVERY_PERSONNEL:
            return run_menu(u->name, "delivery Personnel", DeliveryPersonnelMenu, sizeof(DeliveryPersonnelMenu) / sizeof(DeliveryPersonnelMenu[0]));
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------

static int run_menu(const char *name, const char *role, const struct MenuOption menu[], int menu_size) {
    printf("\nWelcome back, %s!\n", name);
    char buffer[5];
    char extra;
    int task_choice;

    while (1) {
        printf("\n----- MAIN MENU -----\n");
        for (int i = 0; i < menu_size; i++) {
            printf("%d. %s\n", menu[i].task_number, menu[i].description);
        }
        printf("What would you like to do?: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%d %c", &task_choice, &extra) != 1) {
            printf("\nInvalid input! Please enter a valid choice.\n");
            continue;
        }

        bool found = false;
        for (int i = 0; i < menu_size; i++) {
            if (menu[i].task_number == task_choice) {
                found = true;
                int result = menu[i].function();
                if (result == -1) {
                    return -1; // logout - return to main
                }
                break;
            }
        }
        if (!found) {
            printf("\nInvalid input! Please enter a valid choice.\n");
        }
    }
}

#include "database.h"

// LOGIN FUNCTION
int login(User *u) {
    char input_email_address[EMAIL_ADDRESS_LENGTH];
    char input_password[PASSWORD_LENGTH];

    printf("\n----- USER LOGIN -----\n");
    for (int attempt = 0; attempt < MAX_LOGIN_ATTEMPT; attempt++) {
        printf("Enter email: ");
        fgets(input_email_address, sizeof(input_email_address), stdin);
        input_email_address[strcspn(input_email_address, "\n")] = '\0';

        printf("Enter password: ");
        fgets(input_password, sizeof(input_password), stdin);
        input_password[strcspn(input_password, "\n")] = '\0';

        if (find_user_by_credentials(input_email_address, input_password, u)) {
            if (u->role == RESTAURANT) {
                FILE *fp = fopen(RESTAURANT_FILE, "r");
                char line[MAX_LINE_LENGTH];
                while (fgets(line, sizeof(line), fp)) {
                    char rid[RESTAURANT_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
                    int cuisine;

                    sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d", rid, name, contact, email, password, &cuisine);

                    if (strcmp(email, input_email_address) == 0) {
                        strcpy(current_restaurant_id, rid);  // Assign current restaurant ID
                        break;
                    }
                }
                fclose(fp);
            }
            else if (u->role == STUDENT) {
                FILE *fp = fopen(STUDENT_FILE, "r");
                char line[MAX_LINE_LENGTH];
                while (fgets(line, sizeof(line), fp)) {
                    char sid[STUDENT_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
                    int status;
                    float balance;

                    sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f", sid, name, contact, email, password, &status, &balance);

                    if (strcmp(email, input_email_address) == 0) {
                        strcpy(current_student_id, sid);  // Assign current student ID
                        break;
                    }
                }
                fclose(fp);                
            }
            else if (u->role == DELIVERY_PERSONNEL) {
                FILE *fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
                char line[MAX_LINE_LENGTH];
                while (fgets(line, sizeof(line), fp)) {
                    char pid[DELIVERY_PERSONNEL_ID_LENGTH], name[NAME_LENGTH];
                    char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
                    int status;
                    char delivery_id[DELIVERY_ID_LENGTH];

                    sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s", pid, name, contact, email, password, &status, delivery_id);

                    if (strcmp(email, input_email_address) == 0) {
                        strcpy(current_delivery_personnel_id, pid);  // Assign current delivery personnel ID
                        break;
                    }
                }
                fclose(fp);                
            }
            return true;
        }
    }
    printf("Too many failed login attempts. Please try again later.");
    return false;
}
