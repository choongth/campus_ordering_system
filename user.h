#ifndef USER_H
#define USER_H

#include "setting.h"
#include "enums.h"

typedef struct User {
    char id[USER_ID_LENGTH];
    UserRole role;
    char name[NAME_LENGTH];
    char contact_number[CONTACT_NUMBER_LENGTH];
    char email_address[EMAIL_ADDRESS_LENGTH];
    char password[PASSWORD_LENGTH];
} User;

#endif