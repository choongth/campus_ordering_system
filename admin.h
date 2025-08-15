#ifndef ADMIN_H
#define ADMIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "share.h"
#include "user.h"
#include "student.h"
#include "restaurant.h"
#include "delivery_personnel.h"
#include "setting.h"
#include "enums.h"
#include "safe_input.h"
#include "setting.h"
#include "enums.h"
#include "user.h"

typedef struct UserDetail {
    char user_id[USER_ID_LENGTH];
    int user_type; // 0=Admin, 1=Student, 2=Restaurant, 3=Delivery Personnel
    char name[NAME_LENGTH];
    char contact[CONTACT_NUMBER_LENGTH];
    char email[EMAIL_ADDRESS_LENGTH];
    char password[PASSWORD_LENGTH];
    int status; // For students and delivery personnel
    float balance; // For students
    int cuisine; // For restaurants
    char delivery_id[DELIVERY_ID_LENGTH]; // For delivery personnel
} UserDetail;

typedef struct SystemAnalytics {
    // User Statistics
    int total_students;
    int active_students;
    int total_restaurants;
    int active_restaurants;
    int total_delivery_personnel;
    int active_delivery_personnel;
    
    // Order Statistics
    int total_orders;
    int completed_orders;
    int pending_orders;
    int cancelled_orders;
    float completion_rate;
    float cancellation_rate;
    
    // Revenue Analytics
    float total_system_revenue;
    float average_order_value;
    float total_delivery_fees;
    float restaurant_commissions;
    
    // Performance Metrics
    float average_delivery_time;
    float on_time_delivery_rate;
    int peak_hour;
    float peak_hour_revenue;
    
    // Growth Metrics
    int new_users_today;
    int orders_today;
    float revenue_today;
} SystemAnalytics;

typedef struct UserActivityReport {
    char user_type[20];
    int total_users;
    int active_users;
    float activity_rate;
    char most_active_user[50];
    int most_active_count;
} UserActivityReport;

typedef struct RevenueBreakdown {
    char category[30];
    float amount;
    float percentage;
} RevenueBreakdown;

typedef struct PerformanceMetric {
    char metric_name[50];
    float current_value;
    float target_value;
    char status[20];
    char trend[20];
} PerformanceMetric;

typedef struct BackupInfo {
    char backup_name[100];
    char backup_date[DATE_LENGTH];
    char backup_time[TIME_LENGTH];
    int file_count;
    long total_size;
    char status[20];
} BackupInfo;

typedef struct DataFile {
    char filename[100];
    char filepath[150];
    long file_size;
    int exists;
} DataFile;

typedef struct SystemConfig {
    // System Settings
    int max_concurrent_orders;
    int max_delivery_time_minutes;
    float delivery_base_fee_rate;
    float delivery_bonus_rate;
    float delivery_penalty_rate;
    
    // Security Settings
    int max_login_attempts;
    int session_timeout_minutes;
    int password_min_length;
    int password_require_special_chars;
    
    // Business Settings
    float student_default_balance;
    int order_queue_max_size;
    float restaurant_commission_rate;
    int delivery_personnel_max_orders;

} SystemConfig;

// Global configuration instance declaration
extern SystemConfig system_config;

int user_management(void);
int reporting_and_analytics(void);
int data_backup_and_restore_functionality(void);
int system_configuration_management(void);

#endif