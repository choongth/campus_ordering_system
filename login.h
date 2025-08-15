#ifndef LOGIN_H
#define LOGIN_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

#include "setting.h"
#include "enums.h"
#include "share.h"
#include "user.h"
#include "student.h"
#include "delivery_personnel.h"
#include "restaurant.h"
#include "database.h"
#include "admin.h"

typedef int (*TaskFunction)(void);

typedef struct MenuOption {
    int task_number;
    const char *description;
    TaskFunction function;
} MenuOption;

extern MenuOption AdminMenu[];
extern MenuOption RestaurantMenu[];
extern MenuOption StudentMenu[];
extern MenuOption DeliveryPersonnelMenu[];

int login(User *u);
int show_main_menu(User *u);

#endif