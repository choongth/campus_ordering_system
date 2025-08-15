#include "registration.h"

int register_student(Student *s);
int register_restaurant(Restaurant *r);
int register_delivery_personnel(DeliveryPersonnel *p);

void input_name(char *role, char *name, int name_size);
void input_contact_number(char *role, char *contact_number, int contact_number_size);
void input_email_address(char *role, char *email_address, int email_address_size);
void input_password(char *role, char *password, int password_size);

void get_cuisine(CuisineType *cuisine);

void get_user_id(char *id, int id_size);
void get_student_id(char *id, int id_size);
void get_restaurant_id(char *id, int id_size);
void get_delivery_personnel_id(char *id, int id_size);

int write_student_file(const Student *s);
int write_delivery_personnel_file(const DeliveryPersonnel *p);
int write_restaurant_file(const Restaurant *r);
int write_user_file(const User *u);

void print_user_information(const char *role, const User *u);
void print_restaurant_information(const Restaurant *r);
const char* get_cuisine_name(CuisineType cuisine);

// ------------- Major function to control the users' information collection flow -----------------

int register_student(Student *s) {
    printf("\n----- STUDENT'S REGISTRATION -----");

    // get basic information
    input_name("student", s->user.name, sizeof(s->user.name));
    input_contact_number("student", s->user.contact_number, sizeof(s->user.contact_number));
    input_email_address("student", s->user.email_address, sizeof(s->user.email_address));
    input_password("student", s->user.password, sizeof(s->user.password));

    while (1) {
        // print student detail for checking purpose
        print_user_information("STUDENT", &s->user);

        // confirmation
        int confirm = confirmation("Confirm registration? (1. Yes / 2. No): ");
        if (confirm == YES) break;
        else if (confirm == NO) return false; // User cancelled
        else continue;
    }

    // student-specific fields
    s->user.role = STUDENT;
    s->account_activate_status = DEACTIVATE;
    s->account_balance = 0.00;

    // get id
    get_student_id(s->student_id, sizeof(s->student_id));
    get_user_id(s->user.id, sizeof(s->user.id));

    // write file
    write_student_file(s);
    write_user_file(&s->user);
}

int register_restaurant(Restaurant *r) {
    printf("\n----- RESTAURANT'S REGISTRATION -----");

    // get basic information
    input_name("restaurant", r->user.name, sizeof(r->user.name));
    input_contact_number("restaurant", r->user.contact_number, sizeof(r->user.contact_number));
    input_email_address("restaurant", r->user.email_address, sizeof(r->user.email_address));
    input_password("restaurant", r->user.password, sizeof(r->user.password));

    // restaurant-specific fields: cuisine
    get_cuisine(&r->cuisine);

    while (1) {
        // print restaurant detail for checking purpose
        print_restaurant_information(r);

        // confirmation
        int confirm = confirmation("Confirm registration? (1. Yes / 2. No): ");
        if (confirm == YES) break;
        else if (confirm == NO) return false; // User cancelled
        else continue;
    }

    // set role
    r->user.role = RESTAURANT;

    //get id
    get_restaurant_id(r->restaurant_id, sizeof(r->restaurant_id));
    get_user_id(r->user.id, sizeof(r->user.id));

    // write file
    write_restaurant_file(r);
    write_user_file(&r->user);
}

int register_delivery_personnel(DeliveryPersonnel *p) {
    printf("\n----- DELIVERY PERSONNEL'S REGISTRATION -----");

    // get basic information
    input_name("delivery personnel", p->user.name, sizeof(p->user.name));
    input_contact_number("delivery personnel", p->user.contact_number, sizeof(p->user.contact_number));
    input_email_address("delivery personnel", p->user.email_address, sizeof(p->user.email_address));
    input_password("delivery personnel", p->user.password, sizeof(p->user.password));

    while (1) {
        // print delivery personnel detail for checking purpose
        print_user_information("DELIVERY PERSONNEL", &p->user);

        // confirmation
        int confirm = confirmation("Confirm registration? (1. Yes / 2. No): ");
        if (confirm == YES) break;
        else if (confirm == NO) return false; // User cancelled
        else continue;
    }

    // delivery personnel-specific fields
    p->user.role = DELIVERY_PERSONNEL;
    p->status = NO_ORDER_ASSIGNED;
    strcpy(p->delivery_id, "none");

    // get id
    get_delivery_personnel_id(p->delivery_personnel_id, sizeof(p->delivery_personnel_id));
    get_user_id(p->user.id, sizeof(p->user.id));

    // write file
    write_delivery_personnel_file(p);
    write_user_file(&p->user);
}

// ------------- Get basic users' information -------------------------------------------------

void input_name(char *role, char *name, int name_size) {
    while (1) {
        printf("\nEnter %s's name: ", role);
        if (fgets(name, name_size, stdin) == NULL)
            continue;
        name[strcspn(name, "\n")] = '\0';
        break;
    }
}

void input_contact_number(char *role, char *contact_number, int contact_number_size) {
    while (1) {
        printf("Enter %s's contact number: ", role);
        if (fgets(contact_number, contact_number_size, stdin) == NULL)
            continue;

        // Remove newline if present
        size_t len = strlen(contact_number);
        if (len > 0 && contact_number[len - 1] == '\n') {
            contact_number[len - 1] = '\0';
        } else {
            // Clear buffer if too long (no newline found)
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        if (strlen(contact_number) >= 10 && strlen(contact_number) <= 11) break;
        printf("Contact number must be 10 or 11 digits. Please try again.\n");
    }
}

void input_email_address(char *role, char *email_address, int email_address_size) {
    while (1) {
        printf("Enter %s's email address: ", role);
        if (fgets(email_address, email_address_size, stdin) == NULL)
            continue;
        email_address[strcspn(email_address, "\n")] = '\0';
        break;
    }

}

void input_password(char *role, char *password, int password_size) {
    while (1) {
        printf("Enter %s's password: ", role);
        fgets(password, password_size, stdin);
        password[strcspn(password, "\n")] = '\0';

        int len = strlen(password);
        if (len >= 6 && len <= 20) break;
        printf("Password must be 6 - 20 characters. Please try again.\n");
    }
}

// ----------- Restaurant specific field - cuisine ---------------------------------------

void get_cuisine(CuisineType *cuisine) {
    while (1) {
        char prompt[] = "\n--- CUISINE TO SELL ---\n1. Chinese\n2. Western\n3. Arabic\n4. Japanese\n5. Korean\n6. Asian\n7. Indian\n8. Malay\n9. African\n10. Fast Food\n11. Desserts\nChoose type of cuisine to sell (1 - 11): ";
        int choice = get_integer_input(prompt);
        if (choice >= CHINESE && choice <= DESSERTS) {
            *cuisine = (CuisineType)choice;
            break;
        }
        else {
            printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}

// -------------- Get the last id -----------------------------------------------------------------

void get_user_id(char *id, int id_size) {
    FILE *fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "U000001");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char last_id[USER_ID_LENGTH] = "U000000";

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,]", last_id);
    }
    fclose(fp);

    int num = atoi(&last_id[1]) + 1;
    snprintf(id, id_size, "U%06d", num);
}

void get_student_id(char *id, int id_size) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "S000001");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char last_id[STUDENT_ID_LENGTH] = "S000000";

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,]", last_id);
    }
    fclose(fp);

    int num = atoi(&last_id[1]) + 1;
    snprintf(id, id_size, "S%06d", num);
}

void get_restaurant_id(char *id, int id_size) {
    FILE *fp = fopen(RESTAURANT_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "R01");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char last_id[RESTAURANT_ID_LENGTH] = "R00";

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,]", last_id);
    }
    fclose(fp);

    int num = atoi(&last_id[1]) + 1;
    snprintf(id, id_size, "R%02d", num);
}

void get_delivery_personnel_id(char *id, int id_size) {
    FILE *fp = fopen(DELIVERY_PERSONNEL_FILE, "r");
    if (fp == NULL) {
        snprintf(id, id_size, "P001");
        return;
    }

    char line[MAX_LINE_LENGTH];
    char last_id[DELIVERY_PERSONNEL_ID_LENGTH] = "P000";

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,]", last_id);
    }
    fclose(fp);

    int num = atoi(&last_id[1]) + 1;
    snprintf(id, id_size, "P%03d", num);
}

// ---------- Write users' information into file --------------------------------------------

int write_student_file(const Student *s) {
    FILE *fp = fopen(STUDENT_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", STUDENT_FILE);
        return FILE_ERROR;
    }

    fprintf(fp, "%s,%s,%s,%s,%s,%d,%.2f\n", 
            s->student_id, s->user.name, s->user.contact_number, 
            s->user.email_address, s->user.password, 
            s->account_activate_status, s->account_balance);
    fclose(fp);

    printf("\nStudent '%s' registered successfully!\n", s->user.name);
    return 1;
}

int write_delivery_personnel_file(const DeliveryPersonnel *p) {
    FILE *fp = fopen(DELIVERY_PERSONNEL_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", DELIVERY_PERSONNEL_FILE);
        return FILE_ERROR;
    }

    fprintf(fp, "%s,%s,%s,%s,%s,%d,%s\n", 
            p->delivery_personnel_id, p->user.name, p->user.contact_number, 
            p->user.email_address, p->user.password, 
            p->status, p->delivery_id);
    fclose(fp);

    printf("\nDelivery personnel '%s' registered successfully!\n", p->user.name);
    return 1;
}

int write_restaurant_file(const Restaurant *r) {
    FILE *fp = fopen(RESTAURANT_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", RESTAURANT_FILE);
        return FILE_ERROR;
    }

    fprintf(fp, "%s,%s,%s,%s,%s,%d\n", 
            r->restaurant_id, r->user.name, r->user.contact_number, 
            r->user.email_address, r->user.password, r->cuisine);
    fclose(fp);

    printf("\nRestaurant '%s' registered successfully!\n", r->user.name);
    return 1;
}

int write_user_file(const User *u) {
    FILE *fp = fopen(USER_FILE, "a");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return FILE_ERROR;
    }

    fprintf(fp, "%s,%d,%s,%s,%s,%s\n", u->id, u->role, u->name, u->contact_number, u->email_address, u->password);
    fclose(fp);

    printf("\nUser '%s' registered successfully!\n", u->name);
    return 1;
}

// -----------------------------------------------------------------------------------------------

void print_user_information(const char *role, const User *u) {
    printf("\n----- %s INFORMATION -----\n", role);
    printf("Name: %s\n", u->name);
    printf("Contact Number: %s\n", u->contact_number);
    printf("Email Address: %s\n", u->email_address);
    printf("Password: %s\n", u->password);
}

const char* get_cuisine_name(CuisineType cuisine) {
    switch (cuisine) {
        case CHINESE: return "Chinese";
        case WESTERN: return "Western";
        case ARABIC: return "Arabic";
        case JAPANESE: return "Japanese";
        case KOREAN: return "Korean";
        case ASIAN: return "Asian";
        case INDIAN: return "Indian";
        case MALAY: return "Malay";
        case AFRICAN: return "African";
        case FAST_FOOD: return "Fast Food";
        case DESSERTS: return "Desserts";
        default: return "Unknown";
    }
}

void print_restaurant_information(const Restaurant *r) {
    printf("\n----- RESTAURANT INFORMATION -----\n");
    printf("Name: %s\n", r->user.name);
    printf("Contact Number: %s\n", r->user.contact_number);
    printf("Email Address: %s\n", r->user.email_address);
    printf("Password: %s\n", r->user.password);
    printf("Cuisine Type: %s\n", get_cuisine_name(r->cuisine));
}

// -----------------------------------------------------------------------------------------------

int registration(User *u) {
    while (1) {
        char prompt[] = "\n----- USER REGISTRATION -----\n1. Student\n2. Restaurant\n3. Delivery Personnel\n0. Exit\nRegister?: ";
        int register_role = get_integer_input(prompt);
        switch (register_role) {
            case STUDENT: {
                Student s;
                if (register_student(&s)) {
                    *u = s.user; // Copy the user part for login
                    return true;
                }
                return false;
            }
            case RESTAURANT: {
                Restaurant r;
                if (register_restaurant(&r)) {
                    *u = r.user; // Copy the user part for login
                    return true;
                }
                return false;
            }
            case DELIVERY_PERSONNEL: {
                DeliveryPersonnel p;
                if (register_delivery_personnel(&p)) {
                    *u = p.user; // Copy the user part for login
                    return true;
                }
                return false;
            }
            case 0: // Exit
                return false;
            default:
                printf("Invalid input! Please enter a valid choice.\n");
        }
    }
}
