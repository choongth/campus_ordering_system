#ifndef ENUMS_H
#define ENUMS_H

typedef enum UserRole {
    ADMIN = 0,
    STUDENT = 1,
    RESTAURANT = 2,
    DELIVERY_PERSONNEL = 3
} UserRole;

typedef enum AccountActivateStatus {
    ACTIVATE = 1,
    DEACTIVATE = 2
} AccountActivateStatus;

typedef enum DeliveryPersonnelStatus {
    DELIVERING_ORDER = 1,
    NO_ORDER_ASSIGNED = 2
} DeliveryPersonnelStatus;

typedef enum DeliveryStatus {
    DELIVERED = 1,
    DELIVERING = 2,
    UNDELIVERED = 3
} DeliveryStatus;

typedef enum PunctualityStatus{
    LATE = 1,
    ON_TIME = 2
} PunctualityStatus;

typedef enum CuisineType {
    CHINESE = 1,
    WESTERN = 2,
    ARABIC = 3,
    JAPANESE = 4,
    KOREAN = 5,
    ASIAN = 6,
    INDIAN = 7,
    MALAY = 8,
    AFRICAN = 9,
    FAST_FOOD = 10,
    DESSERTS = 11
} CuisineType;

#endif