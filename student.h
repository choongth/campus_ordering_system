#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setting.h"
#include "enums.h"
#include "user.h"
#include "share.h"
#include "order.h"
#include "safe_input.h"
#include "safe_loop.h"

typedef struct Student {
    User user;
    char student_id[STUDENT_ID_LENGTH];
    AccountActivateStatus account_activate_status;
    float account_balance;
} Student;

typedef struct OrderHistory {
    char order_id[ORDER_ID_LENGTH];
    char restaurant_id[RESTAURANT_ID_LENGTH];
    char restaurant_name[NAME_LENGTH];
    float total_price;
    char order_date[DATE_LENGTH];
    char order_time[TIME_LENGTH];
    char status[20];
    char items_summary[200];
    char delivery_id[DELIVERY_ID_LENGTH];
    char delivery_status[20];
    char estimated_arrival[TIME_LENGTH];
    char delivered_time[TIME_LENGTH];
} OrderHistory;

int profile_management(void);
int order_placement(void);
int track_order_history(void);
int account_balance_management(void);

#endif