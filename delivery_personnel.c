#include "delivery_personnel.h"

extern char current_delivery_personnel_id[DELIVERY_PERSONNEL_ID_LENGTH];

// Forward declarations for order assignment
int find_available_orders(QueueOrder *orders, int *count);
int find_assigned_orders(char *delivery_personnel_id, QueueOrder *orders, int *count);
void display_available_orders(QueueOrder *orders, int count);
void display_assigned_orders(QueueOrder *orders, int count);
void view_available_orders(void);
void view_assigned_orders(void);
int accept_order_assignment(void);
int get_delivery_personnel_status(char *delivery_personnel_id);
int update_delivery_personnel_status(char *delivery_personnel_id, int new_status, char *delivery_id);
void get_delivery_id(char *id, int id_size);
int create_delivery_record(char *order_id, char *delivery_id);
int get_order_by_number_delivery(QueueOrder *orders, int count, int order_number, QueueOrder *selected_order);
void get_restaurant_info(char *restaurant_id, char *restaurant_name, char *restaurant_contact);
void get_student_info(char *student_id, char *student_name, char *student_contact);
void view_delivery_details(void);
extern void get_order_items_summary(char *order_id, char *summary, int summary_size);

#define VIEW_AVAILABLE_ORDERS 1
#define VIEW_ASSIGNED_ORDERS 2
#define ACCEPT_ORDER_ASSIGNMENT 3
#define VIEW_DELIVERY_DETAILS 4

int order_assignment(void) {
    int choice;
    while (1) {
        printf("\n----- ORDER ASSIGNMENT -----\n");
        printf("1. View available orders\n");
        printf("2. View assigned orders\n");
        printf("3. Accept order assignment\n");
        printf("4. View delivery details\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_AVAILABLE_ORDERS:
                view_available_orders();
                break;
            case VIEW_ASSIGNED_ORDERS:
                view_assigned_orders();
                break;
            case ACCEPT_ORDER_ASSIGNMENT:
                accept_order_assignment();
                break;
            case VIEW_DELIVERY_DETAILS:
                view_delivery_details();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to find available orders (READY status)
int find_available_orders(QueueOrder *orders, int *count) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", ORDER_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';

        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];

        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, restaurant_id, &total_price,
               order_date, order_time, status);

        // Only show READY orders that are not yet assigned to delivery
        if (strcmp(status, "READY") == 0) {
            // Check if order is already assigned to a delivery personnel
            FILE *delivery_fp = fopen(DELIVERY_FILE, "r");
            int already_assigned = 0;
            
            if (delivery_fp != NULL) {
                char delivery_line[MAX_LINE_LENGTH];
                while (fgets(delivery_line, sizeof(delivery_line), delivery_fp)) {
                    char delivery_id[DELIVERY_ID_LENGTH], check_order_id[ORDER_ID_LENGTH];
                    sscanf(delivery_line, "%[^,],%[^,]", delivery_id, check_order_id);
                    
                    if (strcmp(check_order_id, order_id) == 0) {
                        already_assigned = 1;
                        break;
                    }
                }
                fclose(delivery_fp);
            }
            
            if (!already_assigned) {
                strcpy(orders[*count].order_id, order_id);
                strcpy(orders[*count].student_id, student_id);
                strcpy(orders[*count].restaurant_id, restaurant_id);
                orders[*count].total_price = total_price;
                strcpy(orders[*count].order_date, order_date);
                strcpy(orders[*count].order_time, order_time);
                strcpy(orders[*count].status, status);
                
                // Get items summary
                get_order_items_summary(order_id, orders[*count].items_summary, sizeof(orders[*count].items_summary));
                
                (*count)++;
            }
        }
    }

    fclose(fp);
    return *count;
}

// Function to find assigned orders for current delivery personnel
int find_assigned_orders(char *delivery_personnel_id, QueueOrder *orders, int *count) {
    FILE *delivery_fp = fopen(DELIVERY_FILE, "r");
    if (delivery_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", DELIVERY_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    
    // First, get delivery records for this personnel
    while (fgets(line, sizeof(line), delivery_fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';
        
        char delivery_id[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               delivery_id, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);
        
        // Check if this delivery belongs to current personnel
        FILE *personnel_fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
        if (personnel_fp != NULL) {
            char personnel_line[MAX_LINE_LENGTH];
            while (fgets(personnel_line, sizeof(personnel_line), personnel_fp)) {
                char pid[DELIVERY_PERSONNEL_ID_LENGTH], name[NAME_LENGTH];
                char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
                int status;
                char assigned_delivery_id[DELIVERY_ID_LENGTH];
                
                sscanf(personnel_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s",
                       pid, name, contact, email, password, &status, assigned_delivery_id);
                
                if (strcmp(pid, delivery_personnel_id) == 0 && 
                    strcmp(assigned_delivery_id, delivery_id) == 0) {
                    
                    // Get order details
                    FILE *order_fp = fopen(ORDER_FILE, "r");
                    if (order_fp != NULL) {
                        char order_line[MAX_LINE_LENGTH];
                        while (fgets(order_line, sizeof(order_line), order_fp)) {
                            char oid[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
                            char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
                            float total_price;
                            char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
                            
                            sscanf(order_line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                                   oid, student_id, restaurant_id, &total_price,
                                   order_date, order_time, order_status);
                            
                            if (strcmp(oid, order_id) == 0) {
                                strcpy(orders[*count].order_id, order_id);
                                strcpy(orders[*count].student_id, student_id);
                                strcpy(orders[*count].restaurant_id, restaurant_id);
                                orders[*count].total_price = total_price;
                                strcpy(orders[*count].order_date, order_date);
                                strcpy(orders[*count].order_time, order_time);
                                
                                // Set delivery status as order status
                                if (delivery_status == DELIVERED) {
                                    strcpy(orders[*count].status, "DELIVERED");
                                } else {
                                    strcpy(orders[*count].status, "DELIVERING");
                                }
                                
                                get_order_items_summary(order_id, orders[*count].items_summary, sizeof(orders[*count].items_summary));
                                (*count)++;
                                break;
                            }
                        }
                        fclose(order_fp);
                    }
                    break;
                }
            }
            fclose(personnel_fp);
        }
    }
    
    fclose(delivery_fp);
    return *count;
}

// Function to display available orders
void display_available_orders(QueueOrder *orders, int count) {
    if (count == 0) {
        printf("No available orders for delivery.\n");
        return;
    }

    printf("\n========================= AVAILABLE ORDERS =========================\n");
    printf("%-4s %-8s %-12s %-10s %-8s %-12s %-25s\n", 
           "No.", "Order ID", "Restaurant", "Student", "Total", "Time", "Items");
    printf("--------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        char restaurant_name[NAME_LENGTH];
        get_restaurant_info(orders[i].restaurant_id, restaurant_name, NULL);
        
        printf("%-4d %-8s %-12s %-10s RM%-6.2f %-12s %-25s\n", 
               i + 1, orders[i].order_id, restaurant_name, orders[i].student_id,
               orders[i].total_price, orders[i].order_time, orders[i].items_summary);
    }
    printf("--------------------------------------------------------------------------------------\n");
}

// Function to display assigned orders
void display_assigned_orders(QueueOrder *orders, int count) {
    if (count == 0) {
        printf("No assigned orders.\n");
        return;
    }

    printf("\n========================= ASSIGNED ORDERS =========================\n");
    printf("%-4s %-8s %-12s %-10s %-8s %-12s %-12s\n", 
           "No.", "Order ID", "Restaurant", "Student", "Total", "Time", "Status");
    printf("------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        char restaurant_name[NAME_LENGTH];
        get_restaurant_info(orders[i].restaurant_id, restaurant_name, NULL);
        
        printf("%-4d %-8s %-12s %-10s RM%-6.2f %-12s %-12s\n", 
               i + 1, orders[i].order_id, restaurant_name, orders[i].student_id,
               orders[i].total_price, orders[i].order_time, orders[i].status);
    }
    printf("------------------------------------------------------------------------------\n");
}

// Function to view available orders
void view_available_orders(void) {
    QueueOrder orders[100];
    int count;
    
    printf("\n----- VIEW AVAILABLE ORDERS -----\n");

    if (find_available_orders(orders, &count) == FILE_ERROR) return;

    display_available_orders(orders, count);
    printf("Total available orders: %d\n", count);
    
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

// Function to view assigned orders
void view_assigned_orders(void) {
    QueueOrder orders[100];
    int count;
    
    printf("\n----- VIEW ASSIGNED ORDERS -----\n");

    if (find_assigned_orders(current_delivery_personnel_id, orders, &count) == FILE_ERROR) return;

    display_assigned_orders(orders, count);
    printf("Total assigned orders: %d\n", count);
    
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

// Function to get delivery personnel status
int get_delivery_personnel_status(char *delivery_personnel_id) {
    FILE *fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
    if (fp == NULL) {
        return NO_ORDER_ASSIGNED;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        char pid[DELIVERY_PERSONNEL_ID_LENGTH];
        int status;
        
        sscanf(line, "%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%d", pid, &status);
        
        if (strcmp(pid, delivery_personnel_id) == 0) {
            fclose(fp);
            return status;
        }
    }
    
    fclose(fp);
    return NO_ORDER_ASSIGNED;
}

// Function to generate delivery ID
void get_delivery_id(char *id, int id_size) {
    FILE *fp = fopen(DELIVERY_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "D00001");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int max_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy));
        char *token = strtok(line_copy, ",");
        if (token && token[0] == 'D') {
            int num = atoi(&token[1]);
            if (num > max_num) max_num = num;
        }
    }
    fclose(fp);
    snprintf(id, id_size, "D%05d", max_num + 1);
}

// Function to create delivery record
int create_delivery_record(char *order_id, char *delivery_id) {
    FILE *fp = fopen(DELIVERY_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", DELIVERY_FILE);
        return FILE_ERROR;
    }
    
    // Create delivery record with current date and estimated time
    fprintf(fp, "%s,%s,28-12-2024,17:00:00,,2,2\n", delivery_id, order_id);
    fclose(fp);
    return 1;
}

// Function to update delivery personnel status
int update_delivery_personnel_status(char *delivery_personnel_id, int new_status, char *delivery_id) {
    FILE *fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", DELIVERY_PERSONNEL_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_delivery_personnel.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        char pid[DELIVERY_PERSONNEL_ID_LENGTH], name[NAME_LENGTH];
        char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
        int status;
        char current_delivery_id[DELIVERY_ID_LENGTH];

        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s",
               pid, name, contact, email, password, &status, current_delivery_id);

        if (strcmp(pid, delivery_personnel_id) == 0) {
            fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%s\n",
                    pid, name, contact, email, password, new_status, delivery_id);
            updated = 1;
        } else {
            fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%s\n",
                    pid, name, contact, email, password, status, current_delivery_id);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (updated) {
        remove(DELIVERY_PERSONNEL_FILE);
        rename("temp_delivery_personnel.txt", DELIVERY_PERSONNEL_FILE);
        return 1;
    } else {
        remove("temp_delivery_personnel.txt");
        return 0;
    }
}

// Function to get order by number
int get_order_by_number_delivery(QueueOrder *orders, int count, int order_number, QueueOrder *selected_order) {
    if (order_number < 1 || order_number > count) {
        return 0;
    }
    
    *selected_order = orders[order_number - 1];
    return 1;
}

// Function to get restaurant information
void get_restaurant_info(char *restaurant_id, char *restaurant_name, char *restaurant_contact) {
    FILE *fp = fopen(RESTAURANT_FILE, "r");
    if (fp == NULL) {
        if (restaurant_name) strcpy(restaurant_name, "Unknown");
        if (restaurant_contact) strcpy(restaurant_contact, "Unknown");
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        char rid[RESTAURANT_ID_LENGTH], name[NAME_LENGTH];
        char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
        int cuisine;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d",
               rid, name, contact, email, password, &cuisine);
        
        if (strcmp(rid, restaurant_id) == 0) {
            if (restaurant_name) strcpy(restaurant_name, name);
            if (restaurant_contact) strcpy(restaurant_contact, contact);
            break;
        }
    }
    
    fclose(fp);
}

// Function to get student information
void get_student_info(char *student_id, char *student_name, char *student_contact) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        if (student_name) strcpy(student_name, "Unknown");
        if (student_contact) strcpy(student_contact, "Unknown");
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
            if (student_name) strcpy(student_name, name);
            if (student_contact) strcpy(student_contact, contact);
            break;
        }
    }
    
    fclose(fp);
}

// Function to accept order assignment
int accept_order_assignment(void) {
    // Check if delivery personnel is already assigned to an order
    int current_status = get_delivery_personnel_status(current_delivery_personnel_id);
    if (current_status == DELIVERING_ORDER) {
        printf("You are already assigned to a delivery. Complete current delivery first.\n");
        return 0;
    }
    
    QueueOrder orders[100];
    int count;
    
    printf("\n----- ACCEPT ORDER ASSIGNMENT -----\n");
    
    if (find_available_orders(orders, &count) == FILE_ERROR) {
        return 0;
    }
    
    if (count == 0) {
        printf("No available orders for assignment.\n");
        return 0;
    }
    
    display_available_orders(orders, count);
    
    int assignment_choice;
    while (1) {
        assignment_choice = get_integer_input("Select order number to accept (enter '0' to exit): ");
        if (assignment_choice >= 0 && assignment_choice <= count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", count);
    }
    
    if (assignment_choice == 0) {
        return 0;
    }
    
    QueueOrder selected_order;
    get_order_by_number_delivery(orders, count, assignment_choice, &selected_order);
    
    // Get restaurant and student details
    char restaurant_name[NAME_LENGTH], restaurant_contact[CONTACT_NUMBER_LENGTH];
    char student_name[NAME_LENGTH], student_contact[CONTACT_NUMBER_LENGTH];
    
    get_restaurant_info(selected_order.restaurant_id, restaurant_name, restaurant_contact);
    get_student_info(selected_order.student_id, student_name, student_contact);
    
    printf("\n--- ORDER ASSIGNMENT DETAILS ---\n");
    printf("Order ID: %s\n", selected_order.order_id);
    printf("Restaurant: %s (%s)\n", restaurant_name, restaurant_contact);
    printf("Student: %s (%s)\n", student_name, student_contact);
    printf("Items: %s\n", selected_order.items_summary);
    printf("Total: RM%.2f\n", selected_order.total_price);
    printf("Order Time: %s\n", selected_order.order_time);
    
    int confirm = confirmation("Are you sure you want to accept this delivery assignment? (1. Yes / 2. No): ");
    if (confirm == 1) {
        // Generate delivery ID
        char delivery_id[DELIVERY_ID_LENGTH];
        get_delivery_id(delivery_id, sizeof(delivery_id));
        
        // Create delivery record
        if (create_delivery_record(selected_order.order_id, delivery_id) == 1) {
            // Update delivery personnel status
            if (update_delivery_personnel_status(current_delivery_personnel_id, DELIVERING_ORDER, delivery_id) == 1) {
                printf("Order assignment successful!\n");
                printf("Delivery ID: %s\n", delivery_id);
                printf("You are now assigned to deliver this order.\n");
                printf("Please proceed to %s to collect the order.\n", restaurant_name);
                return 1;
            } else {
                printf("Failed to update delivery personnel status.\n");
                return 0;
            }
        } else {
            printf("Failed to create delivery record.\n");
            return 0;
        }
    } else {
        printf("Order assignment cancelled.\n");
        return 0;
    }
}

// Function to view delivery details
void view_delivery_details(void) {
    printf("\n----- DELIVERY DETAILS -----\n");
    
    // Get current delivery personnel status
    int status = get_delivery_personnel_status(current_delivery_personnel_id);
    
    if (status == NO_ORDER_ASSIGNED) {
        printf("You are currently not assigned to any delivery.\n");
        printf("Use 'Accept order assignment' to get assigned to a delivery.\n");
    } else {
        printf("Status: Currently delivering an order\n");
        
        // Show assigned orders
        QueueOrder orders[100];
        int count;
        
        if (find_assigned_orders(current_delivery_personnel_id, orders, &count) != FILE_ERROR && count > 0) {
            printf("\nYour current delivery:\n");
            display_assigned_orders(orders, count);
            
            // Show delivery details
            for (int i = 0; i < count; i++) {
                if (strcmp(orders[i].status, "DELIVERING") == 0) {
                    char restaurant_name[NAME_LENGTH], restaurant_contact[CONTACT_NUMBER_LENGTH];
                    char student_name[NAME_LENGTH], student_contact[CONTACT_NUMBER_LENGTH];
                    
                    get_restaurant_info(orders[i].restaurant_id, restaurant_name, restaurant_contact);
                    get_student_info(orders[i].student_id, student_name, student_contact);
                    
                    printf("\n--- DELIVERY INFORMATION ---\n");
                    printf("Pickup Location: %s (%s)\n", restaurant_name, restaurant_contact);
                    printf("Delivery To: %s (%s)\n", student_name, student_contact);
                    printf("Items: %s\n", orders[i].items_summary);
                    printf("Estimated Delivery Fee: RM%.2f\n", orders[i].total_price * 0.1); // 10% of order value
                    break;
                }
            }
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

// --------------------------------------------------------------------------------------------

// Forward declarations for delivery status management

int find_my_deliveries(char *delivery_personnel_id, DeliveryStatusInfo *deliveries, int *count);
void display_delivery_status(DeliveryStatusInfo *deliveries, int count);
void view_current_deliveries(void);
int update_delivery_status_action(void);
int mark_order_as_delivered(void);
int report_delivery_issue(void);
int update_estimated_arrival_time(void);
int get_delivery_by_number(DeliveryStatusInfo *deliveries, int count, int delivery_number, DeliveryStatusInfo *selected_delivery);
int update_delivery_status_in_file(const char *delivery_id, DeliveryStatus new_status);
const char* get_delivery_status_text(DeliveryStatus status);
const char* get_punctuality_status_text(PunctualityStatus status);
void get_current_time(char *time_str, int size);

#define VIEW_CURRENT_DELIVERIES 1
#define UPDATE_DELIVERY_STATUS 2
#define MARK_AS_DELIVERED 3
#define REPORT_DELIVERY_ISSUE 4
#define UPDATE_ETA 5

int delivery_status_management(void) {
    int choice;
    while (1) {
        printf("\n----- DELIVERY STATUS MANAGEMENT -----\n");
        printf("1. View current deliveries\n");
        printf("2. Update delivery status\n");
        printf("3. Mark order as delivered\n");
        printf("4. Report delivery issue\n");
        printf("5. Update estimated arrival time\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_CURRENT_DELIVERIES:
                view_current_deliveries();
                break;
            case UPDATE_DELIVERY_STATUS:
                update_delivery_status_action();
                break;
            case MARK_AS_DELIVERED:
                mark_order_as_delivered();
                break;
            case REPORT_DELIVERY_ISSUE:
                report_delivery_issue();
                break;
            case UPDATE_ETA:
                update_estimated_arrival_time();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get delivery status text
const char* get_delivery_status_text(DeliveryStatus status) {
    switch (status) {
        case DELIVERED: return "Delivered";
        case DELIVERING: return "Delivering";
        case UNDELIVERED: return "Undelivered";
        default: return "Unknown";
    }
}

// Function to get punctuality status text
const char* get_punctuality_status_text(PunctualityStatus status) {
    switch (status) {
        case ON_TIME: return "On Time";
        case LATE: return "Late";
        default: return "Unknown";
    }
}

// Function to get current time (simplified)
void get_current_time(char *time_str, int size) {
    // For simplicity, using a fixed current time
    // In a real system, this would get actual current time
    snprintf(time_str, size, "16:30:00");
}

// Function to find deliveries for current delivery personnel
int find_my_deliveries(char *delivery_personnel_id, DeliveryStatusInfo *deliveries, int *count) {
    FILE *delivery_fp = fopen(DELIVERY_FILE, "r");
    if (delivery_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", DELIVERY_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    
    // Get delivery records for this personnel
    while (fgets(line, sizeof(line), delivery_fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';
        
        char delivery_id[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               delivery_id, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);
        
        // Check if this delivery belongs to current personnel
        FILE *personnel_fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
        if (personnel_fp != NULL) {
            char personnel_line[MAX_LINE_LENGTH];
            while (fgets(personnel_line, sizeof(personnel_line), personnel_fp)) {
                char pid[DELIVERY_PERSONNEL_ID_LENGTH], name[NAME_LENGTH];
                char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
                int status;
                char assigned_delivery_id[DELIVERY_ID_LENGTH];
                
                sscanf(personnel_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s",
                       pid, name, contact, email, password, &status, assigned_delivery_id);
                
                if (strcmp(pid, delivery_personnel_id) == 0 && 
                    (strcmp(assigned_delivery_id, delivery_id) == 0 || status == DELIVERING_ORDER)) {
                    
                    // Get order details
                    FILE *order_fp = fopen(ORDER_FILE, "r");
                    if (order_fp != NULL) {
                        char order_line[MAX_LINE_LENGTH];
                        while (fgets(order_line, sizeof(order_line), order_fp)) {
                            char oid[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
                            char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
                            float total_price;
                            char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
                            
                            sscanf(order_line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                                   oid, student_id, restaurant_id, &total_price,
                                   order_date, order_time, order_status);
                            
                            if (strcmp(oid, order_id) == 0) {
                                // Fill delivery status info
                                strcpy(deliveries[*count].delivery_id, delivery_id);
                                strcpy(deliveries[*count].order_id, order_id);
                                strcpy(deliveries[*count].delivery_date, delivery_date);
                                strcpy(deliveries[*count].estimated_time_arrival, eta);
                                strcpy(deliveries[*count].delivered_time, delivered_time);
                                deliveries[*count].delivery_status = (DeliveryStatus)delivery_status;
                                deliveries[*count].punctuality_status = (PunctualityStatus)punctuality_status;
                                deliveries[*count].order_total = total_price;
                                
                                // Get restaurant and student names
                                get_restaurant_info(restaurant_id, deliveries[*count].restaurant_name, NULL);
                                get_student_info(student_id, deliveries[*count].student_name, NULL);
                                
                                // Get items summary
                                get_order_items_summary(order_id, deliveries[*count].items_summary, sizeof(deliveries[*count].items_summary));
                                
                                (*count)++;
                                break;
                            }
                        }
                        fclose(order_fp);
                    }
                    break;
                }
            }
            fclose(personnel_fp);
        }
    }
    
    fclose(delivery_fp);
    return *count;
}

// Function to display delivery status
void display_delivery_status(DeliveryStatusInfo *deliveries, int count) {
    if (count == 0) {
        printf("No deliveries found.\n");
        return;
    }

    printf("\n==================== DELIVERY STATUS ====================\n");
    printf("%-4s %-8s %-8s %-15s %-12s %-10s %-12s %-10s\n", 
           "No.", "Del. ID", "Order ID", "Restaurant", "Student", "Status", "ETA", "Delivered");
    printf("-----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        const char *delivered_time = strlen(deliveries[i].delivered_time) > 0 ? 
                                   deliveries[i].delivered_time : "Pending";
        
        printf("%-4d %-8s %-8s %-15s %-12s %-10s %-12s %-10s\n", 
               i + 1, deliveries[i].delivery_id, deliveries[i].order_id,
               deliveries[i].restaurant_name, deliveries[i].student_name,
               get_delivery_status_text(deliveries[i].delivery_status),
               deliveries[i].estimated_time_arrival, delivered_time);
    }
    printf("-----------------------------------------------------------------------------------\n");
}

// Function to view current deliveries
void view_current_deliveries(void) {
    DeliveryStatusInfo deliveries[100];
    int count;
    
    printf("\n----- VIEW CURRENT DELIVERIES -----\n");

    if (find_my_deliveries(current_delivery_personnel_id, deliveries, &count) == FILE_ERROR) return;

    display_delivery_status(deliveries, count);
    printf("Total deliveries: %d\n", count);
    
    // Show detailed information for active deliveries
    for (int i = 0; i < count; i++) {
        if (deliveries[i].delivery_status == DELIVERING) {
            printf("\n--- ACTIVE DELIVERY DETAILS ---\n");
            printf("Delivery ID: %s\n", deliveries[i].delivery_id);
            printf("Order ID: %s\n", deliveries[i].order_id);
            printf("Items: %s\n", deliveries[i].items_summary);
            printf("Order Total: RM%.2f\n", deliveries[i].order_total);
            printf("Estimated Delivery Fee: RM%.2f\n", deliveries[i].order_total * 0.1);
            printf("Pickup: %s\n", deliveries[i].restaurant_name);
            printf("Deliver To: %s\n", deliveries[i].student_name);
            printf("ETA: %s\n", deliveries[i].estimated_time_arrival);
            break;
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

// Function to get delivery by number
int get_delivery_by_number(DeliveryStatusInfo *deliveries, int count, int delivery_number, DeliveryStatusInfo *selected_delivery) {
    if (delivery_number < 1 || delivery_number > count) {
        return 0;
    }
    
    *selected_delivery = deliveries[delivery_number - 1];
    return 1;
}

// Function to update delivery status in file
int update_delivery_status_in_file(const char *delivery_id, DeliveryStatus new_status) {
    FILE *fp = fopen(DELIVERY_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", DELIVERY_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_delivery.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        char did[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;

        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               did, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);

        if (strcmp(did, delivery_id) == 0) {
            // Update delivery status and set delivered time if marking as delivered
            if (new_status == DELIVERED && strlen(delivered_time) == 0) {
                get_current_time(delivered_time, sizeof(delivered_time));
            }
            
            fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%d\n",
                    did, order_id, delivery_date, eta, delivered_time,
                    (int)new_status, punctuality_status);
            updated = 1;
        } else {
            fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%d\n",
                    did, order_id, delivery_date, eta, delivered_time,
                    delivery_status, punctuality_status);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (updated) {
        remove(DELIVERY_FILE);
        rename("temp_delivery.txt", DELIVERY_FILE);
        printf("Delivery status updated successfully!\n");
        return 1;
    } else {
        remove("temp_delivery.txt");
        printf("Delivery not found for status update.\n");
        return 0;
    }
}

// Function to update delivery status
int update_delivery_status_action(void) {
    DeliveryStatusInfo deliveries[100];
    int count;
    
    printf("\n----- UPDATE DELIVERY STATUS -----\n");
    
    if (find_my_deliveries(current_delivery_personnel_id, deliveries, &count) == FILE_ERROR) {
        return 0;
    }
    
    // Filter to show only active deliveries
    DeliveryStatusInfo active_deliveries[100];
    int active_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (deliveries[i].delivery_status != DELIVERED) {
            active_deliveries[active_count] = deliveries[i];
            active_count++;
        }
    }
    
    if (active_count == 0) {
        printf("No active deliveries found to update.\n");
        return 0;
    }
    
    display_delivery_status(active_deliveries, active_count);
    
    int update_choice;
    while (1) {
        update_choice = get_integer_input("Select delivery number to update (enter '0' to exit): ");
        if (update_choice >= 0 && update_choice <= active_count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", active_count);
    }
    
    if (update_choice == 0) {
        return 0;
    }
    
    DeliveryStatusInfo selected_delivery;
    get_delivery_by_number(active_deliveries, active_count, update_choice, &selected_delivery);
    
    printf("\nSelected Delivery: %s\n", selected_delivery.delivery_id);
    printf("Current Status: %s\n", get_delivery_status_text(selected_delivery.delivery_status));
    printf("Order: %s\n", selected_delivery.items_summary);
    
    printf("\nAvailable Status Options:\n");
    printf("1. UNDELIVERED - Order not yet picked up\n");
    printf("2. DELIVERING - Currently delivering the order\n");
    printf("3. DELIVERED - Order has been delivered\n");
    
    int status_choice = get_integer_input("Select new status (1-3): ");
    
    DeliveryStatus new_status;
    switch (status_choice) {
        case 1: new_status = UNDELIVERED; break;
        case 2: new_status = DELIVERING; break;
        case 3: new_status = DELIVERED; break;
        default:
            printf("Invalid status choice.\n");
            return 0;
    }
    
    int confirm = confirmation("Are you sure you want to update this delivery status? (1. Yes / 2. No): ");
    if (confirm == 1) {
        int result = update_delivery_status_in_file(selected_delivery.delivery_id, new_status);
        
        // If marking as delivered, update personnel status to available
        if (new_status == DELIVERED && result == 1) {
            update_delivery_personnel_status(current_delivery_personnel_id, NO_ORDER_ASSIGNED, "none");
            printf("You are now available for new deliveries.\n");
        }
        
        return result;
    } else {
        printf("Delivery status update cancelled.\n");
        return 0;
    }
}

// Function to mark order as delivered
int mark_order_as_delivered(void) {
    DeliveryStatusInfo deliveries[100];
    int count;
    
    printf("\n----- MARK ORDER AS DELIVERED -----\n");
    
    if (find_my_deliveries(current_delivery_personnel_id, deliveries, &count) == FILE_ERROR) {
        return 0;
    }
    
    // Filter to show only delivering orders
    DeliveryStatusInfo delivering_orders[100];
    int delivering_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (deliveries[i].delivery_status == DELIVERING) {
            delivering_orders[delivering_count] = deliveries[i];
            delivering_count++;
        }
    }
    
    if (delivering_count == 0) {
        printf("No orders currently being delivered.\n");
        return 0;
    }
    
    display_delivery_status(delivering_orders, delivering_count);
    
    int delivery_choice;
    while (1) {
        delivery_choice = get_integer_input("Select delivery to mark as delivered (enter '0' to exit): ");
        if (delivery_choice >= 0 && delivery_choice <= delivering_count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", delivering_count);
    }
    
    if (delivery_choice == 0) {
        return 0;
    }
    
    DeliveryStatusInfo selected_delivery;
    get_delivery_by_number(delivering_orders, delivering_count, delivery_choice, &selected_delivery);
    
    printf("\n--- DELIVERY COMPLETION ---\n");
    printf("Delivery ID: %s\n", selected_delivery.delivery_id);
    printf("Order: %s\n", selected_delivery.items_summary);
    printf("Customer: %s\n", selected_delivery.student_name);
    printf("Delivery Fee Earned: RM%.2f\n", selected_delivery.order_total * 0.1);
    
    int confirm = confirmation("Confirm that this order has been delivered? (1. Yes / 2. No): ");
    if (confirm == 1) {
        int result = update_delivery_status_in_file(selected_delivery.delivery_id, DELIVERED);
        
        if (result == 1) {
            // Update personnel status to available
            update_delivery_personnel_status(current_delivery_personnel_id, NO_ORDER_ASSIGNED, "none");
            printf("Order marked as delivered successfully!\n");
            printf("You have earned RM%.2f for this delivery.\n", selected_delivery.order_total * 0.1);
            printf("You are now available for new deliveries.\n");
        }
        
        return result;
    } else {
        printf("Delivery completion cancelled.\n");
        return 0;
    }
}

// Function to report delivery issue
int report_delivery_issue(void) {
    DeliveryStatusInfo deliveries[100];
    int count;
    
    printf("\n----- REPORT DELIVERY ISSUE -----\n");
    
    if (find_my_deliveries(current_delivery_personnel_id, deliveries, &count) == FILE_ERROR) {
        return 0;
    }
    
    // Filter to show only active deliveries
    DeliveryStatusInfo active_deliveries[100];
    int active_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (deliveries[i].delivery_status != DELIVERED) {
            active_deliveries[active_count] = deliveries[i];
            active_count++;
        }
    }
    
    if (active_count == 0) {
        printf("No active deliveries to report issues for.\n");
        return 0;
    }
    
    display_delivery_status(active_deliveries, active_count);
    
    int issue_choice;
    while (1) {
        issue_choice = get_integer_input("Select delivery to report issue (enter '0' to exit): ");
        if (issue_choice >= 0 && issue_choice <= active_count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", active_count);
    }
    
    if (issue_choice == 0) {
        return 0;
    }
    
    DeliveryStatusInfo selected_delivery;
    get_delivery_by_number(active_deliveries, active_count, issue_choice, &selected_delivery);
    
    printf("\n--- DELIVERY ISSUE REPORT ---\n");
    printf("Delivery ID: %s\n", selected_delivery.delivery_id);
    printf("Order: %s\n", selected_delivery.items_summary);
    
    printf("\nCommon Issues:\n");
    printf("1. Customer not available\n");
    printf("2. Wrong address\n");
    printf("3. Order not ready at restaurant\n");
    printf("4. Traffic/weather delay\n");
    printf("5. Other issue\n");
    
    int issue_type = get_integer_input("Select issue type (1-5): ");
    
    const char *issue_description;
    switch (issue_type) {
        case 1: issue_description = "Customer not available"; break;
        case 2: issue_description = "Wrong address"; break;
        case 3: issue_description = "Order not ready at restaurant"; break;
        case 4: issue_description = "Traffic/weather delay"; break;
        case 5: issue_description = "Other issue"; break;
        default:
            printf("Invalid issue type.\n");
            return 0;
    }
    
    printf("\nIssue: %s\n", issue_description);
    printf("This issue has been logged for delivery %s.\n", selected_delivery.delivery_id);
    printf("The system administrator will be notified.\n");
    
    // In a real system, this would log the issue to a separate file
    printf("Issue reported successfully!\n");
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("Invalid input! Please enter 0 to exit.\n");
        }
    }
    
    return 1;
}

// Function to update estimated arrival time
int update_estimated_arrival_time(void) {
    DeliveryStatusInfo deliveries[100];
    int count;
    
    printf("\n----- UPDATE ESTIMATED ARRIVAL TIME -----\n");
    
    if (find_my_deliveries(current_delivery_personnel_id, deliveries, &count) == FILE_ERROR) {
        return 0;
    }
    
    // Filter to show only delivering orders
    DeliveryStatusInfo delivering_orders[100];
    int delivering_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (deliveries[i].delivery_status == DELIVERING) {
            delivering_orders[delivering_count] = deliveries[i];
            delivering_count++;
        }
    }
    
    if (delivering_count == 0) {
        printf("No orders currently being delivered.\n");
        return 0;
    }
    
    display_delivery_status(delivering_orders, delivering_count);
    
    int eta_choice;
    while (1) {
        eta_choice = get_integer_input("Select delivery to update ETA (enter '0' to exit): ");
        if (eta_choice >= 0 && eta_choice <= delivering_count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", delivering_count);
    }
    
    if (eta_choice == 0) {
        return 0;
    }
    
    DeliveryStatusInfo selected_delivery;
    get_delivery_by_number(delivering_orders, delivering_count, eta_choice, &selected_delivery);
    
    printf("\n--- UPDATE ETA ---\n");
    printf("Delivery ID: %s\n", selected_delivery.delivery_id);
    printf("Current ETA: %s\n", selected_delivery.estimated_time_arrival);
    
    char new_eta[TIME_LENGTH];
    printf("Enter new estimated arrival time (HH:MM:SS format): ");
    fgets(new_eta, sizeof(new_eta), stdin);
    new_eta[strcspn(new_eta, "\n")] = '\0';
    
    // Basic validation for time format
    if (strlen(new_eta) != 8 || new_eta[2] != ':' || new_eta[5] != ':') {
        printf("Invalid time format. Please use HH:MM:SS format.\n");
        return 0;
    }
    
    printf("New ETA: %s\n", new_eta);
    
    // Use safe integer input after string input to prevent buffer issues
    int confirm = safe_integer_input_after_string("Confirm ETA update? (1. Yes / 2. No): ");
    if (confirm == 1) {
        // Update ETA in delivery file
        FILE *fp = fopen(DELIVERY_FILE, "r");
        if (fp == NULL) {
            printf("Error: Could not open delivery file.\n");
            return 0;
        }

        FILE *temp_fp = fopen("temp_delivery_eta.txt", "w");
        if (temp_fp == NULL) {
            printf("Error: Could not create temporary file.\n");
            fclose(fp);
            return 0;
        }

        char line[MAX_LINE_LENGTH];
        int updated = 0;

        while (fgets(line, sizeof(line), fp)) {
            char did[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
            char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
            int delivery_status, punctuality_status;

            sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
                   did, order_id, delivery_date, eta, delivered_time,
                   &delivery_status, &punctuality_status);

            if (strcmp(did, selected_delivery.delivery_id) == 0) {
                fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%d\n",
                        did, order_id, delivery_date, new_eta, delivered_time,
                        delivery_status, punctuality_status);
                updated = 1;
            } else {
                fprintf(temp_fp, "%s,%s,%s,%s,%s,%d,%d\n",
                        did, order_id, delivery_date, eta, delivered_time,
                        delivery_status, punctuality_status);
            }
        }

        fclose(fp);
        fclose(temp_fp);

        if (updated) {
            remove(DELIVERY_FILE);
            rename("temp_delivery_eta.txt", DELIVERY_FILE);
            printf("ETA updated successfully to %s!\n", new_eta);
            return 1;
        } else {
            remove("temp_delivery_eta.txt");
            printf("Failed to update ETA.\n");
            return 0;
        }
    } else {
        printf("ETA update cancelled.\n");
        return 0;
    }
}

// --------------------------------------------------------------------------------------------

// Forward declarations for earnings calculation and tracking

int calculate_delivery_earnings(char *delivery_personnel_id, EarningsRecord *earnings, int *count);
void display_earnings_records(EarningsRecord *earnings, int count);
void view_daily_earnings(void);
void view_monthly_earnings(void);
void view_earnings_summary(void);
void generate_earnings_report(void);
float calculate_base_fee(float order_total);
float calculate_bonus(PunctualityStatus punctuality, float base_fee);
float calculate_penalty(PunctualityStatus punctuality, float base_fee);
void calculate_earnings_summary(EarningsRecord *earnings, int count, EarningsSummary *summary);
void display_earnings_summary(EarningsSummary *summary);
int filter_earnings_by_date(EarningsRecord *all_earnings, int total_count, const char *target_date, EarningsRecord *filtered_earnings);
void get_current_date(char *date_str, int size);

#define VIEW_DAILY_EARNINGS 1
#define VIEW_MONTHLY_EARNINGS 2
#define VIEW_EARNINGS_SUMMARY 3
#define GENERATE_EARNINGS_REPORT 4

// Earnings calculation constants
#define BASE_FEE_RATE 0.10           // 10% of order value as base fee
#define ON_TIME_BONUS_RATE 0.02      // 2% bonus for on-time delivery
#define LATE_PENALTY_RATE 0.03       // 3% penalty for late delivery
#define MIN_BASE_FEE 2.00            // Minimum base fee RM2.00
#define MAX_BASE_FEE 15.00           // Maximum base fee RM15.00

int earnings_calculation_and_tracking(void) {
    int choice;
    while (1) {
        printf("\n----- EARNINGS CALCULATION AND TRACKING -----\n");
        printf("1. View daily earnings\n");
        printf("2. View monthly earnings\n");
        printf("3. View earnings summary\n");
        printf("4. Generate earnings report\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_DAILY_EARNINGS:
                view_daily_earnings();
                break;
            case VIEW_MONTHLY_EARNINGS:
                view_monthly_earnings();
                break;
            case VIEW_EARNINGS_SUMMARY:
                view_earnings_summary();
                break;
            case GENERATE_EARNINGS_REPORT:
                generate_earnings_report();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get current date (simplified)
void get_current_date(char *date_str, int size) {
    // For simplicity, using a fixed current date
    // In a real system, this would get actual current date
    snprintf(date_str, size, "28-12-2024");
}

// Function to calculate base delivery fee
float calculate_base_fee(float order_total) {
    float base_fee = order_total * BASE_FEE_RATE;
    
    // Apply minimum and maximum limits
    if (base_fee < MIN_BASE_FEE) {
        base_fee = MIN_BASE_FEE;
    } else if (base_fee > MAX_BASE_FEE) {
        base_fee = MAX_BASE_FEE;
    }
    
    return base_fee;
}

// Function to calculate bonus for on-time delivery
float calculate_bonus(PunctualityStatus punctuality, float base_fee) {
    if (punctuality == ON_TIME) {
        return base_fee * ON_TIME_BONUS_RATE;
    }
    return 0.0;
}

// Function to calculate penalty for late delivery
float calculate_penalty(PunctualityStatus punctuality, float base_fee) {
    if (punctuality == LATE) {
        return base_fee * LATE_PENALTY_RATE;
    }
    return 0.0;
}

// Function to calculate delivery earnings for current personnel
int calculate_delivery_earnings(char *delivery_personnel_id, EarningsRecord *earnings, int *count) {
    FILE *delivery_fp = fopen(DELIVERY_FILE, "r");
    if (delivery_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", DELIVERY_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    
    // Get delivery records for this personnel
    while (fgets(line, sizeof(line), delivery_fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';
        
        char delivery_id[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               delivery_id, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);
        
        // Only process delivered orders
        if (delivery_status != DELIVERED) {
            continue;
        }
        
        // Check if this delivery belongs to current personnel
        FILE *personnel_fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
        if (personnel_fp != NULL) {
            char personnel_line[MAX_LINE_LENGTH];
            while (fgets(personnel_line, sizeof(personnel_line), personnel_fp)) {
                char pid[DELIVERY_PERSONNEL_ID_LENGTH], name[NAME_LENGTH];
                char contact[CONTACT_NUMBER_LENGTH], email[EMAIL_ADDRESS_LENGTH], password[PASSWORD_LENGTH];
                int status;
                char assigned_delivery_id[DELIVERY_ID_LENGTH];
                
                sscanf(personnel_line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s",
                       pid, name, contact, email, password, &status, assigned_delivery_id);
                
                if (strcmp(pid, delivery_personnel_id) == 0 && 
                    (strcmp(assigned_delivery_id, delivery_id) == 0 || strstr(assigned_delivery_id, delivery_id) != NULL)) {
                    
                    // Get order details for earnings calculation
                    FILE *order_fp = fopen(ORDER_FILE, "r");
                    if (order_fp != NULL) {
                        char order_line[MAX_LINE_LENGTH];
                        while (fgets(order_line, sizeof(order_line), order_fp)) {
                            char oid[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
                            char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
                            float total_price;
                            char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
                            
                            sscanf(order_line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                                   oid, student_id, restaurant_id, &total_price,
                                   order_date, order_time, order_status);
                            
                            if (strcmp(oid, order_id) == 0) {
                                // Calculate earnings for this delivery
                                strcpy(earnings[*count].delivery_id, delivery_id);
                                strcpy(earnings[*count].order_id, order_id);
                                strcpy(earnings[*count].delivery_date, delivery_date);
                                strcpy(earnings[*count].delivered_time, delivered_time);
                                earnings[*count].order_total = total_price;
                                earnings[*count].punctuality = (PunctualityStatus)punctuality_status;
                                
                                // Calculate fees, bonuses, and penalties
                                earnings[*count].base_fee = calculate_base_fee(total_price);
                                earnings[*count].bonus = calculate_bonus((PunctualityStatus)punctuality_status, earnings[*count].base_fee);
                                earnings[*count].penalty = calculate_penalty((PunctualityStatus)punctuality_status, earnings[*count].base_fee);
                                earnings[*count].net_earnings = earnings[*count].base_fee + earnings[*count].bonus - earnings[*count].penalty;
                                
                                // Get restaurant name and items summary
                                get_restaurant_info(restaurant_id, earnings[*count].restaurant_name, NULL);
                                get_order_items_summary(order_id, earnings[*count].items_summary, sizeof(earnings[*count].items_summary));
                                
                                (*count)++;
                                break;
                            }
                        }
                        fclose(order_fp);
                    }
                    break;
                }
            }
            fclose(personnel_fp);
        }
    }
    
    fclose(delivery_fp);
    return *count;
}

// Function to display earnings records
void display_earnings_records(EarningsRecord *earnings, int count) {
    if (count == 0) {
        printf("No earnings records found.\n");
        return;
    }

    printf("\n==================== EARNINGS RECORDS ====================\n");
    printf("%-8s %-8s %-12s %-15s %-8s %-8s %-8s %-8s %-10s\n", 
           "Del. ID", "Order ID", "Date", "Restaurant", "Base", "Bonus", "Penalty", "Net", "Status");
    printf("-------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        const char *status = (earnings[i].punctuality == ON_TIME) ? "On Time" : "Late";
        
        printf("%-8s %-8s %-12s %-15s RM%-6.2f RM%-6.2f RM%-6.2f RM%-6.2f %-10s\n", 
               earnings[i].delivery_id, earnings[i].order_id, earnings[i].delivery_date,
               earnings[i].restaurant_name, earnings[i].base_fee, earnings[i].bonus,
               earnings[i].penalty, earnings[i].net_earnings, status);
    }
    printf("-------------------------------------------------------------------------------------\n");
}

// Function to calculate earnings summary
void calculate_earnings_summary(EarningsRecord *earnings, int count, EarningsSummary *summary) {
    summary->total_deliveries = count;
    summary->on_time_deliveries = 0;
    summary->late_deliveries = 0;
    summary->total_earnings = 0.0;
    summary->total_base_fees = 0.0;
    summary->total_bonuses = 0.0;
    summary->total_penalties = 0.0;
    
    for (int i = 0; i < count; i++) {
        summary->total_earnings += earnings[i].net_earnings;
        summary->total_base_fees += earnings[i].base_fee;
        summary->total_bonuses += earnings[i].bonus;
        summary->total_penalties += earnings[i].penalty;
        
        if (earnings[i].punctuality == ON_TIME) {
            summary->on_time_deliveries++;
        } else {
            summary->late_deliveries++;
        }
    }
    
    summary->average_earnings_per_delivery = (count > 0) ? (summary->total_earnings / count) : 0.0;
    summary->on_time_percentage = (count > 0) ? ((float)summary->on_time_deliveries / count * 100.0) : 0.0;
}

// Function to display earnings summary
void display_earnings_summary(EarningsSummary *summary) {
    printf("\n==================== EARNINGS SUMMARY ====================\n");
    printf("Total Deliveries: %d\n", summary->total_deliveries);
    printf("On-Time Deliveries: %d\n", summary->on_time_deliveries);
    printf("Late Deliveries: %d\n", summary->late_deliveries);
    printf("On-Time Percentage: %.1f%%\n", summary->on_time_percentage);
    
    printf("\n--- FINANCIAL BREAKDOWN ---\n");
    printf("Total Base Fees: RM%.2f\n", summary->total_base_fees);
    printf("Total Bonuses: RM%.2f\n", summary->total_bonuses);
    printf("Total Penalties: RM%.2f\n", summary->total_penalties);
    printf("Net Total Earnings: RM%.2f\n", summary->total_earnings);
    printf("Average Earnings per Delivery: RM%.2f\n", summary->average_earnings_per_delivery);
    
    printf("\n--- PERFORMANCE INSIGHTS ---\n");
    if (summary->on_time_percentage >= 90.0) {
        printf("Excellent performance! Keep up the great work!\n");
    } else if (summary->on_time_percentage >= 75.0) {
        printf("Good performance. Try to improve punctuality for more bonuses.\n");
    } else {
        printf("Performance needs improvement. Focus on on-time deliveries.\n");
    }
    
    if (summary->total_penalties > 0) {
        printf("Tip: Reduce late deliveries to avoid penalties and increase earnings.\n");
    }
    
    printf("===========================================================\n");
}

// Function to filter earnings by date
int filter_earnings_by_date(EarningsRecord *all_earnings, int total_count, const char *target_date, EarningsRecord *filtered_earnings) {
    int filtered_count = 0;
    
    for (int i = 0; i < total_count; i++) {
        if (strcmp(all_earnings[i].delivery_date, target_date) == 0) {
            filtered_earnings[filtered_count] = all_earnings[i];
            filtered_count++;
        }
    }
    
    return filtered_count;
}

// Function to view daily earnings
void view_daily_earnings(void) {
    EarningsRecord earnings[100];
    int count;
    
    printf("\n----- VIEW DAILY EARNINGS -----\n");
    
    if (calculate_delivery_earnings(current_delivery_personnel_id, earnings, &count) == FILE_ERROR) return;
    
    char target_date[DATE_LENGTH];
    printf("Enter date to view earnings (DD-MM-YYYY) or press Enter for today: ");
    fgets(target_date, sizeof(target_date), stdin);
    target_date[strcspn(target_date, "\n")] = '\0';
    
    if (strlen(target_date) == 0) {
        get_current_date(target_date, sizeof(target_date));
    }
    
    // Filter earnings for the specified date
    EarningsRecord daily_earnings[100];
    int daily_count = filter_earnings_by_date(earnings, count, target_date, daily_earnings);
    
    printf("\nEarnings for %s:\n", target_date);
    display_earnings_records(daily_earnings, daily_count);
    
    if (daily_count > 0) {
        EarningsSummary daily_summary;
        calculate_earnings_summary(daily_earnings, daily_count, &daily_summary);
        
        printf("\n--- DAILY SUMMARY ---\n");
        printf("Deliveries Completed: %d\n", daily_summary.total_deliveries);
        printf("Total Earnings: RM%.2f\n", daily_summary.total_earnings);
        printf("Average per Delivery: RM%.2f\n", daily_summary.average_earnings_per_delivery);
        printf("On-Time Rate: %.1f%%\n", daily_summary.on_time_percentage);
    } else {
        printf("No deliveries completed on %s.\n", target_date);
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("\nInvalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view monthly earnings
void view_monthly_earnings(void) {
    EarningsRecord earnings[100];
    int count;
    
    printf("\n----- VIEW MONTHLY EARNINGS -----\n");
    
    if (calculate_delivery_earnings(current_delivery_personnel_id, earnings, &count) == FILE_ERROR) return;
    
    char target_month[8];
    printf("Enter month to view earnings (MM-YYYY) or press Enter for current month: ");
    fgets(target_month, sizeof(target_month), stdin);
    target_month[strcspn(target_month, "\n")] = '\0';
    
    if (strlen(target_month) == 0) {
        strcpy(target_month, "12-2024"); // Default to current month
    }
    
    // Filter earnings for the specified month
    EarningsRecord monthly_earnings[100];
    int monthly_count = 0;
    
    for (int i = 0; i < count; i++) {
        // Extract month-year from date (DD-MM-YYYY)
        char date_month_year[8];
        if (strlen(earnings[i].delivery_date) >= 10) {
            snprintf(date_month_year, sizeof(date_month_year), "%c%c-%c%c%c%c", 
                     earnings[i].delivery_date[3], earnings[i].delivery_date[4],
                     earnings[i].delivery_date[6], earnings[i].delivery_date[7],
                     earnings[i].delivery_date[8], earnings[i].delivery_date[9]);
            
            if (strcmp(date_month_year, target_month) == 0) {
                monthly_earnings[monthly_count] = earnings[i];
                monthly_count++;
            }
        }
    }
    
    printf("\nEarnings for %s:\n", target_month);
    display_earnings_records(monthly_earnings, monthly_count);
    
    if (monthly_count > 0) {
        EarningsSummary monthly_summary;
        calculate_earnings_summary(monthly_earnings, monthly_count, &monthly_summary);
        
        printf("\n--- MONTHLY SUMMARY ---\n");
        printf("Total Deliveries: %d\n", monthly_summary.total_deliveries);
        printf("Total Earnings: RM%.2f\n", monthly_summary.total_earnings);
        printf("Average per Delivery: RM%.2f\n", monthly_summary.average_earnings_per_delivery);
        printf("On-Time Rate: %.1f%%\n", monthly_summary.on_time_percentage);
        
        // Calculate daily average
        printf("Average Daily Earnings: RM%.2f (assuming 30 days)\n", monthly_summary.total_earnings / 30.0);
    } else {
        printf("No deliveries completed in %s.\n", target_month);
    }
      
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("\nInvalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view earnings summary
void view_earnings_summary(void) {
    EarningsRecord earnings[100];
    int count;
    
    printf("\n----- VIEW EARNINGS SUMMARY -----\n");
    
    if (calculate_delivery_earnings(current_delivery_personnel_id, earnings, &count) == FILE_ERROR) return;
    
    if (count == 0) {
        printf("No earnings records found.\n");
        printf("Complete some deliveries to see your earnings summary.\n");
        return;
    }
    
    EarningsSummary summary;
    calculate_earnings_summary(earnings, count, &summary);
    
    display_earnings_summary(&summary);
    
    // Show recent deliveries
    printf("\n--- RECENT DELIVERIES ---\n");
    int recent_count = (count > 5) ? 5 : count;
    display_earnings_records(&earnings[count - recent_count], recent_count);
    
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

// Function to generate earnings report
void generate_earnings_report(void) {
    EarningsRecord earnings[100];
    int count;
    
    printf("\n----- GENERATE EARNINGS REPORT -----\n");
    
    if (calculate_delivery_earnings(current_delivery_personnel_id, earnings, &count) == FILE_ERROR) return;
    
    if (count == 0) {
        printf("No earnings data available to generate report.\n");
        return;
    }
    
    printf("Generating comprehensive earnings report...\n");
    
    // Create report filename
    char report_filename[50];
    snprintf(report_filename, sizeof(report_filename), "earnings_report_%s.txt", current_delivery_personnel_id);
    
    FILE *report_fp = fopen(report_filename, "w");
    if (report_fp == NULL) {
        printf("Error: Could not create report file.\n");
        return;
    }
    
    // Write report header
    fprintf(report_fp, "==================== EARNINGS REPORT ====================\n");
    fprintf(report_fp, "Delivery Personnel ID: %s\n", current_delivery_personnel_id);
    fprintf(report_fp, "Report Generated: 28-12-2024\n");
    fprintf(report_fp, "Total Records: %d deliveries\n", count);
    fprintf(report_fp, "=========================================================\n\n");
    
    // Write earnings summary
    EarningsSummary summary;
    calculate_earnings_summary(earnings, count, &summary);
    
    fprintf(report_fp, "EARNINGS SUMMARY:\n");
    fprintf(report_fp, "Total Deliveries: %d\n", summary.total_deliveries);
    fprintf(report_fp, "On-Time Deliveries: %d (%.1f%%)\n", summary.on_time_deliveries, summary.on_time_percentage);
    fprintf(report_fp, "Late Deliveries: %d\n", summary.late_deliveries);
    fprintf(report_fp, "Total Base Fees: RM%.2f\n", summary.total_base_fees);
    fprintf(report_fp, "Total Bonuses: RM%.2f\n", summary.total_bonuses);
    fprintf(report_fp, "Total Penalties: RM%.2f\n", summary.total_penalties);
    fprintf(report_fp, "Net Total Earnings: RM%.2f\n", summary.total_earnings);
    fprintf(report_fp, "Average Earnings per Delivery: RM%.2f\n\n", summary.average_earnings_per_delivery);
    
    // Write detailed records
    fprintf(report_fp, "DETAILED EARNINGS RECORDS:\n");
    fprintf(report_fp, "%-8s %-8s %-12s %-15s %-8s %-8s %-8s %-8s %-10s\n", 
            "Del. ID", "Order ID", "Date", "Restaurant", "Base", "Bonus", "Penalty", "Net", "Status");
    fprintf(report_fp, "-------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        const char *status = (earnings[i].punctuality == ON_TIME) ? "On Time" : "Late";
        
        fprintf(report_fp, "%-8s %-8s %-12s %-15s RM%-6.2f RM%-6.2f RM%-6.2f RM%-6.2f %-10s\n", 
                earnings[i].delivery_id, earnings[i].order_id, earnings[i].delivery_date,
                earnings[i].restaurant_name, earnings[i].base_fee, earnings[i].bonus,
                earnings[i].penalty, earnings[i].net_earnings, status);
    }
    
    fprintf(report_fp, "\n==================== END OF REPORT ====================\n");
    
    fclose(report_fp);
    
    printf("Earnings report generated successfully!\n");
    printf("Report saved as: %s\n", report_filename);
    printf("The report contains:\n");
    printf("- Complete earnings summary\n");
    printf("- Detailed delivery records\n");
    printf("- Performance statistics\n");
    printf("- Bonus and penalty breakdown\n");
    
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

// --------------------------------------------------------------------------------------------

// Forward declarations for performance statistics

int calculate_performance_metrics(char *delivery_personnel_id, PerformanceMetrics *metrics);
int get_delivery_time_records(char *delivery_personnel_id, DeliveryTimeRecord *records, int *count);
void display_performance_overview(PerformanceMetrics *metrics);
void view_delivery_success_rate(void);
void view_average_delivery_time(void);
void view_performance_trends(void);
void view_performance_ranking(void);
int calculate_delivery_time_minutes(const char *order_time, const char *delivered_time);
char calculate_performance_grade(float success_rate, float on_time_rate);
void display_delivery_time_analysis(DeliveryTimeRecord *records, int count);
void display_performance_trends(PerformanceMetrics *metrics);
int find_performance_streak(char *delivery_personnel_id, int *current_streak, int *best_streak);

#define VIEW_SUCCESS_RATE 1
#define VIEW_DELIVERY_TIME 2
#define VIEW_PERFORMANCE_TRENDS 3
#define VIEW_PERFORMANCE_RANKING 4

// Performance grading thresholds
#define GRADE_A_SUCCESS_RATE 95.0
#define GRADE_A_ON_TIME_RATE 90.0
#define GRADE_B_SUCCESS_RATE 85.0
#define GRADE_B_ON_TIME_RATE 80.0
#define GRADE_C_SUCCESS_RATE 75.0
#define GRADE_C_ON_TIME_RATE 70.0

int performance_statistics(void) {
    int choice;
    while (1) {
        printf("\n----- PERFORMANCE STATISTICS -----\n");
        printf("1. View delivery success rate\n");
        printf("2. View average delivery time\n");
        printf("3. View performance trends\n");
        printf("4. View performance ranking\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_SUCCESS_RATE:
                view_delivery_success_rate();
                break;
            case VIEW_DELIVERY_TIME:
                view_average_delivery_time();
                break;
            case VIEW_PERFORMANCE_TRENDS:
                view_performance_trends();
                break;
            case VIEW_PERFORMANCE_RANKING:
                view_performance_ranking();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to calculate delivery time in minutes
int calculate_delivery_time_minutes(const char *order_time, const char *delivered_time) {
    // Parse time strings (HH:MM:SS format)
    int order_hour, order_min, order_sec;
    int delivered_hour, delivered_min, delivered_sec;
    
    sscanf(order_time, "%d:%d:%d", &order_hour, &order_min, &order_sec);
    sscanf(delivered_time, "%d:%d:%d", &delivered_hour, &delivered_min, &delivered_sec);
    
    // Convert to total minutes
    int order_total_minutes = order_hour * 60 + order_min;
    int delivered_total_minutes = delivered_hour * 60 + delivered_min;
    
    // Calculate difference (assuming same day delivery)
    int delivery_time = delivered_total_minutes - order_total_minutes;
    
    // Handle negative values (next day delivery)
    if (delivery_time < 0) {
        delivery_time += 24 * 60; // Add 24 hours
    }
    
    return delivery_time;
}

// Function to calculate performance grade
char calculate_performance_grade(float success_rate, float on_time_rate) {
    if (success_rate >= GRADE_A_SUCCESS_RATE && on_time_rate >= GRADE_A_ON_TIME_RATE) {
        return 'A';
    } else if (success_rate >= GRADE_B_SUCCESS_RATE && on_time_rate >= GRADE_B_ON_TIME_RATE) {
        return 'B';
    } else if (success_rate >= GRADE_C_SUCCESS_RATE && on_time_rate >= GRADE_C_ON_TIME_RATE) {
        return 'C';
    } else {
        return 'D';
    }
}

// Function to find performance streaks
int find_performance_streak(char *delivery_personnel_id, int *current_streak, int *best_streak) {
    DeliveryTimeRecord records[100];
    int count;
    
    *current_streak = 0;
    *best_streak = 0;
    
    if (get_delivery_time_records(delivery_personnel_id, records, &count) == FILE_ERROR) {
        return FILE_ERROR;
    }
    
    int temp_streak = 0;
    
    // Calculate streaks (consecutive on-time deliveries)
    for (int i = count - 1; i >= 0; i--) { // Start from most recent
        if (records[i].punctuality == ON_TIME) {
            temp_streak++;
            if (i == count - 1) { // Most recent delivery
                *current_streak = temp_streak;
            }
        } else {
            if (temp_streak > *best_streak) {
                *best_streak = temp_streak;
            }
            if (i == count - 1) { // Most recent delivery was late
                *current_streak = 0;
            }
            temp_streak = 0;
        }
    }
    
    // Check if the entire record is a streak
    if (temp_streak > *best_streak) {
        *best_streak = temp_streak;
    }
    
    return 1;
}

// Function to get delivery time records
int get_delivery_time_records(char *delivery_personnel_id, DeliveryTimeRecord *records, int *count) {
    FILE *delivery_fp = fopen(DELIVERY_FILE, "r");
    if (delivery_fp == NULL) {
        printf("Error: Could not open '%s' file.\n", DELIVERY_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    
    // Get delivery records for this personnel
    while (fgets(line, sizeof(line), delivery_fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';
        
        char delivery_id[DELIVERY_ID_LENGTH], order_id[ORDER_ID_LENGTH];
        char delivery_date[DATE_LENGTH], eta[TIME_LENGTH], delivered_time[TIME_LENGTH];
        int delivery_status, punctuality_status;
        
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
               delivery_id, order_id, delivery_date, eta, delivered_time,
               &delivery_status, &punctuality_status);
        
        // Only process delivered orders
        if (delivery_status != DELIVERED || strlen(delivered_time) == 0) {
            continue;
        }
        
        // Check if this delivery belongs to current personnel
        FILE *personnel_fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
        if (personnel_fp != NULL) {
            char personnel_line[MAX_LINE_LENGTH];
            while (fgets(personnel_line, sizeof(personnel_line), personnel_fp)) {
                char pid[DELIVERY_PERSONNEL_ID_LENGTH];
                sscanf(personnel_line, "%[^,]", pid);
                
                if (strcmp(pid, delivery_personnel_id) == 0) {
                    // Get order time for delivery time calculation
                    FILE *order_fp = fopen(ORDER_FILE, "r");
                    if (order_fp != NULL) {
                        char order_line[MAX_LINE_LENGTH];
                        while (fgets(order_line, sizeof(order_line), order_fp)) {
                            char oid[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
                            char restaurant_id[RESTAURANT_ID_LENGTH], order_status[20];
                            float total_price;
                            char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
                            
                            sscanf(order_line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                                   oid, student_id, restaurant_id, &total_price,
                                   order_date, order_time, order_status);
                            
                            if (strcmp(oid, order_id) == 0) {
                                strcpy(records[*count].delivery_id, delivery_id);
                                strcpy(records[*count].order_id, order_id);
                                strcpy(records[*count].delivery_date, delivery_date);
                                strcpy(records[*count].order_time, order_time);
                                strcpy(records[*count].delivered_time, delivered_time);
                                records[*count].delivery_minutes = calculate_delivery_time_minutes(order_time, delivered_time);
                                records[*count].punctuality = (PunctualityStatus)punctuality_status;
                                records[*count].earnings = calculate_base_fee(total_price);
                                
                                if (records[*count].punctuality == ON_TIME) {
                                    records[*count].earnings += calculate_bonus(ON_TIME, records[*count].earnings);
                                } else {
                                    records[*count].earnings -= calculate_penalty(LATE, records[*count].earnings);
                                }
                                
                                (*count)++;
                                break;
                            }
                        }
                        fclose(order_fp);
                    }
                    break;
                }
            }
            fclose(personnel_fp);
        }
    }
    
    fclose(delivery_fp);
    return *count;
}

// Function to calculate performance metrics
int calculate_performance_metrics(char *delivery_personnel_id, PerformanceMetrics *metrics) {
    DeliveryTimeRecord records[100];
    int count;
    
    if (get_delivery_time_records(delivery_personnel_id, records, &count) == FILE_ERROR) {
        return FILE_ERROR;
    }
    
    // Initialize metrics
    metrics->total_deliveries = count;
    metrics->successful_deliveries = count; // All completed deliveries are successful
    metrics->on_time_deliveries = 0;
    metrics->late_deliveries = 0;
    metrics->total_earnings = 0.0;
    
    int total_delivery_time = 0;
    int best_day_count = 0;
    strcpy(metrics->best_performance_date, "N/A");
    
    // Calculate metrics
    for (int i = 0; i < count; i++) {
        if (records[i].punctuality == ON_TIME) {
            metrics->on_time_deliveries++;
        } else {
            metrics->late_deliveries++;
        }
        
        total_delivery_time += records[i].delivery_minutes;
        metrics->total_earnings += records[i].earnings;
        
        // Find best performance date (most deliveries in a day)
        int day_count = 1;
        for (int j = i + 1; j < count; j++) {
            if (strcmp(records[i].delivery_date, records[j].delivery_date) == 0) {
                day_count++;
            }
        }
        
        if (day_count > best_day_count) {
            best_day_count = day_count;
            strcpy(metrics->best_performance_date, records[i].delivery_date);
        }
    }
    
    // Calculate rates and averages
    metrics->success_rate = (count > 0) ? 100.0 : 0.0; // All completed deliveries are successful
    metrics->on_time_rate = (count > 0) ? ((float)metrics->on_time_deliveries / count * 100.0) : 0.0;
    metrics->average_delivery_time = (count > 0) ? ((float)total_delivery_time / count) : 0.0;
    metrics->average_earnings_per_delivery = (count > 0) ? (metrics->total_earnings / count) : 0.0;
    metrics->best_day_deliveries = best_day_count;
    metrics->performance_grade = calculate_performance_grade(metrics->success_rate, metrics->on_time_rate);
    
    // Calculate streaks
    find_performance_streak(delivery_personnel_id, &metrics->current_streak, &metrics->best_streak);
    
    return 1;
}

// Function to display performance overview
void display_performance_overview(PerformanceMetrics *metrics) {
    printf("\n==================== PERFORMANCE OVERVIEW ====================\n");
    printf("Overall Performance Grade: %c\n", metrics->performance_grade);
    printf("Total Deliveries Completed: %d\n", metrics->total_deliveries);
    printf("Delivery Success Rate: %.1f%%\n", metrics->success_rate);
    printf("On-Time Delivery Rate: %.1f%%\n", metrics->on_time_rate);
    printf("Average Delivery Time: %.1f minutes\n", metrics->average_delivery_time);
    
    printf("\n--- DELIVERY BREAKDOWN ---\n");
    printf("On-Time Deliveries: %d\n", metrics->on_time_deliveries);
    printf("Late Deliveries: %d\n", metrics->late_deliveries);
    printf("Current On-Time Streak: %d deliveries\n", metrics->current_streak);
    printf("Best On-Time Streak: %d deliveries\n", metrics->best_streak);
    
    printf("\n--- PERFORMANCE HIGHLIGHTS ---\n");
    printf("Best Performance Date: %s (%d deliveries)\n", metrics->best_performance_date, metrics->best_day_deliveries);
    printf("Total Earnings: RM%.2f\n", metrics->total_earnings);
    printf("Average Earnings per Delivery: RM%.2f\n", metrics->average_earnings_per_delivery);
    
    printf("\n--- PERFORMANCE ANALYSIS ---\n");
    if (metrics->performance_grade == 'A') {
        printf("Excellent performance! You're a top performer.\n");
    } else if (metrics->performance_grade == 'B') {
        printf("Good performance! Keep up the good work.\n");
    } else if (metrics->performance_grade == 'C') {
        printf("Average performance. There's room for improvement.\n");
    } else {
        printf("Performance needs improvement. Focus on punctuality.\n");
    }
    
    if (metrics->on_time_rate < 80.0) {
        printf("Tip: Improve punctuality to increase earnings and performance grade.\n");
    }
    
    if (metrics->average_delivery_time > 45.0) {
        printf("Tip: Try to reduce average delivery time for better efficiency.\n");
    }
    
    printf("============================================================\n");
}

// Function to view delivery success rate
void view_delivery_success_rate(void) {
    PerformanceMetrics metrics;
    
    printf("\n----- DELIVERY SUCCESS RATE -----\n");
    
    if (calculate_performance_metrics(current_delivery_personnel_id, &metrics) == FILE_ERROR) return;
    
    if (metrics.total_deliveries == 0) {
        printf("No delivery data available.\n");
        printf("Complete some deliveries to see your success rate.\n");
        return;
    }
    
    printf("================= SUCCESS RATE ANALYSIS =================\n");
    printf("Total Deliveries Attempted: %d\n", metrics.total_deliveries);
    printf("Successful Deliveries: %d\n", metrics.successful_deliveries);
    printf("Success Rate: %.1f%%\n", metrics.success_rate);
    
    printf("\n--- PUNCTUALITY BREAKDOWN ---\n");
    printf("On-Time Deliveries: %d (%.1f%%)\n", metrics.on_time_deliveries, metrics.on_time_rate);
    printf("Late Deliveries: %d (%.1f%%)\n", metrics.late_deliveries, 100.0 - metrics.on_time_rate);
    
    printf("\n--- STREAK INFORMATION ---\n");
    printf("Current On-Time Streak: %d deliveries\n", metrics.current_streak);
    printf("Best On-Time Streak: %d deliveries\n", metrics.best_streak);
    
    printf("\n--- PERFORMANCE INSIGHTS ---\n");
    if (metrics.on_time_rate >= 90.0) {
        printf("Outstanding punctuality! You're consistently on time.\n");
    } else if (metrics.on_time_rate >= 80.0) {
        printf("Good punctuality. Keep maintaining this standard.\n");
    } else if (metrics.on_time_rate >= 70.0) {
        printf("Average punctuality. Try to improve on-time deliveries.\n");
    } else {
        printf("Punctuality needs improvement. Focus on time management.\n");
    }
    
    printf("=======================================================\n");
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("\nInvalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to display delivery time analysis
void display_delivery_time_analysis(DeliveryTimeRecord *records, int count) {
    if (count == 0) {
        printf("No delivery time data available.\n");
        return;
    }
    
    printf("\n======================== DELIVERY TIME ANALYSIS ========================\n");
    printf("%-8s %-8s %-12s %-10s %-10s %-8s %-10s\n", 
           "Del. ID", "Order ID", "Date", "Order", "Delivered", "Minutes", "Status");
    printf("------------------------------------------------------------------------\n");
    
    int total_time = 0;
    int fastest_time = records[0].delivery_minutes;
    int slowest_time = records[0].delivery_minutes;
    
    for (int i = 0; i < count && i < 10; i++) { // Show last 10 deliveries
        const char *status = (records[i].punctuality == ON_TIME) ? "On Time" : "Late";
        
        printf("%-8s %-8s %-12s %-10s %-10s %-8d %-10s\n", 
               records[i].delivery_id, records[i].order_id, records[i].delivery_date,
               records[i].order_time, records[i].delivered_time, 
               records[i].delivery_minutes, status);
        
        total_time += records[i].delivery_minutes;
        if (records[i].delivery_minutes < fastest_time) fastest_time = records[i].delivery_minutes;
        if (records[i].delivery_minutes > slowest_time) slowest_time = records[i].delivery_minutes;
    }
    
    printf("------------------------------------------------------------------------\n");
    printf("Average Delivery Time: %.1f minutes\n", (float)total_time / count);
    printf("Fastest Delivery: %d minutes\n", fastest_time);
    printf("Slowest Delivery: %d minutes\n", slowest_time);
    printf("========================================================================\n");
}

// Function to view average delivery time
void view_average_delivery_time(void) {
    DeliveryTimeRecord records[100];
    int count;
    
    printf("\n----- AVERAGE DELIVERY TIME -----\n");
    
    if (get_delivery_time_records(current_delivery_personnel_id, records, &count) == FILE_ERROR) return;
    
    if (count == 0) {
        printf("No delivery time data available.\n");
        printf("Complete some deliveries to see your delivery time statistics.\n");
        return;
    }
    
    display_delivery_time_analysis(records, count);
    
    // Additional time-based insights
    printf("\n--- TIME PERFORMANCE INSIGHTS ---\n");
    float avg_time = 0;
    for (int i = 0; i < count; i++) {
        avg_time += records[i].delivery_minutes;
    }
    avg_time /= count;
    
    if (avg_time <= 30.0) {
        printf("Excellent delivery speed! You're very efficient.\n");
    } else if (avg_time <= 45.0) {
        printf("Good delivery speed. Keep up the good work.\n");
    } else if (avg_time <= 60.0) {
        printf("Average delivery speed. Consider optimizing your routes.\n");
    } else {
        printf("Delivery speed needs improvement. Focus on efficiency.\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("\nInvalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view performance trends
void view_performance_trends(void) {
    PerformanceMetrics metrics;
    
    printf("\n----- PERFORMANCE TRENDS -----\n");
    
    if (calculate_performance_metrics(current_delivery_personnel_id, &metrics) == FILE_ERROR) return;
    
    if (metrics.total_deliveries == 0) {
        printf("No performance data available.\n");
        printf("Complete some deliveries to see your performance trends.\n");
        return;
    }
    
    display_performance_overview(&metrics);
    
    // Additional trend analysis
    printf("\n--- TREND ANALYSIS ---\n");
    printf("Performance Grade: %c\n", metrics.performance_grade);
    
    if (metrics.current_streak > 0) {
        printf("Current Trend: Positive (On-time streak: %d)\n", metrics.current_streak);
    } else {
        printf("Current Trend: Needs attention (Recent late delivery)\n");
    }
    
    printf("Improvement Areas:\n");
    if (metrics.on_time_rate < 90.0) {
        printf("- Focus on punctuality (current: %.1f%%)\n", metrics.on_time_rate);
    }
    if (metrics.average_delivery_time > 45.0) {
        printf("- Reduce delivery time (current: %.1f minutes)\n", metrics.average_delivery_time);
    }
    if (metrics.performance_grade < 'B') {
        printf("- Overall performance improvement needed\n");
    }
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("\nInvalid input! Please enter 0 to exit.\n");
        }
    }
}

// Function to view performance ranking
void view_performance_ranking(void) {
    PerformanceMetrics metrics;
    
    printf("\n----- PERFORMANCE RANKING -----\n");
    
    if (calculate_performance_metrics(current_delivery_personnel_id, &metrics) == FILE_ERROR) return;
    
    if (metrics.total_deliveries == 0) {
        printf("No performance data available for ranking.\n");
        printf("Complete some deliveries to see your performance ranking.\n");
        return;
    }
    
    printf("============== YOUR PERFORMANCE RANKING ==============\n");
    printf("Delivery Personnel ID: %s\n", current_delivery_personnel_id);
    printf("Performance Grade: %c\n", metrics.performance_grade);
    printf("Total Deliveries: %d\n", metrics.total_deliveries);
    printf("On-Time Rate: %.1f%%\n", metrics.on_time_rate);
    printf("Average Delivery Time: %.1f minutes\n", metrics.average_delivery_time);
    printf("Total Earnings: RM%.2f\n", metrics.total_earnings);
    
    printf("\n--- RANKING ANALYSIS ---\n");
    printf("Based on your performance metrics:\n");
    
    if (metrics.performance_grade == 'A') {
        printf("TOP PERFORMER - You're in the top tier!\n");
        printf("Ranking: Excellent (Top 10%%)\n");
    } else if (metrics.performance_grade == 'B') {
        printf("HIGH PERFORMER - Great job!\n");
        printf("Ranking: Above Average (Top 25%%)\n");
    } else if (metrics.performance_grade == 'C') {
        printf("AVERAGE PERFORMER - Room for improvement\n");
        printf("Ranking: Average (Top 50%%)\n");
    } else {
        printf("DEVELOPING PERFORMER - Focus on improvement\n");
        printf("Ranking: Below Average (Bottom 50%%)\n");
    }
    
    printf("\n--- PERFORMANCE BENCHMARKS ---\n");
    printf("To reach Grade A: %.1f%% on-time rate needed\n", GRADE_A_ON_TIME_RATE);
    printf("To reach Grade B: %.1f%% on-time rate needed\n", GRADE_B_ON_TIME_RATE);
    printf("To reach Grade C: %.1f%% on-time rate needed\n", GRADE_C_ON_TIME_RATE);
    
    printf("\n--- NEXT LEVEL REQUIREMENTS ---\n");
    if (metrics.performance_grade == 'D') {
        printf("To reach Grade C: Improve on-time rate to %.1f%%\n", GRADE_C_ON_TIME_RATE);
    } else if (metrics.performance_grade == 'C') {
        printf("To reach Grade B: Improve on-time rate to %.1f%%\n", GRADE_B_ON_TIME_RATE);
    } else if (metrics.performance_grade == 'B') {
        printf("To reach Grade A: Improve on-time rate to %.1f%%\n", GRADE_A_ON_TIME_RATE);
    } else {
        printf("Congratulations! You've achieved the highest grade!\n");
    }
    
    printf("====================================================\n");
    
    int exit_choice;
    while (1) {
        exit_choice = get_integer_input("\nEnter 0 to exit: ");
        if (exit_choice == 0) {
            break;
        } else {
            printf("\nInvalid input! Please enter 0 to exit.\n");
        }
    }
}
