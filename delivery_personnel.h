#ifndef DELIVERY_PERSONNEL_H
#define DELIVERY_PERSONNEL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "setting.h"
#include "enums.h"
#include "delivery_personnel.h"
#include "order.h"
#include "share.h"
#include "safe_input.h"
#include "setting.h"
#include "user.h"

typedef struct DeliveryPersonnel {
    User user;
    char delivery_personnel_id[DELIVERY_PERSONNEL_ID_LENGTH];
    DeliveryPersonnelStatus status;
    char delivery_id[DELIVERY_ID_LENGTH];
} DeliveryPersonnel;

typedef struct Delivery {
    char delivery_id[DELIVERY_ID_LENGTH]; // 1 prefix + 5 digits + 1 terminator
    char order_id[ORDER_ID_LENGTH];
    char delivery_date[DATE_LENGTH];
    char estimated_time_arrival[TIME_LENGTH];
    char delivered_time[TIME_LENGTH];
    DeliveryStatus delivery_status;
    PunctualityStatus punctuality_status;
} Delivery;

typedef struct DeliveryStatusInfo {
    char delivery_id[DELIVERY_ID_LENGTH];
    char order_id[ORDER_ID_LENGTH];
    char delivery_date[DATE_LENGTH];
    char estimated_time_arrival[TIME_LENGTH];
    char delivered_time[TIME_LENGTH];
    DeliveryStatus delivery_status;
    PunctualityStatus punctuality_status;
    char restaurant_name[NAME_LENGTH];
    char student_name[NAME_LENGTH];
    char items_summary[200];
    float order_total;
} DeliveryStatusInfo;

typedef struct EarningsRecord {
    char delivery_id[DELIVERY_ID_LENGTH];
    char order_id[ORDER_ID_LENGTH];
    char delivery_date[DATE_LENGTH];
    char delivered_time[TIME_LENGTH];
    float order_total;
    float base_fee;
    float bonus;
    float penalty;
    float net_earnings;
    char restaurant_name[NAME_LENGTH];
    char items_summary[200];
    PunctualityStatus punctuality;
} EarningsRecord;

typedef struct EarningsSummary {
    int total_deliveries;
    int on_time_deliveries;
    int late_deliveries;
    float total_earnings;
    float total_base_fees;
    float total_bonuses;
    float total_penalties;
    float average_earnings_per_delivery;
    float on_time_percentage;
} EarningsSummary;

typedef struct PerformanceMetrics {
    int total_deliveries;
    int successful_deliveries;
    int on_time_deliveries;
    int late_deliveries;
    float success_rate;
    float on_time_rate;
    float average_delivery_time;
    float total_earnings;
    float average_earnings_per_delivery;
    char best_performance_date[DATE_LENGTH];
    int best_day_deliveries;
    char performance_grade;
    int current_streak;
    int best_streak;
} PerformanceMetrics;

typedef struct DeliveryTimeRecord {
    char delivery_id[DELIVERY_ID_LENGTH];
    char order_id[ORDER_ID_LENGTH];
    char delivery_date[DATE_LENGTH];
    char order_time[TIME_LENGTH];
    char delivered_time[TIME_LENGTH];
    int delivery_minutes;
    PunctualityStatus punctuality;
    float earnings;
} DeliveryTimeRecord;

int order_assignment(void);
int delivery_status_management(void);
int earnings_calculation_and_tracking(void);
int performance_statistics(void);

#endif