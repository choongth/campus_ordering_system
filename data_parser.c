#include "data_parser.h"

// Parse user line from file
int parse_user_line(const char *line, User *user) {
    if (!line || !user) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%s",
                       user->id, (int*)&user->role, user->name, 
                       user->contact_number, user->email_address, user->password);
    
    return (result == 6) ? 1 : 0;
}

// Parse student line from file
int parse_student_line(const char *line, Student *student) {
    if (!line || !student) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%f",
                       student->student_id, student->user.name,
                       student->user.contact_number, student->user.email_address,
                       student->user.password, (int*)&student->account_activate_status,
                       &student->account_balance);
    
    // Set user role and copy student_id to user.id
    student->user.role = STUDENT;
    strcpy(student->user.id, student->student_id);
    
    return (result == 7) ? 1 : 0;
}

// Parse restaurant line from file
int parse_restaurant_line(const char *line, Restaurant *restaurant) {
    if (!line || !restaurant) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d",
                       restaurant->restaurant_id, restaurant->user.name,
                       restaurant->user.contact_number, restaurant->user.email_address,
                       restaurant->user.password, (int*)&restaurant->cuisine);
    
    // Set user role and copy restaurant_id to user.id
    restaurant->user.role = RESTAURANT;
    strcpy(restaurant->user.id, restaurant->restaurant_id);
    
    return (result == 6) ? 1 : 0;
}

// Parse delivery personnel line from file
int parse_delivery_personnel_line(const char *line, DeliveryPersonnel *personnel) {
    if (!line || !personnel) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%s",
                       personnel->delivery_personnel_id, personnel->user.name,
                       personnel->user.contact_number, personnel->user.email_address,
                       personnel->user.password, (int*)&personnel->status,
                       personnel->delivery_id);
    
    // Set user role and copy personnel_id to user.id
    personnel->user.role = DELIVERY_PERSONNEL;
    strcpy(personnel->user.id, personnel->delivery_personnel_id);
    
    return (result == 7) ? 1 : 0;
}

// Parse order line from file
int parse_order_line(const char *line, ParsedOrder *order) {
    if (!line || !order) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%[^,],%[^,],%f,%[^,],%[^,],%s",
                       order->order_id, order->student_id, order->restaurant_id,
                       &order->total_price, order->order_date, order->order_time,
                       order->status);
    
    return (result == 7) ? 1 : 0;
}

// Parse delivery line from file
int parse_delivery_line(const char *line, ParsedDelivery *delivery) {
    if (!line || !delivery) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d",
                       delivery->delivery_id, delivery->order_id, delivery->delivery_date,
                       delivery->eta, delivery->delivered_time,
                       &delivery->delivery_status, &delivery->punctuality_status);
    
    return (result == 7) ? 1 : 0;
}

// Parse menu item line from file
int parse_menu_item_line(const char *line, ParsedMenuItem *item) {
    if (!line || !item) {
        return 0;
    }
    
    int result = sscanf(line, "%[^,],%[^,],%[^,],%f",
                       item->restaurant_id, item->menu_item_id,
                       item->item_name, &item->price);
    
    return (result == 4) ? 1 : 0;
}

// Read users from file
int read_users_from_file(const char *filename, User *users, int max_count) {
    if (!filename || !users || max_count <= 0) {
        return 0;
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    while (fgets(line, sizeof(line), fp) && count < max_count) {
        line[strcspn(line, "\n")] = '\0';
        
        if (parse_user_line(line, &users[count])) {
            count++;
        }
    }
    
    fclose(fp);
    return count;
}

// Read students from file
int read_students_from_file(const char *filename, Student *students, int max_count) {
    if (!filename || !students || max_count <= 0) {
        return 0;
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    while (fgets(line, sizeof(line), fp) && count < max_count) {
        line[strcspn(line, "\n")] = '\0';
        
        if (parse_student_line(line, &students[count])) {
            count++;
        }
    }
    
    fclose(fp);
    return count;
}

// Read orders from file
int read_orders_from_file(const char *filename, ParsedOrder *orders, int max_count) {
    if (!filename || !orders || max_count <= 0) {
        return 0;
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        return 0;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    
    while (fgets(line, sizeof(line), fp) && count < max_count) {
        line[strcspn(line, "\n")] = '\0';
        
        if (parse_order_line(line, &orders[count])) {
            count++;
        }
    }
    
    fclose(fp);
    return count;
}

// Find user by email in array
int find_user_by_email_in_array(User *users, int count, const char *email) {
    if (!users || !email || count <= 0) {
        return -1;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].email_address, email) == 0) {
            return i;
        }
    }
    
    return -1;
}

// Find student by ID in array
int find_student_by_id_in_array(Student *students, int count, const char *student_id) {
    if (!students || !student_id || count <= 0) {
        return -1;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].student_id, student_id) == 0) {
            return i;
        }
    }
    
    return -1;
}

// Find order by ID in array
int find_order_by_id_in_array(ParsedOrder *orders, int count, const char *order_id) {
    if (!orders || !order_id || count <= 0) {
        return -1;
    }
    
    for (int i = 0; i < count; i++) {
        if (strcmp(orders[i].order_id, order_id) == 0) {
            return i;
        }
    }
    
    return -1;
}

// Validate user data
int validate_user_data(const User *user) {
    if (!user) {
        return 0;
    }
    
    // Validate ID
    if (strlen(user->id) == 0 || strlen(user->id) >= USER_ID_LENGTH) {
        return 0;
    }
    
    // Validate name
    if (strlen(user->name) == 0 || strlen(user->name) >= NAME_LENGTH) {
        return 0;
    }
    
    // Validate contact
    if (!is_valid_contact(user->contact_number)) {
        return 0;
    }
    
    // Validate email
    if (!is_valid_email(user->email_address)) {
        return 0;
    }
    
    // Validate password
    if (!is_valid_password(user->password)) {
        return 0;
    }
    
    return 1;
}

// Validate student data
int validate_student_data(const Student *student) {
    if (!student) {
        return 0;
    }
    
    // Validate user data first
    if (!validate_user_data(&student->user)) {
        return 0;
    }
    
    // Validate student ID
    if (strlen(student->student_id) == 0 || strlen(student->student_id) >= STUDENT_ID_LENGTH) {
        return 0;
    }
    
    // Validate balance (should be non-negative)
    if (student->account_balance < 0) {
        return 0;
    }
    
    return 1;
}

// Format user line for file output
int format_user_line(const User *user, char *buffer, int buffer_size) {
    if (!user || !buffer || buffer_size <= 0) {
        return 0;
    }
    
    int result = snprintf(buffer, buffer_size, "%s,%d,%s,%s,%s,%s\n",
                         user->id, user->role, user->name,
                         user->contact_number, user->email_address, user->password);
    
    return (result > 0 && result < buffer_size) ? 1 : 0;
}

// Format student line for file output
int format_student_line(const Student *student, char *buffer, int buffer_size) {
    if (!student || !buffer || buffer_size <= 0) {
        return 0;
    }
    
    int result = snprintf(buffer, buffer_size, "%s,%s,%s,%s,%s,%d,%.2f\n",
                         student->student_id, student->user.name,
                         student->user.contact_number, student->user.email_address,
                         student->user.password, student->account_activate_status,
                         student->account_balance);
    
    return (result > 0 && result < buffer_size) ? 1 : 0;
}

// Format order line for file output
int format_order_line(const ParsedOrder *order, char *buffer, int buffer_size) {
    if (!order || !buffer || buffer_size <= 0) {
        return 0;
    }
    
    int result = snprintf(buffer, buffer_size, "%s,%s,%s,%.2f,%s,%s,%s\n",
                         order->order_id, order->student_id, order->restaurant_id,
                         order->total_price, order->order_date, order->order_time,
                         order->status);
    
    return (result > 0 && result < buffer_size) ? 1 : 0;
}