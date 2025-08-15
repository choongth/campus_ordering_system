#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "setting.h"
#include "enums.h"
#include "share.h"
#include "user.h"
#include "order.h"

typedef struct Restaurant {
    User user;
    char restaurant_id[RESTAURANT_ID_LENGTH];
    CuisineType cuisine;
} Restaurant;

typedef struct MenuItem {
    char restaurant_id[RESTAURANT_ID_LENGTH];
    char id[MENU_ITEM_ID_LENGTH];
    char name[NAME_LENGTH];
    float price;
} MenuItem;

typedef struct Inventory {
    char inventory_id[INVENTORY_ID_LENGTH];
    char inventory_name[NAME_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
    float inventory_price;
    int inventory_amount;
} Inventory;

typedef struct Order {
    char order_id[ORDER_ID_LENGTH];
    char item_id[10][MENU_ITEM_ID_LENGTH]; // maximum 10 types of food/drinks
    int item_amount[10];
    float total_price;
    char order_date[DATE_LENGTH];
    char order_time[TIME_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
} Order;

typedef struct DailySales {
    char date[DATE_LENGTH];
    int total_orders;
    int completed_orders;
    int pending_orders;
    int cancelled_orders;
    float total_revenue;
    float completed_revenue;
    float pending_revenue;
    float average_order_value;
    char best_selling_item[NAME_LENGTH];
    int best_selling_quantity;
    float best_selling_revenue;
} DailySales;

typedef struct ItemSales {
    char item_id[MENU_ITEM_ID_LENGTH];
    char item_name[NAME_LENGTH];
    int quantity_sold;
    float unit_price;
    float total_revenue;
    int order_count;
} ItemSales;

typedef struct HourlySales {
    int hour;
    int order_count;
    float revenue;
} HourlySales;

int menu_item_management(void);
int order_queue_management(void);
int daily_sales_report(void);
int inventory_tracking(void);

#endif