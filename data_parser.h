#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "safe_input.h"
#include "setting.h"
#include "enums.h"
#include "user.h"
#include "student.h"
#include "restaurant.h"
#include "delivery_personnel.h"

// Common data structures for parsing
typedef struct {
    char order_id[ORDER_ID_LENGTH];
    char student_id[STUDENT_ID_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
    float total_price;
    char order_date[DATE_LENGTH];
    char order_time[TIME_LENGTH];
    char status[20];
} ParsedOrder;

typedef struct {
    char delivery_id[DELIVERY_ID_LENGTH];
    char order_id[ORDER_ID_LENGTH];
    char delivery_date[DATE_LENGTH];
    char eta[TIME_LENGTH];
    char delivered_time[TIME_LENGTH];
    int delivery_status;
    int punctuality_status;
} ParsedDelivery;

typedef struct {
    char restaurant_id[RESTAURANT_ID_LENGTH];
    char menu_item_id[MENU_ITEM_ID_LENGTH];
    char item_name[NAME_LENGTH];
    float price;
} ParsedMenuItem;

// Common parsing functions
int parse_user_line(const char *line, User *user);
int parse_student_line(const char *line, Student *student);
int parse_restaurant_line(const char *line, Restaurant *restaurant);
int parse_delivery_personnel_line(const char *line, DeliveryPersonnel *personnel);
int parse_order_line(const char *line, ParsedOrder *order);
int parse_delivery_line(const char *line, ParsedDelivery *delivery);
int parse_menu_item_line(const char *line, ParsedMenuItem *item);

// Common file reading functions
int read_users_from_file(const char *filename, User *users, int max_count);
int read_students_from_file(const char *filename, Student *students, int max_count);
int read_orders_from_file(const char *filename, ParsedOrder *orders, int max_count);
int read_deliveries_from_file(const char *filename, ParsedDelivery *deliveries, int max_count);

// Common search functions
int find_user_by_email_in_array(User *users, int count, const char *email);
int find_student_by_id_in_array(Student *students, int count, const char *student_id);
int find_order_by_id_in_array(ParsedOrder *orders, int count, const char *order_id);

// Common validation functions (standardized)
int validate_user_data(const User *user);
int validate_student_data(const Student *student);
int validate_order_data(const ParsedOrder *order);

// Common display functions
void display_user_info_standard(const User *user);
void display_student_info_standard(const Student *student);
void display_order_info_standard(const ParsedOrder *order);

// File format functions
int format_user_line(const User *user, char *buffer, int buffer_size);
int format_student_line(const Student *student, char *buffer, int buffer_size);
int format_order_line(const ParsedOrder *order, char *buffer, int buffer_size);

#endif