#include "restaurant.h"

#define VIEW_MENU_ITEM 1
#define ADD_MENU_ITEM 2
#define UPDATE_MENU_ITEM 3
#define DELETE_MENU_ITEM 4

// Forward declarations
int find_menu_item(char *restaurant_id, MenuItem *items, int *count);
void view_menu_item(char *restaurant_id);
int add_menu_item(char *restaurant_id);
int update_menu_item(char *restaurant_id);
int delete_menu_item(char *restaurant_id);
void input_menu_item_name(char *name, int name_size);
int input_menu_item_price(float *price);
void get_menu_item_id(char *id, int id_size, char *restaurant_id);
int write_menu_item_file(const MenuItem *m);
int update_menu_item_in_file(const MenuItem *m);
int delete_menu_item_from_file(const char *item_id, const char *item_name);
void display_menu_items(MenuItem *items, int count);
int get_menu_item_by_number(MenuItem *items, int count, int item_number, MenuItem *selected_item);

extern char current_restaurant_id[RESTAURANT_ID_LENGTH];

int menu_item_management(void) {
    int choice;
    while (1) {
        char prompt[] = "\n----- MENU ITEM MANAGEMENT -----\n"
                        "1. View items\n"
                        "2. Add item\n"
                        "3. Update item\n"
                        "4. Delete item\n"
                        "0. Exit\n"
                        "What would you like to do?: ";

        choice = get_integer_input(prompt);
        switch (choice) {
            case VIEW_MENU_ITEM:
                view_menu_item(current_restaurant_id);
                break;
            case ADD_MENU_ITEM:
                add_menu_item(current_restaurant_id);
                break;
            case UPDATE_MENU_ITEM:
                update_menu_item(current_restaurant_id);
                break;
            case DELETE_MENU_ITEM:
                delete_menu_item(current_restaurant_id);
                break;
            case EXIT:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

int find_menu_item(char *restaurant_id, MenuItem *items, int *count) {
    FILE *fp = fopen(MENU_ITEM_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", MENU_ITEM_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';

        char *token;
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy));

        token = strtok(line_copy, ",");
        if (!token) continue;
        strncpy(items[*count].restaurant_id, token, RESTAURANT_ID_LENGTH);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(items[*count].id, token, MENU_ITEM_ID_LENGTH);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(items[*count].name, token, NAME_LENGTH);

        token = strtok(NULL, ",");
        if (!token) continue;
        items[*count].price = atof(token);

        if (strcmp(restaurant_id, items[*count].restaurant_id) == 0) {
            (*count)++;
        }
    }

    fclose(fp);
    return *count;
}

void display_menu_items(MenuItem *items, int count) {
    if (count == 0) {
        printf("No menu items found.\n");
        return;
    }

    printf("\n-------------------- MENU ITEMS --------------------\n");
    printf("%-5s %-10s %-20s %s\n", "No.", "ID", "Name", "Price (RM)");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-5d %-10s %-20s %.2f\n", i + 1, items[i].id, items[i].name, items[i].price);
    }
    printf("----------------------------------------------------\n");
}

void view_menu_item(char *restaurant_id) {
    MenuItem items[100];
    int count;
    printf("\n----- VIEW MENU ITEMS -----\n");

    if (find_menu_item(restaurant_id, items, &count) == FILE_ERROR) return;

    display_menu_items(items, count);
    printf("Total menu items: %d\n", count);
    
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

void input_menu_item_name(char *name, int name_size) {
    while (1) {
        printf("Enter new item's name: ");
        if (fgets(name, name_size, stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            clear_input_buffer();
            continue;
        }
        name[strcspn(name, "\n")] = '\0';
        if (strlen(name) > 0) break;
        printf("Name cannot be empty. Please try again.\n");
    }
}

int input_menu_item_price(float *price) {
    char input[20];
    while (1) {
        printf("Enter new item's price (RM): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            clear_input_buffer();
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        char *endptr;
        *price = strtof(input, &endptr);
        if (endptr != input && *endptr == '\0' && *price > 0) return 1;
        printf("Invalid input! Please enter a valid price (e.g., 12.50).\n");
    }
}

void get_menu_item_id(char *id, int id_size, char *restaurant_id) {
    FILE *fp = fopen(MENU_ITEM_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "M001");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int max_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy));
        
        char *rest_id_token = strtok(line_copy, ",");
        if (!rest_id_token) continue;
        
        // Only check menu items for the current restaurant
        if (strcmp(rest_id_token, restaurant_id) == 0) {
            char *menu_id_token = strtok(NULL, ",");
            if (menu_id_token && menu_id_token[0] == 'M') {
                int num = atoi(&menu_id_token[1]);
                if (num > max_num) max_num = num;
            }
        }
    }
    fclose(fp);
    snprintf(id, id_size, "M%03d", max_num + 1);
}

int write_menu_item_file(const MenuItem *m) {
    FILE *fp = fopen(MENU_ITEM_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", MENU_ITEM_FILE);
        return FILE_ERROR;
    }
    
    fprintf(fp, "%s,%s,%s,%.2f\n", m->restaurant_id, m->id, m->name, m->price);
    fclose(fp);
    printf("\nMenu item '%s' added successfully!\n", m->name);
    return 1;
}

int add_menu_item(char *restaurant_id) {
    MenuItem new_item;
    MenuItem items[100];
    int count;

    printf("\n----- ADD MENU ITEM -----\n");

    if (find_menu_item(restaurant_id, items, &count) == FILE_ERROR) return 0;
    display_menu_items(items, count);

    strcpy(new_item.restaurant_id, restaurant_id);
    input_menu_item_name(new_item.name, sizeof(new_item.name));
    if (!input_menu_item_price(&new_item.price)) return 0;
    printf("\n--- NEW MENU ITEM INFORMATION ---\n");
    printf("Item Name: %s\n", new_item.name);
    printf("Item Price: RM%.2f\n", new_item.price);
    int confirm = confirmation("Are you sure you want to add this item? (1. Yes / 2. No): ");
    if (confirm == YES) {
        get_menu_item_id(new_item.id, sizeof(new_item.id), restaurant_id);
        return write_menu_item_file(&new_item);
    }
    printf("Menu item addition cancelled.\n");
    return 0;
}

// Get menu item by selection number
int get_menu_item_by_number(MenuItem *items, int count, int item_number, MenuItem *selected_item) {
    if (item_number < 1 || item_number > count) {
        return 0;
    }
    
    *selected_item = items[item_number - 1];
    return 1;
}

// Update menu item in file
int update_menu_item_in_file(const MenuItem *updated_item) {
    FILE *fp = fopen(MENU_ITEM_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", MENU_ITEM_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_menu.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        MenuItem temp_item;
        char *token = strtok(line, ",");
        if (token == NULL) continue;
        strncpy(temp_item.restaurant_id, token, RESTAURANT_ID_LENGTH);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(temp_item.id, token, MENU_ITEM_ID_LENGTH);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(temp_item.name, token, NAME_LENGTH);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        temp_item.price = atof(token);

        if (strcmp(temp_item.id, updated_item->id) == 0) {
            fprintf(temp_fp, "%s,%s,%s,%.2f\n",
                    updated_item->restaurant_id,
                    updated_item->id,
                    updated_item->name,
                    updated_item->price);
            updated = 1;
        } else {
            fprintf(temp_fp, "%s,%s,%s,%.2f\n",
                    temp_item.restaurant_id,
                    temp_item.id,
                    temp_item.name,
                    temp_item.price);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (updated) {
        remove(MENU_ITEM_FILE);
        rename("temp_menu.txt", MENU_ITEM_FILE);
        printf("Menu item '%s' has been updated successfully!\n", updated_item->name);
        return 1;
    } else {
        remove("temp_menu.txt");
        printf("Menu item not found for update.\n");
        return 0;
    }
}

// Update menu item
int update_menu_item(char *restaurant_id) {
    MenuItem items[100];
    int count;
    
    printf("\n----- UPDATE MENU ITEM -----\n");
    
    int result = find_menu_item(restaurant_id, items, &count);
    if (result == FILE_ERROR) {
        return 0;
    }
    
    if (count == 0) {
        printf("No menu items found to update.\n");
        return 0;
    }
    
    display_menu_items(items, count);
    
    int update_choice;
    while (1) {
        update_choice = get_integer_input("Select item number to update (enter '0' to exit): ");
        if (update_choice >= 0 && update_choice <= count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", count);
    }
    
    if (update_choice == 0) {
        return 0;
    }
    
    MenuItem selected_item;
    get_menu_item_by_number(items, count, update_choice, &selected_item);
    
    printf("\nCurrent item details:\n");
    printf("Name: %s\n", selected_item.name);
    printf("Price: RM%.2f\n", selected_item.price);
    
    MenuItem updated_item = selected_item;
    
    printf("\nEnter new details:\n");
    input_menu_item_name(updated_item.name, sizeof(updated_item.name));
    if (!input_menu_item_price(&updated_item.price)) {
        return 0;
    }
    
    printf("\n--- UPDATED MENU ITEM INFORMATION ---\n");
    printf("Item Name: %s\n", updated_item.name);
    printf("Item Price: RM%.2f\n", updated_item.price);
    
    int confirm = confirmation("Are you sure you want to update this item? (1. Yes / 2. No): ");
    if (confirm == 1) {
        return update_menu_item_in_file(&updated_item);
    }
    else {
        printf("Menu item update cancelled.\n");
        return 0;
    }
}

// Delete menu item from file
int delete_menu_item_from_file(const char *item_id, const char *item_name) {
    FILE *fp = fopen(MENU_ITEM_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", MENU_ITEM_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_menu.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int deleted = 0;

    while (fgets(line, sizeof(line), fp)) {
        MenuItem temp_item;
        char *token = strtok(line, ",");
        if (token == NULL) continue;
        strncpy(temp_item.restaurant_id, token, RESTAURANT_ID_LENGTH);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(temp_item.id, token, MENU_ITEM_ID_LENGTH);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strncpy(temp_item.name, token, NAME_LENGTH);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        temp_item.price = atof(token);

        if (strcmp(temp_item.id, item_id) == 0) {
            deleted = 1;
            continue; // skip this one
        } else {
            fprintf(temp_fp, "%s,%s,%s,%.2f\n",
                    temp_item.restaurant_id,
                    temp_item.id,
                    temp_item.name,
                    temp_item.price);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (deleted) {
        remove(MENU_ITEM_FILE);
        rename("temp_menu.txt", MENU_ITEM_FILE);
        printf("Menu item '%s' has been deleted successfully!\n", item_name);
        return 1;
    } else {
        remove("temp_menu.txt");
        printf("Menu item not found for deletion.\n");
        return 0;
    }
}

// Delete menu item
int delete_menu_item(char *restaurant_id) {
    MenuItem items[100];
    int count;
    
    printf("\n----- DELETE MENU ITEM -----\n");
    
    int result = find_menu_item(restaurant_id, items, &count);
    if (result == FILE_ERROR) {
        return 0;
    }
    
    if (count == 0) {
        printf("No menu items found to delete.\n");
        return 0;
    }
    
    display_menu_items(items, count);
    
    int delete_choice;
    while (1) {
        delete_choice = get_integer_input("Select item number to delete (enter '0' to exit): ");
        if (delete_choice >= 0 && delete_choice <= count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", count);
    }
    
    if (delete_choice == 0) {
        return 0;
    }
    
    MenuItem selected_item;
    get_menu_item_by_number(items, count, delete_choice, &selected_item);
    
    printf("\nItem to delete:\n");
    printf("Name: %s\n", selected_item.name);
    printf("Price: RM%.2f\n", selected_item.price);
    
    int confirm = confirmation("Are you sure you want to delete this item? (1. Yes / 2. No): ");
    if (confirm == 1) {
        return delete_menu_item_from_file(selected_item.id, selected_item.name);
    }
    else {
        printf("Menu item deletion cancelled.\n");
        return 0;
    }
}


// --------------------------------------------------------------------------------------------

// Forward declarations for order queue management
int find_restaurant_orders(char *restaurant_id, QueueOrder *orders, int *count);
void display_order_queue(QueueOrder *orders, int count);
void view_pending_orders(char *restaurant_id);
int update_order_status(char *restaurant_id);
void view_order_details(char *order_id);
int get_order_by_number(QueueOrder *orders, int count, int order_number, QueueOrder *selected_order);
int update_order_status_in_file(const char *order_id, const char *new_status);
void get_order_items_summary(char *order_id, char *summary, int summary_size);
const char* get_status_display_name(const char *status);
int count_orders_by_status(QueueOrder *orders, int count, const char *status);
void view_order_statistics(char *restaurant_id);

#define VIEW_PENDING_ORDERS 1
#define UPDATE_ORDER_STATUS 2
#define VIEW_ORDER_DETAILS 3
#define ORDER_STATISTICS 4

int order_queue_management(void) {
    int choice;
    while (1) {
        printf("\n----- ORDER QUEUE MANAGEMENT -----\n");
        printf("1. View pending orders\n");
        printf("2. Update order status\n");
        printf("3. View order details\n");
        printf("4. Order statistics\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_PENDING_ORDERS:
                view_pending_orders(current_restaurant_id);
                break;
            case UPDATE_ORDER_STATUS:
                update_order_status(current_restaurant_id);
                break;
            case VIEW_ORDER_DETAILS: {
                char order_id[ORDER_ID_LENGTH];
                printf("Enter Order ID: ");
                fgets(order_id, sizeof(order_id), stdin);
                order_id[strcspn(order_id, "\n")] = '\0';
                view_order_details(order_id);
                break;
            }
            case ORDER_STATISTICS:
                view_order_statistics(current_restaurant_id);
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to find orders for a specific restaurant
int find_restaurant_orders(char *restaurant_id, QueueOrder *orders, int *count) {
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
        char rest_id[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];

        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, rest_id, &total_price,
               order_date, order_time, status);

        if (strcmp(restaurant_id, rest_id) == 0) {
            strcpy(orders[*count].order_id, order_id);
            strcpy(orders[*count].student_id, student_id);
            strcpy(orders[*count].restaurant_id, rest_id);
            orders[*count].total_price = total_price;
            strcpy(orders[*count].order_date, order_date);
            strcpy(orders[*count].order_time, order_time);
            strcpy(orders[*count].status, status);
            
            // Get items summary for this order
            get_order_items_summary(order_id, orders[*count].items_summary, sizeof(orders[*count].items_summary));
            
            (*count)++;
        }
    }

    fclose(fp);
    return *count;
}

// Function to get order items summary
void get_order_items_summary(char *order_id, char *summary, int summary_size) {
    FILE *fp = fopen(QUEUE_FILE, "r");
    if (fp == NULL) {
        strcpy(summary, "No items found");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char queue_order_id[ORDER_ID_LENGTH];
        sscanf(line, "%[^,]", queue_order_id);
        
        if (strcmp(order_id, queue_order_id) == 0) {
            // Parse the items from the queue line
            char *token = strtok(line, ",");
            token = strtok(NULL, ","); // Skip order_id
            
            strcpy(summary, "");
            int item_count = 0;
            
            while (token != NULL && item_count < 5) { // Limit to 5 items for display
                char menu_id[MENU_ITEM_ID_LENGTH];
                strcpy(menu_id, token);
                
                token = strtok(NULL, ",");
                if (token != NULL) {
                    int quantity = atoi(token);
                    
                    // Get menu item name
                    char item_name[NAME_LENGTH] = "Unknown";
                    FILE *menu_fp = fopen(MENU_ITEM_FILE, "r");
                    if (menu_fp != NULL) {
                        char menu_line[MAX_LINE_LENGTH];
                        while (fgets(menu_line, sizeof(menu_line), menu_fp)) {
                            char mid[MENU_ITEM_ID_LENGTH], name[NAME_LENGTH];
                            char rid[RESTAURANT_ID_LENGTH];
                            float price;
                            
                            sscanf(menu_line, "%[^,],%[^,],%[^,],%f", rid, mid, name, &price);
                            if (strcmp(mid, menu_id) == 0) {
                                strcpy(item_name, name);
                                break;
                            }
                        }
                        fclose(menu_fp);
                    }
                    
                    if (strlen(summary) > 0) {
                        strcat(summary, ", ");
                    }
                    
                    char item_info[100];
                    snprintf(item_info, sizeof(item_info), "%s x%d", item_name, quantity);
                    strcat(summary, item_info);
                    
                    item_count++;
                }
                token = strtok(NULL, ",");
            }
            
            if (strlen(summary) == 0) {
                strcpy(summary, "No items");
            }
            break;
        }
    }
    
    fclose(fp);
}

// Function to get status display name
const char* get_status_display_name(const char *status) {
    if (strcmp(status, "CONFIRMED") == 0) return "Confirmed";
    if (strcmp(status, "PREPARING") == 0) return "Preparing";
    if (strcmp(status, "READY") == 0) return "Ready";
    if (strcmp(status, "DELIVERED") == 0) return "Delivered";
    if (strcmp(status, "CANCELLED") == 0) return "Cancelled";
    return status;
}

// Function to display order queue
void display_order_queue(QueueOrder *orders, int count) {
    if (count == 0) {
        printf("No orders found.\n");
        return;
    }

    printf("\n================================== ORDER QUEUE ==================================\n");
    printf("%-4s %-8s %-10s %-8s %-12s %-10s %-30s\n", 
           "No.", "Order ID", "Student", "Total", "Time", "Status", "Items");
    printf("---------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-4d %-8s %-10s RM%-6.2f %-12s %-10s %-30s\n", 
               i + 1, orders[i].order_id, orders[i].student_id,
               orders[i].total_price, orders[i].order_time,
               get_status_display_name(orders[i].status), orders[i].items_summary);
    }
    printf("---------------------------------------------------------------------------------\n");
}

// Function to view pending orders
void view_pending_orders(char *restaurant_id) {
    QueueOrder orders[100];
    int count;
    
    printf("\n----- VIEW PENDING ORDERS -----\n");

    if (find_restaurant_orders(restaurant_id, orders, &count) == FILE_ERROR) return;

    // Filter to show only non-delivered orders
    QueueOrder pending_orders[100];
    int pending_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].status, "DELIVERED") != 0 && strcmp(orders[i].status, "CANCELLED") != 0) {
            pending_orders[pending_count] = orders[i];
            pending_count++;
        }
    }

    display_order_queue(pending_orders, pending_count);
    printf("Total pending orders: %d\n", pending_count);
    
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
int get_order_by_number(QueueOrder *orders, int count, int order_number, QueueOrder *selected_order) {
    if (order_number < 1 || order_number > count) {
        return 0;
    }
    
    *selected_order = orders[order_number - 1];
    return 1;
}

// Function to update order status in file
int update_order_status_in_file(const char *order_id, const char *new_status) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", ORDER_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_order.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        char oid[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];

        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               oid, student_id, restaurant_id, &total_price,
               order_date, order_time, status);

        if (strcmp(oid, order_id) == 0) {
            fprintf(temp_fp, "%s,%s,%s,%.2f,%s,%s,%s\n",
                    oid, student_id, restaurant_id, total_price,
                    order_date, order_time, new_status);
            updated = 1;
        } else {
            fprintf(temp_fp, "%s,%s,%s,%.2f,%s,%s,%s\n",
                    oid, student_id, restaurant_id, total_price,
                    order_date, order_time, status);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (updated) {
        remove(ORDER_FILE);
        rename("temp_order.txt", ORDER_FILE);
        printf("Order '%s' status updated to '%s' successfully!\n", order_id, new_status);
        return 1;
    } else {
        remove("temp_order.txt");
        printf("Order not found for status update.\n");
        return 0;
    }
}

// Function to update order status
int update_order_status(char *restaurant_id) {
    QueueOrder orders[100];
    int count;
    
    printf("\n----- UPDATE ORDER STATUS -----\n");
    
    if (find_restaurant_orders(restaurant_id, orders, &count) == FILE_ERROR) {
        return 0;
    }
    
    // Filter to show only non-delivered orders
    QueueOrder pending_orders[100];
    int pending_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].status, "DELIVERED") != 0 && strcmp(orders[i].status, "CANCELLED") != 0) {
            pending_orders[pending_count] = orders[i];
            pending_count++;
        }
    }
    
    if (pending_count == 0) {
        printf("No pending orders found to update.\n");
        return 0;
    }
    
    display_order_queue(pending_orders, pending_count);
    
    int update_choice;
    while (1) {
        update_choice = get_integer_input("Select order number to update (enter '0' to exit): ");
        if (update_choice >= 0 && update_choice <= pending_count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", pending_count);
    }
    
    if (update_choice == 0) {
        return 0;
    }
    
    QueueOrder selected_order;
    get_order_by_number(pending_orders, pending_count, update_choice, &selected_order);
    
    printf("\nSelected Order: %s\n", selected_order.order_id);
    printf("Current Status: %s\n", get_status_display_name(selected_order.status));
    printf("Items: %s\n", selected_order.items_summary);
    
    printf("\nAvailable Status Options:\n");
    printf("1. CONFIRMED - Order confirmed, waiting to prepare\n");
    printf("2. PREPARING - Order is being prepared\n");
    printf("3. READY - Order is ready for pickup/delivery\n");
    printf("4. DELIVERED - Order has been delivered\n");
    printf("5. CANCELLED - Order has been cancelled\n");
    
    int status_choice = get_integer_input("Select new status (1-5): ");
    
    const char *new_status;
    switch (status_choice) {
        case 1: new_status = "CONFIRMED"; break;
        case 2: new_status = "PREPARING"; break;
        case 3: new_status = "READY"; break;
        case 4: new_status = "DELIVERED"; break;
        case 5: new_status = "CANCELLED"; break;
        default:
            printf("Invalid status choice.\n");
            return 0;
    }
    
    int confirm = confirmation("Are you sure you want to update this order status? (1. Yes / 2. No): ");
    if (confirm == 1) {
        return update_order_status_in_file(selected_order.order_id, new_status);
    } else {
        printf("Order status update cancelled.\n");
        return 0;
    }
}

// Function to view detailed order information
void view_order_details(char *order_id) {
    printf("\n----- ORDER DETAILS -----\n");
    
    // Find order in Order.txt
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open order file.\n");
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    int found = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        char oid[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char restaurant_id[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];

        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               oid, student_id, restaurant_id, &total_price,
               order_date, order_time, status);
        
        if (strcmp(oid, order_id) == 0) {
            printf("Order ID: %s\n", oid);
            printf("Student ID: %s\n", student_id);
            printf("Restaurant ID: %s\n", restaurant_id);
            printf("Total Price: RM%.2f\n", total_price);
            printf("Order Date: %s\n", order_date);
            printf("Order Time: %s\n", order_time);
            printf("Status: %s\n", get_status_display_name(status));
            
            // Get detailed items
            char items_summary[200];
            get_order_items_summary(order_id, items_summary, sizeof(items_summary));
            printf("Items: %s\n", items_summary);
            
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("Order ID '%s' not found.\n", order_id);
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

// Function to count orders by status
int count_orders_by_status(QueueOrder *orders, int count, const char *status) {
    int status_count = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].status, status) == 0) {
            status_count++;
        }
    }
    return status_count;
}

// Function to view order statistics
void view_order_statistics(char *restaurant_id) {
    QueueOrder orders[100];
    int count;
    
    printf("\n----- ORDER STATISTICS -----\n");

    if (find_restaurant_orders(restaurant_id, orders, &count) == FILE_ERROR) return;

    if (count == 0) {
        printf("No orders found for statistics.\n");
        return;
    }

    printf("==================== ORDER STATISTICS ====================\n");
    printf("Total Orders: %d\n", count);
    printf("\nStatus Breakdown:\n");
    printf("- Confirmed: %d orders\n", count_orders_by_status(orders, count, "CONFIRMED"));
    printf("- Preparing: %d orders\n", count_orders_by_status(orders, count, "PREPARING"));
    printf("- Ready: %d orders\n", count_orders_by_status(orders, count, "READY"));
    printf("- Delivered: %d orders\n", count_orders_by_status(orders, count, "DELIVERED"));
    printf("- Cancelled: %d orders\n", count_orders_by_status(orders, count, "CANCELLED"));
    
    // Calculate total revenue
    float total_revenue = 0.0;
    float pending_revenue = 0.0;
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].status, "DELIVERED") == 0) {
            total_revenue += orders[i].total_price;
        } else if (strcmp(orders[i].status, "CANCELLED") != 0) {
            pending_revenue += orders[i].total_price;
        }
    }
    
    printf("\nRevenue Information:\n");
    printf("- Completed Revenue: RM%.2f\n", total_revenue);
    printf("- Pending Revenue: RM%.2f\n", pending_revenue);
    printf("- Total Potential Revenue: RM%.2f\n", total_revenue + pending_revenue);
    
    printf("=======================================================\n");
    
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

// Daily Sales Report Structures

// Forward declarations for daily sales report
int calculate_daily_sales(char *restaurant_id, char *date, DailySales *sales);
int get_item_sales_data(char *restaurant_id, char *date, ItemSales *items, int *count);
int get_hourly_sales_data(char *restaurant_id, char *date, HourlySales *hourly, int *count);
void display_daily_sales_summary(DailySales *sales);
void display_item_sales_analysis(ItemSales *items, int count);
void display_hourly_sales_trend(HourlySales *hourly, int count);
void view_daily_sales_summary(void);
void view_item_sales_analysis(void);
void view_hourly_sales_trend(void);
void view_sales_comparison(void);
void export_sales_report(void);
int parse_order_items(char *order_id, ItemSales *items, int *item_count);
void get_menu_item_name(char *restaurant_id, char *item_id, char *item_name);
float get_menu_item_price(char *restaurant_id, char *item_id);
int extract_hour_from_time(char *time_str);
void get_current_date_restaurant(char *date_str);

#define VIEW_DAILY_SUMMARY 1
#define VIEW_ITEM_ANALYSIS 2
#define VIEW_HOURLY_TREND 3
#define VIEW_SALES_COMPARISON 4
#define EXPORT_SALES_REPORT 5

int daily_sales_report(void) {
    int choice;
    
    while (1) {
        printf("\n----- DAILY SALES REPORT -----\n");
        printf("1. View daily sales summary\n");
        printf("2. View item sales analysis\n");
        printf("3. View hourly sales trend\n");
        printf("4. View sales comparison\n");
        printf("5. Export sales report\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_DAILY_SUMMARY:
                view_daily_sales_summary();
                break;
            case VIEW_ITEM_ANALYSIS:
                view_item_sales_analysis();
                break;
            case VIEW_HOURLY_TREND:
                view_hourly_sales_trend();
                break;
            case VIEW_SALES_COMPARISON:
                view_sales_comparison();
                break;
            case EXPORT_SALES_REPORT:
                export_sales_report();
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to get current date (simplified)
void get_current_date_restaurant(char *date_str) {
    // For demonstration, using a fixed current date
    // In a real system, this would get actual current date
    strcpy(date_str, "28-12-2024");
}

// Function to extract hour from time string
int extract_hour_from_time(char *time_str) {
    int hour;
    sscanf(time_str, "%d:", &hour);
    return hour;
}

// Function to get menu item name
void get_menu_item_name(char *restaurant_id, char *item_id, char *item_name) {
    FILE *fp = fopen(MENU_ITEM_FILE, "r");
    if (fp == NULL) {
        strcpy(item_name, "Unknown Item");
        return;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        char rid[RESTAURANT_ID_LENGTH], mid[MENU_ITEM_ID_LENGTH], name[NAME_LENGTH];
        float price;
        
        sscanf(line, "%[^,],%[^,],%[^,],%f", rid, mid, name, &price);
        
        if (strcmp(rid, restaurant_id) == 0 && strcmp(mid, item_id) == 0) {
            strcpy(item_name, name);
            fclose(fp);
            return;
        }
    }
    
    strcpy(item_name, "Unknown Item");
    fclose(fp);
}

// Function to get menu item price
float get_menu_item_price(char *restaurant_id, char *item_id) {
    FILE *fp = fopen(MENU_ITEM_FILE, "r");
    if (fp == NULL) {
        return 0.0;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        char rid[RESTAURANT_ID_LENGTH], mid[MENU_ITEM_ID_LENGTH], name[NAME_LENGTH];
        float price;
        
        sscanf(line, "%[^,],%[^,],%[^,],%f", rid, mid, name, &price);
        
        if (strcmp(rid, restaurant_id) == 0 && strcmp(mid, item_id) == 0) {
            fclose(fp);
            return price;
        }
    }
    
    fclose(fp);
    return 0.0;
}

// Function to calculate daily sales
int calculate_daily_sales(char *restaurant_id, char *date, DailySales *sales) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", ORDER_FILE);
        return FILE_ERROR;
    }
    
    // Initialize sales data
    strcpy(sales->date, date);
    sales->total_orders = 0;
    sales->completed_orders = 0;
    sales->pending_orders = 0;
    sales->cancelled_orders = 0;
    sales->total_revenue = 0.0;
    sales->completed_revenue = 0.0;
    sales->pending_revenue = 0.0;
    sales->average_order_value = 0.0;
    strcpy(sales->best_selling_item, "N/A");
    sales->best_selling_quantity = 0;
    sales->best_selling_revenue = 0.0;
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char rid[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
        
        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, rid, &total_price,
               order_date, order_time, status);
        
        // Check if order belongs to this restaurant and date
        if (strcmp(rid, restaurant_id) == 0 && strcmp(order_date, date) == 0) {
            sales->total_orders++;
            sales->total_revenue += total_price;
            
            if (strcmp(status, "DELIVERED") == 0) {
                sales->completed_orders++;
                sales->completed_revenue += total_price;
            } else if (strcmp(status, "CANCELLED") == 0) {
                sales->cancelled_orders++;
            } else {
                sales->pending_orders++;
                sales->pending_revenue += total_price;
            }
        }
    }
    
    fclose(fp);
    
    // Calculate average order value
    if (sales->total_orders > 0) {
        sales->average_order_value = sales->total_revenue / sales->total_orders;
    }
    
    return 1;
}

// Function to get item sales data
int get_item_sales_data(char *restaurant_id, char *date, ItemSales *items, int *count) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", ORDER_FILE);
        return FILE_ERROR;
    }
    
    *count = 0;
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char rid[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
        
        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, rid, &total_price,
               order_date, order_time, status);
        
        // Check if order belongs to this restaurant and date
        if (strcmp(rid, restaurant_id) == 0 && strcmp(order_date, date) == 0) {
            // For simplification, we'll create sample item sales data
            // In a real system, this would parse actual order items
            
            // Sample logic: assume each order has 1-3 random items
            if (*count < 50) { // Limit to prevent overflow
                // Create sample item sales entries
                strcpy(items[*count].item_id, "M001");
                get_menu_item_name(restaurant_id, "M001", items[*count].item_name);
                items[*count].quantity_sold = 1;
                items[*count].unit_price = get_menu_item_price(restaurant_id, "M001");
                items[*count].total_revenue = items[*count].unit_price;
                items[*count].order_count = 1;
                (*count)++;
            }
        }
    }
    
    fclose(fp);
    
    // Consolidate duplicate items
    for (int i = 0; i < *count - 1; i++) {
        for (int j = i + 1; j < *count; j++) {
            if (strcmp(items[i].item_id, items[j].item_id) == 0) {
                items[i].quantity_sold += items[j].quantity_sold;
                items[i].total_revenue += items[j].total_revenue;
                items[i].order_count += items[j].order_count;
                
                // Remove duplicate
                for (int k = j; k < *count - 1; k++) {
                    items[k] = items[k + 1];
                }
                (*count)--;
                j--;
            }
        }
    }
    
    return *count;
}

// Function to get hourly sales data
int get_hourly_sales_data(char *restaurant_id, char *date, HourlySales *hourly, int *count) {
    FILE *fp = fopen(ORDER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", ORDER_FILE);
        return FILE_ERROR;
    }
    
    // Initialize hourly data (24 hours)
    *count = 24;
    for (int i = 0; i < 24; i++) {
        hourly[i].hour = i;
        hourly[i].order_count = 0;
        hourly[i].revenue = 0.0;
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        
        char order_id[ORDER_ID_LENGTH], student_id[STUDENT_ID_LENGTH];
        char rid[RESTAURANT_ID_LENGTH], status[20];
        float total_price;
        char order_date[DATE_LENGTH], order_time[TIME_LENGTH];
        
        sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
               order_id, student_id, rid, &total_price,
               order_date, order_time, status);
        
        // Check if order belongs to this restaurant and date
        if (strcmp(rid, restaurant_id) == 0 && strcmp(order_date, date) == 0) {
            int hour = extract_hour_from_time(order_time);
            if (hour >= 0 && hour < 24) {
                hourly[hour].order_count++;
                hourly[hour].revenue += total_price;
            }
        }
    }
    
    fclose(fp);
    return *count;
}

// Function to display daily sales summary
void display_daily_sales_summary(DailySales *sales) {
    printf("\n==================== DAILY SALES SUMMARY ====================\n");
    printf("Date: %s\n", sales->date);
    printf("Restaurant ID: %s\n", current_restaurant_id);
    
    printf("\n--- ORDER STATISTICS ---\n");
    printf("Total Orders: %d\n", sales->total_orders);
    printf("Completed Orders: %d\n", sales->completed_orders);
    printf("Pending Orders: %d\n", sales->pending_orders);
    printf("Cancelled Orders: %d\n", sales->cancelled_orders);
    
    if (sales->total_orders > 0) {
        printf("Completion Rate: %.1f%%\n", (float)sales->completed_orders / sales->total_orders * 100);
        printf("Cancellation Rate: %.1f%%\n", (float)sales->cancelled_orders / sales->total_orders * 100);
    }
    
    printf("\n--- REVENUE ANALYSIS ---\n");
    printf("Total Revenue: RM%.2f\n", sales->total_revenue);
    printf("Completed Revenue: RM%.2f\n", sales->completed_revenue);
    printf("Pending Revenue: RM%.2f\n", sales->pending_revenue);
    printf("Average Order Value: RM%.2f\n", sales->average_order_value);
    
    printf("\n--- PERFORMANCE INSIGHTS ---\n");
    if (sales->completed_revenue > 500.0) {
        printf("Excellent daily performance! Revenue target exceeded.\n");
    } else if (sales->completed_revenue > 300.0) {
        printf("Good daily performance. Keep up the good work.\n");
    } else if (sales->completed_revenue > 100.0) {
        printf("Average performance. Consider promotional activities.\n");
    } else {
        printf("Below average performance. Review menu and pricing.\n");
    }
    
    if (sales->total_orders > 0) {
        float cancellation_rate = (float)sales->cancelled_orders / sales->total_orders * 100;
        if (cancellation_rate > 20.0) {
            printf("High cancellation rate detected. Review order processing.\n");
        } else if (cancellation_rate > 10.0) {
            printf("Moderate cancellation rate. Monitor order quality.\n");
        }
    }
    
    printf("=============================================================\n");
}

// Function to display item sales analysis
void display_item_sales_analysis(ItemSales *items, int count) {
    if (count == 0) {
        printf("No item sales data available for the selected date.\n");
        return;
    }
    
    printf("\n========================= ITEM SALES ANALYSIS =========================\n");
    printf("%-8s %-20s %-8s %-8s %-10s %-8s\n", 
           "Item ID", "Item Name", "Qty Sold", "Unit Price", "Revenue", "Orders");
    printf("-----------------------------------------------------------------------\n");
    
    // Sort items by revenue (descending)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (items[j].total_revenue > items[i].total_revenue) {
                ItemSales temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
    
    float total_revenue = 0.0;
    int total_quantity = 0;
    
    for (int i = 0; i < count; i++) {
        printf("%-8s %-20s %-8d RM%-6.2f RM%-8.2f %-8d\n", 
               items[i].item_id, items[i].item_name, items[i].quantity_sold,
               items[i].unit_price, items[i].total_revenue, items[i].order_count);
        
        total_revenue += items[i].total_revenue;
        total_quantity += items[i].quantity_sold;
    }
    
    printf("-----------------------------------------------------------------------\n");
    printf("TOTALS: %-28s %-8d %-8s RM%-8.2f\n", "", total_quantity, "", total_revenue);
    
    printf("\n--- TOP PERFORMERS ---\n");
    if (count > 0) {
        printf("Best Selling Item: %s (RM%.2f revenue)\n", items[0].item_name, items[0].total_revenue);
        printf("Most Popular Item: %s (%d units sold)\n", items[0].item_name, items[0].quantity_sold);
    }
    
    printf("\n--- RECOMMENDATIONS ---\n");
    if (count > 0) {
        printf("Focus on promoting: %s (top revenue generator)\n", items[0].item_name);
        if (count > 1) {
            printf("Consider improving: %s (lower performance)\n", items[count-1].item_name);
        }
    }
    
    printf("===============================================================\n");
}

// Function to display hourly sales trend
void display_hourly_sales_trend(HourlySales *hourly, int count) {
    printf("\n============== HOURLY SALES TREND ==============\n");
    printf("%-6s %-8s %-10s %-15s\n", "Hour", "Orders", "Revenue", "Trend");
    printf("------------------------------------------------\n");
    
    int peak_hour = 0;
    float peak_revenue = 0.0;
    int total_orders = 0;
    float total_revenue = 0.0;
    
    for (int i = 0; i < count; i++) {
        if (hourly[i].revenue > peak_revenue) {
            peak_revenue = hourly[i].revenue;
            peak_hour = i;
        }
        total_orders += hourly[i].order_count;
        total_revenue += hourly[i].revenue;
        
        // Only show hours with activity
        if (hourly[i].order_count > 0) {
            char trend[20] = "";
            if (hourly[i].revenue > 50.0) {
                strcpy(trend, "High");
            } else if (hourly[i].revenue > 20.0) {
                strcpy(trend, "Medium");
            } else {
                strcpy(trend, "Low");
            }
            
            printf("%02d:00  %-8d RM%-7.2f %-15s\n", 
                   i, hourly[i].order_count, hourly[i].revenue, trend);
        }
    }
    
    printf("------------------------------------------------\n");
    printf("TOTAL: %-8d RM%-7.2f\n", total_orders, total_revenue);
    
    printf("\n--- PEAK HOURS ANALYSIS ---\n");
    printf("Peak Hour: %02d:00 (RM%.2f revenue)\n", peak_hour, peak_revenue);
    
    // Find busy periods
    printf("Busy Periods:\n");
    for (int i = 0; i < count; i++) {
        if (hourly[i].revenue > total_revenue / count * 1.5) {
            printf("- %02d:00-%02d:00 (RM%.2f)\n", i, i+1, hourly[i].revenue);
        }
    }
    
    if (peak_hour >= 11 && peak_hour <= 13) {
        printf("\n--- OPERATIONAL INSIGHTS ---\n");
        printf("Lunch time is your peak period. Ensure adequate staffing.\n");
    } else if (peak_hour >= 17 && peak_hour <= 19) {
        printf("\n--- OPERATIONAL INSIGHTS ---\n");
        printf("Dinner time is your peak period. Prepare inventory accordingly.\n");
    }
    
    printf("==============================================\n");
}


// ---------------------------------------------------------------------------------------------

// Forward declarations for inventory tracking
int find_inventory_items(char *restaurant_id, Inventory *items, int *count);
void display_inventory_items(Inventory *items, int count);
void view_inventory(char *restaurant_id);
int add_inventory_item(char *restaurant_id);
int update_inventory_item(char *restaurant_id);
int delete_inventory_item(char *restaurant_id);
void check_low_stock_alerts(char *restaurant_id);
void input_inventory_name(char *name, int name_size);
int input_inventory_price(float *price);
int input_inventory_amount(int *amount);
void get_inventory_id(char *id, int id_size);
int write_inventory_file(const Inventory *inv);
int update_inventory_in_file(const Inventory *inv);
int delete_inventory_from_file(const char *inventory_id, const char *inventory_name);
int get_inventory_by_number(Inventory *items, int count, int item_number, Inventory *selected_item);

#define VIEW_INVENTORY 1
#define ADD_INVENTORY 2
#define UPDATE_INVENTORY 3
#define DELETE_INVENTORY 4
#define LOW_STOCK_ALERTS 5
#define LOW_STOCK_THRESHOLD 10

int inventory_tracking(void) {
    int choice;
    while (1) {
        printf("\n----- INVENTORY TRACKING -----\n");
        printf("1. View inventory\n");
        printf("2. Add inventory item\n");
        printf("3. Update inventory item\n");
        printf("4. Delete inventory item\n");
        printf("5. Check low-stock alerts\n");
        printf("0. Exit\n");
        
        choice = get_integer_input("What would you like to do?: ");
        
        switch (choice) {
            case VIEW_INVENTORY:
                view_inventory(current_restaurant_id);
                break;
            case ADD_INVENTORY:
                add_inventory_item(current_restaurant_id);
                break;
            case UPDATE_INVENTORY:
                update_inventory_item(current_restaurant_id);
                break;
            case DELETE_INVENTORY:
                delete_inventory_item(current_restaurant_id);
                break;
            case LOW_STOCK_ALERTS:
                check_low_stock_alerts(current_restaurant_id);
                break;
            case 0:
                return 0;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// Function to find inventory items for a specific restaurant
int find_inventory_items(char *restaurant_id, Inventory *items, int *count) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", INVENTORY_FILE);
        return FILE_ERROR;
    }

    *count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp) && *count < 100) {
        line[strcspn(line, "\n")] = '\0';

        char *token;
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy));

        token = strtok(line_copy, ",");
        if (!token) continue;
        strncpy(items[*count].inventory_id, token, INVENTORY_ID_LENGTH);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(items[*count].inventory_name, token, NAME_LENGTH);

        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(items[*count].restaurant_id, token, RESTAURANT_ID_LENGTH);

        token = strtok(NULL, ",");
        if (!token) continue;
        items[*count].inventory_price = atof(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        items[*count].inventory_amount = atoi(token);

        if (strcmp(restaurant_id, items[*count].restaurant_id) == 0) {
            (*count)++;
        }
    }

    fclose(fp);
    return *count;
}

// Function to display inventory items
void display_inventory_items(Inventory *items, int count) {
    if (count == 0) {
        printf("No inventory items found.\n");
        return;
    }

    printf("\n=========================== INVENTORY ITEMS ===========================\n");
    printf("%-5s %-12s %-20s %-12s %-10s %-8s\n", 
           "No.", "ID", "Item Name", "Price (RM)", "Quantity", "Status");
    printf("-----------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        const char *status = (items[i].inventory_amount <= LOW_STOCK_THRESHOLD) ? "LOW STOCK" : "OK";
        printf("%-5d %-12s %-20s %-10.2f %-14d %-11s\n", 
               i + 1, items[i].inventory_id, items[i].inventory_name,
               items[i].inventory_price, items[i].inventory_amount, status);
    }
    printf("-----------------------------------------------------------------------\n");
}

// Function to view inventory
void view_inventory(char *restaurant_id) {
    Inventory items[100];
    int count;
    printf("\n----- VIEW INVENTORY -----\n");

    if (find_inventory_items(restaurant_id, items, &count) == FILE_ERROR) return;

    display_inventory_items(items, count);
    printf("Total inventory items: %d\n", count);
    
    // Calculate total inventory value
    float total_value = 0.0;
    for (int i = 0; i < count; i++) {
        total_value += items[i].inventory_price * items[i].inventory_amount;
    }
    printf("Total inventory value: RM%.2f\n", total_value);
    
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

// Function to input inventory item name
void input_inventory_name(char *name, int name_size) {
    while (1) {
        printf("Enter inventory item name: ");
        if (fgets(name, name_size, stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            clear_input_buffer();
            continue;
        }
        name[strcspn(name, "\n")] = '\0';
        if (strlen(name) > 0) break;
        printf("Name cannot be empty. Please try again.\n");
    }
}

// Function to input inventory price
int input_inventory_price(float *price) {
    char input[20];
    while (1) {
        printf("Enter inventory item price (RM): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input. Please try again.\n");
            clear_input_buffer();
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        char *endptr;
        *price = strtof(input, &endptr);
        if (endptr != input && *endptr == '\0' && *price > 0) return 1;
        printf("Invalid input! Please enter a valid price (e.g., 12.50).\n");
    }
}

// Function to input inventory amount
int input_inventory_amount(int *amount) {
    while (1) {
        *amount = get_integer_input("Enter inventory quantity: ");
        if (*amount >= 0) return 1;
        printf("Invalid input! Please enter a non-negative quantity.\n");
    }
}

// Function to generate inventory ID
void get_inventory_id(char *id, int id_size) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "I001");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int max_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        char line_copy[MAX_LINE_LENGTH];
        strncpy(line_copy, line, sizeof(line_copy));
        char *token = strtok(line_copy, ",");
        if (token && token[0] == 'I') {
            int num = atoi(&token[1]);
            if (num > max_num) max_num = num;
        }
    }
    fclose(fp);
    snprintf(id, id_size, "I%03d", max_num + 1);
}

// Function to write inventory item to file
int write_inventory_file(const Inventory *inv) {
    FILE *fp = fopen(INVENTORY_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", INVENTORY_FILE);
        return FILE_ERROR;
    }
    
    fprintf(fp, "%s,%s,%s,%.2f,%d\n", 
            inv->inventory_id, inv->inventory_name, inv->restaurant_id,
            inv->inventory_price, inv->inventory_amount);
    fclose(fp);
    printf("\nInventory item '%s' added successfully!\n", inv->inventory_name);
    return 1;
}

// Function to add inventory item
int add_inventory_item(char *restaurant_id) {
    Inventory new_item;
    Inventory items[100];
    int count;

    printf("\n----- ADD INVENTORY ITEM -----\n");

    if (find_inventory_items(restaurant_id, items, &count) == FILE_ERROR) return 0;
    display_inventory_items(items, count);

    strcpy(new_item.restaurant_id, restaurant_id);
    input_inventory_name(new_item.inventory_name, sizeof(new_item.inventory_name));
    if (!input_inventory_price(&new_item.inventory_price)) return 0;
    if (!input_inventory_amount(&new_item.inventory_amount)) return 0;
    
    printf("\n--- NEW INVENTORY ITEM INFORMATION ---\n");
    printf("Item Name: %s\n", new_item.inventory_name);
    printf("Item Price: RM%.2f\n", new_item.inventory_price);
    printf("Item Quantity: %d\n", new_item.inventory_amount);
    
    int confirm = confirmation("Are you sure you want to add this inventory item? (1. Yes / 2. No): ");
    if (confirm == YES) {
        get_inventory_id(new_item.inventory_id, sizeof(new_item.inventory_id));
        return write_inventory_file(&new_item);
    }
    printf("Inventory item addition cancelled.\n");
    return 0;
}

// Function to get inventory item by number
int get_inventory_by_number(Inventory *items, int count, int item_number, Inventory *selected_item) {
    if (item_number < 1 || item_number > count) {
        return 0;
    }
    
    *selected_item = items[item_number - 1];
    return 1;
}

// Function to update inventory item in file
int update_inventory_in_file(const Inventory *updated_item) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", INVENTORY_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_inventory.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        Inventory temp_item;
        sscanf(line, "%[^,],%[^,],%[^,],%f,%d",
               temp_item.inventory_id, temp_item.inventory_name, temp_item.restaurant_id,
               &temp_item.inventory_price, &temp_item.inventory_amount);

        if (strcmp(temp_item.inventory_id, updated_item->inventory_id) == 0) {
            fprintf(temp_fp, "%s,%s,%s,%.2f,%d\n",
                    updated_item->inventory_id, updated_item->inventory_name,
                    updated_item->restaurant_id, updated_item->inventory_price,
                    updated_item->inventory_amount);
            updated = 1;
        } else {
            fprintf(temp_fp, "%s,%s,%s,%.2f,%d\n",
                    temp_item.inventory_id, temp_item.inventory_name,
                    temp_item.restaurant_id, temp_item.inventory_price,
                    temp_item.inventory_amount);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (updated) {
        remove(INVENTORY_FILE);
        rename("temp_inventory.txt", INVENTORY_FILE);
        printf("Inventory item '%s' has been updated successfully!\n", updated_item->inventory_name);
        return 1;
    } else {
        remove("temp_inventory.txt");
        printf("Inventory item not found for update.\n");
        return 0;
    }
}

// Function to update inventory item
int update_inventory_item(char *restaurant_id) {
    Inventory items[100];
    int count;
    
    printf("\n----- UPDATE INVENTORY ITEM -----\n");
    
    int result = find_inventory_items(restaurant_id, items, &count);
    if (result == FILE_ERROR) {
        return 0;
    }
    
    if (count == 0) {
        printf("No inventory items found to update.\n");
        return 0;
    }
    
    display_inventory_items(items, count);
    
    int update_choice;
    while (1) {
        update_choice = get_integer_input("Select item number to update (enter '0' to exit): ");
        if (update_choice >= 0 && update_choice <= count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", count);
    }
    
    if (update_choice == 0) {
        return 0;
    }
    
    Inventory selected_item;
    get_inventory_by_number(items, count, update_choice, &selected_item);
    
    printf("\nCurrent item details:\n");
    printf("Name: %s\n", selected_item.inventory_name);
    printf("Price: RM%.2f\n", selected_item.inventory_price);
    printf("Quantity: %d\n", selected_item.inventory_amount);
    
    Inventory updated_item = selected_item;
    
    printf("\nEnter new details:\n");
    input_inventory_name(updated_item.inventory_name, sizeof(updated_item.inventory_name));
    if (!input_inventory_price(&updated_item.inventory_price)) {
        return 0;
    }
    if (!input_inventory_amount(&updated_item.inventory_amount)) {
        return 0;
    }
    
    printf("\n--- UPDATED INVENTORY ITEM INFORMATION ---\n");
    printf("Item Name: %s\n", updated_item.inventory_name);
    printf("Item Price: RM%.2f\n", updated_item.inventory_price);
    printf("Item Quantity: %d\n", updated_item.inventory_amount);
    
    int confirm = confirmation("Are you sure you want to update this inventory item? (1. Yes / 2. No): ");
    if (confirm == 1) {
        return update_inventory_in_file(&updated_item);
    }
    else {
        printf("Inventory item update cancelled.\n");
        return 0;
    }
}

// Function to delete inventory item from file
int delete_inventory_from_file(const char *inventory_id, const char *inventory_name) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file for reading.\n", INVENTORY_FILE);
        return FILE_ERROR;
    }

    FILE *temp_fp = fopen("temp_inventory.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file.\n");
        fclose(fp);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    int deleted = 0;

    while (fgets(line, sizeof(line), fp)) {
        Inventory temp_item;
        sscanf(line, "%[^,],%[^,],%[^,],%f,%d",
               temp_item.inventory_id, temp_item.inventory_name, temp_item.restaurant_id,
               &temp_item.inventory_price, &temp_item.inventory_amount);

        if (strcmp(temp_item.inventory_id, inventory_id) == 0) {
            deleted = 1;
            continue; // skip this one
        } else {
            fprintf(temp_fp, "%s,%s,%s,%.2f,%d\n",
                    temp_item.inventory_id, temp_item.inventory_name,
                    temp_item.restaurant_id, temp_item.inventory_price,
                    temp_item.inventory_amount);
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (deleted) {
        remove(INVENTORY_FILE);
        rename("temp_inventory.txt", INVENTORY_FILE);
        printf("Inventory item '%s' has been deleted successfully!\n", inventory_name);
        return 1;
    } else {
        remove("temp_inventory.txt");
        printf("Inventory item not found for deletion.\n");
        return 0;
    }
}

// Function to delete inventory item
int delete_inventory_item(char *restaurant_id) {
    Inventory items[100];
    int count;
    
    printf("\n----- DELETE INVENTORY ITEM -----\n");
    
    int result = find_inventory_items(restaurant_id, items, &count);
    if (result == FILE_ERROR) {
        return 0;
    }
    
    if (count == 0) {
        printf("No inventory items found to delete.\n");
        return 0;
    }
    
    display_inventory_items(items, count);
    
    int delete_choice;
    while (1) {
        delete_choice = get_integer_input("Select item number to delete (enter '0' to exit): ");
        if (delete_choice >= 0 && delete_choice <= count) {
            break;
        }
        printf("Invalid input. Please enter a number between 1 and %d.\n", count);
    }
    
    if (delete_choice == 0) {
        return 0;
    }
    
    Inventory selected_item;
    get_inventory_by_number(items, count, delete_choice, &selected_item);
    
    printf("\nItem to delete:\n");
    printf("Name: %s\n", selected_item.inventory_name);
    printf("Price: RM%.2f\n", selected_item.inventory_price);
    printf("Quantity: %d\n", selected_item.inventory_amount);
    
    int confirm = confirmation("Are you sure you want to delete this inventory item? (1. Yes / 2. No): ");
    if (confirm == 1) {
        return delete_inventory_from_file(selected_item.inventory_id, selected_item.inventory_name);
    }
    else {
        printf("Inventory item deletion cancelled.\n");
        return 0;
    }
}

// Function to check low-stock alerts
void check_low_stock_alerts(char *restaurant_id) {
    Inventory items[100];
    int count;
    
    printf("\n----- LOW-STOCK ALERTS -----\n");
    
    if (find_inventory_items(restaurant_id, items, &count) == FILE_ERROR) return;
    
    printf("Low-stock threshold: %d units\n", LOW_STOCK_THRESHOLD);
    printf("\n==================== LOW-STOCK ITEMS ====================\n");
    
    int low_stock_count = 0;
    for (int i = 0; i < count; i++) {
        if (items[i].inventory_amount <= LOW_STOCK_THRESHOLD) {
            if (low_stock_count == 0) {
                printf("%-12s %-20s %-12s %-10s\n", 
                       "ID", "Item Name", "Price (RM)", "Quantity");
                printf("-------------------------------------------------------\n");
            }
            printf("%-12s %-20s RM%-10.2f %-10d\n", 
                   items[i].inventory_id, items[i].inventory_name,
                   items[i].inventory_price, items[i].inventory_amount);
            low_stock_count++;
        }
    }
    
    if (low_stock_count == 0) {
        printf("No low-stock items found. All inventory levels are adequate.\n");
    } else {
        printf("-------------------------------------------------------\n");
        printf("Total low-stock items: %d\n", low_stock_count);
        printf("\n*** ATTENTION: Please restock these items soon! ***\n");
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
// Function to view daily sales summary
void view_daily_sales_summary(void) {
    char date[DATE_LENGTH];
    
    printf("\n----- DAILY SALES SUMMARY -----\n");
    printf("Enter date (DD-MM-YYYY) or press Enter for today: ");
    
    char input[20];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    
    if (strlen(input) == 0) {
        get_current_date_restaurant(date);
    } else {
        strcpy(date, input);
    }
    
    DailySales sales;
    if (calculate_daily_sales(current_restaurant_id, date, &sales) == FILE_ERROR) {
        return;
    }
    
    display_daily_sales_summary(&sales);
    
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

// Function to view item sales analysis
void view_item_sales_analysis(void) {
    char date[DATE_LENGTH];
    
    printf("\n----- ITEM SALES ANALYSIS -----\n");
    printf("Enter date (DD-MM-YYYY) or press Enter for today: ");
    
    char input[20];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    
    if (strlen(input) == 0) {
        get_current_date_restaurant(date);
    } else {
        strcpy(date, input);
    }
    
    ItemSales items[50];
    int count;
    
    if (get_item_sales_data(current_restaurant_id, date, items, &count) == FILE_ERROR) {
        return;
    }
    
    display_item_sales_analysis(items, count);
    
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

// Function to view hourly sales trend
void view_hourly_sales_trend(void) {
    char date[DATE_LENGTH];
    
    printf("\n----- HOURLY SALES TREND -----\n");
    printf("Enter date (DD-MM-YYYY) or press Enter for today: ");
    
    char input[20];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    
    if (strlen(input) == 0) {
        get_current_date_restaurant(date);
    } else {
        strcpy(date, input);
    }
    
    HourlySales hourly[24];
    int count;
    
    if (get_hourly_sales_data(current_restaurant_id, date, hourly, &count) == FILE_ERROR) {
        return;
    }
    
    display_hourly_sales_trend(hourly, count);
    
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

// Function to view sales comparison
void view_sales_comparison(void) {
    printf("\n----- SALES COMPARISON -----\n");
    
    char date1[DATE_LENGTH], date2[DATE_LENGTH];
    
    if (!safe_string_input("Enter first date (DD-MM-YYYY): ", date1, sizeof(date1))) {
        printf("Invalid date format.\n");
        return;
    }
    
    if (!safe_string_input("Enter second date (DD-MM-YYYY): ", date2, sizeof(date2))) {
        printf("Invalid date format.\n");
        return;
    }
    
    DailySales sales1, sales2;
    
    if (calculate_daily_sales(current_restaurant_id, date1, &sales1) == FILE_ERROR ||
        calculate_daily_sales(current_restaurant_id, date2, &sales2) == FILE_ERROR) {
        return;
    }
    
    printf("\n==================== SALES COMPARISON ====================\n");
    printf("%-25s %-12s %-12s %-12s\n", "Metric", date1, date2, "Change");
    printf("------------------------------------------------------------\n");
    
    printf("%-25s %-12d %-12d ", "Total Orders", sales1.total_orders, sales2.total_orders);
    int order_change = sales2.total_orders - sales1.total_orders;
    if (order_change > 0) {
        printf("+%d\n", order_change);
    } else {
        printf("%d\n", order_change);
    }
    
    printf("%-25s %-12d %-12d ", "Completed Orders", sales1.completed_orders, sales2.completed_orders);
    int completed_change = sales2.completed_orders - sales1.completed_orders;
    if (completed_change > 0) {
        printf("+%d\n", completed_change);
    } else {
        printf("%d\n", completed_change);
    }
    
    printf("%-25s RM%-10.2f RM%-10.2f ", "Total Revenue", sales1.total_revenue, sales2.total_revenue);
    float revenue_change = sales2.total_revenue - sales1.total_revenue;
    if (revenue_change > 0) {
        printf("+RM%.2f\n", revenue_change);
    } else {
        printf("RM%.2f\n", revenue_change);
    }
    
    printf("%-25s RM%-10.2f RM%-10.2f ", "Completed Revenue", sales1.completed_revenue, sales2.completed_revenue);
    float completed_revenue_change = sales2.completed_revenue - sales1.completed_revenue;
    if (completed_revenue_change > 0) {
        printf("+RM%.2f\n", completed_revenue_change);
    } else {
        printf("RM%.2f\n", completed_revenue_change);
    }
    
    printf("%-25s RM%-10.2f RM%-10.2f ", "Average Order Value", sales1.average_order_value, sales2.average_order_value);
    float avg_change = sales2.average_order_value - sales1.average_order_value;
    if (avg_change > 0) {
        printf("+RM%.2f\n", avg_change);
    } else {
        printf("RM%.2f\n", avg_change);
    }
    
    printf("------------------------------------------------------------\n");
    
    printf("\n--- COMPARISON INSIGHTS ---\n");
    if (revenue_change > 0) {
        printf("Revenue increased by RM%.2f (%.1f%% growth)\n", 
               revenue_change, (revenue_change / sales1.total_revenue) * 100);
    } else if (revenue_change < 0) {
        printf("Revenue decreased by RM%.2f (%.1f%% decline)\n", 
               -revenue_change, (-revenue_change / sales1.total_revenue) * 100);
    } else {
        printf("Revenue remained stable.\n");
    }
    
    if (order_change > 0) {
        printf("Order volume increased by %d orders.\n", order_change);
    } else if (order_change < 0) {
        printf("Order volume decreased by %d orders.\n", -order_change);
    } else {
        printf("Order volume remained stable.\n");
    }
    
    printf("==========================================================\n");
    
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

// Function to export sales report
void export_sales_report(void) {
    char date[DATE_LENGTH];
    char filename[100];
    
    printf("\n----- EXPORT SALES REPORT -----\n");
    printf("Enter date (DD-MM-YYYY) or press Enter for today: ");
    
    char input[20];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';
    
    if (strlen(input) == 0) {
        get_current_date_restaurant(date);
    } else {
        strcpy(date, input);
    }
    
    if (!safe_filename_input("Enter filename (without extension): ", filename, sizeof(filename))) {
        printf("Invalid filename.\n");
        return;
    }
    
    char full_filename[120];
    snprintf(full_filename, sizeof(full_filename), "%s_%s_sales_report.txt", current_restaurant_id, filename);
    
    FILE *fp = fopen(full_filename, "w");
    if (fp == NULL) {
        printf("Error: Could not create export file.\n");
        return;
    }
    
    // Get sales data
    DailySales sales;
    ItemSales items[50];
    int item_count;
    HourlySales hourly[24];
    int hourly_count;
    
    calculate_daily_sales(current_restaurant_id, date, &sales);
    get_item_sales_data(current_restaurant_id, date, items, &item_count);
    get_hourly_sales_data(current_restaurant_id, date, hourly, &hourly_count);
    
    // Write report header
    fprintf(fp, "========================================\n");
    fprintf(fp, "DAILY SALES REPORT\n");
    fprintf(fp, "========================================\n");
    fprintf(fp, "Restaurant ID: %s\n", current_restaurant_id);
    fprintf(fp, "Report Date: %s\n", date);
    fprintf(fp, "Generated on: %s\n", __DATE__);
    fprintf(fp, "========================================\n\n");
    
    // Write daily summary
    fprintf(fp, "DAILY SALES SUMMARY\n");
    fprintf(fp, "-------------------\n");
    fprintf(fp, "Total Orders: %d\n", sales.total_orders);
    fprintf(fp, "Completed Orders: %d\n", sales.completed_orders);
    fprintf(fp, "Pending Orders: %d\n", sales.pending_orders);
    fprintf(fp, "Cancelled Orders: %d\n", sales.cancelled_orders);
    fprintf(fp, "Total Revenue: RM%.2f\n", sales.total_revenue);
    fprintf(fp, "Completed Revenue: RM%.2f\n", sales.completed_revenue);
    fprintf(fp, "Pending Revenue: RM%.2f\n", sales.pending_revenue);
    fprintf(fp, "Average Order Value: RM%.2f\n", sales.average_order_value);
    
    if (sales.total_orders > 0) {
        fprintf(fp, "Completion Rate: %.1f%%\n", (float)sales.completed_orders / sales.total_orders * 100);
        fprintf(fp, "Cancellation Rate: %.1f%%\n", (float)sales.cancelled_orders / sales.total_orders * 100);
    }
    
    fprintf(fp, "\n");
    
    // Write item sales analysis
    fprintf(fp, "ITEM SALES ANALYSIS\n");
    fprintf(fp, "-------------------\n");
    fprintf(fp, "%-8s %-20s %-8s %-8s %-10s %-8s\n", 
            "Item ID", "Item Name", "Qty Sold", "Unit Price", "Revenue", "Orders");
    fprintf(fp, "-----------------------------------------------------------------------\n");
    
    for (int i = 0; i < item_count; i++) {
        fprintf(fp, "%-8s %-20s %-8d RM%-6.2f RM%-8.2f %-8d\n", 
                items[i].item_id, items[i].item_name, items[i].quantity_sold,
                items[i].unit_price, items[i].total_revenue, items[i].order_count);
    }
    
    fprintf(fp, "\n");
    
    // Write hourly trend
    fprintf(fp, "HOURLY SALES TREND\n");
    fprintf(fp, "------------------\n");
    fprintf(fp, "%-6s %-8s %-10s\n", "Hour", "Orders", "Revenue");
    fprintf(fp, "-------------------------\n");
    
    for (int i = 0; i < hourly_count; i++) {
        if (hourly[i].order_count > 0) {
            fprintf(fp, "%02d:00  %-8d RM%-7.2f\n", 
                    i, hourly[i].order_count, hourly[i].revenue);
        }
    }
    
    fprintf(fp, "\n");
    fprintf(fp, "========================================\n");
    fprintf(fp, "END OF REPORT\n");
    fprintf(fp, "========================================\n");
    
    fclose(fp);
    printf("Sales report exported successfully to '%s'.\n", full_filename);
    
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