#ifndef ORDER_H
#define ORDER_H

#include "setting.h"

// Structure to represent an item in an order
typedef struct OrderItem {
    char menu_item_id[MENU_ITEM_ID_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
    char restaurant_name[NAME_LENGTH];
    char cuisine_name[50];
    char item_name[NAME_LENGTH];
    float item_price;
    int quantity;
    float subtotal;
} OrderItem;

// Structure to display menu items for ordering
typedef struct MenuItemDisplay {
    char menu_item_id[MENU_ITEM_ID_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
    char restaurant_name[NAME_LENGTH];
    char cuisine_name[50];
    char item_name[NAME_LENGTH];
    float item_price;
} MenuItemDisplay;

// Structure to represent an order in the queue management system
typedef struct QueueOrder {
    char order_id[ORDER_ID_LENGTH];
    char student_id[STUDENT_ID_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
    float total_price;
    char order_date[DATE_LENGTH];
    char order_time[TIME_LENGTH];
    char status[20];
    char items_summary[200];
} QueueOrder;

// Important Note: Order struct is defined in restaurant.h to avoid conflicts

#endif