#ifndef SETTING_H
#define SETTING_H

#define USER_FILE "User.txt"
#define STUDENT_FILE "Student.txt"
#define RESTAURANT_FILE "Restaurant.txt"
#define MENU_ITEM_FILE "Menu Item.txt"
#define ORDER_FILE "Order.txt"
#define QUEUE_FILE "Queue.txt" // this file is only use for order management
#define DELIVERY_PERSONNEL_FILE "Delivery Personnel.txt"
#define DELIVERY_FILE "Delivery.txt"
#define INVENTORY_FILE "Inventory.txt"
#define CONFIG_FILE "config.txt"

#define NAME_LENGTH 50
#define CONTACT_NUMBER_LENGTH 12 // max 11 digits + 1 terminator
#define EMAIL_ADDRESS_LENGTH 40
#define PASSWORD_LENGTH 21 // max 20 characters + 1 terminator
#define DEFAULT_ACCOUNT_BALANCE 00.00

#define USER_ID_LENGTH 8 // 1 prefix + 6 digits + 1 terminator, prefix = 'U', e.g:U000001
#define STUDENT_ID_LENGTH 8 // 1 prefix + 6 digits + 1 terminator, prefix = 'S', e.g:S000001
#define DELIVERY_PERSONNEL_ID_LENGTH 5 // 1 prefix + 3 digits + 1 terminator, prefix = 'P', e.g:P001
#define RESTAURANT_ID_LENGTH 4 // 1 prefix + 2 digits + 1 terminator, prefix = 'R', e.g:R01
#define MENU_ITEM_ID_LENGTH 5 // 1 prefix + 3 digits + 1 terminator, prefix = 'M', e.g:M001
#define INVENTORY_ID_LENGTH 5 // 1 prefix + 3 digits + 1 terminator, prefix = 'I'
#define ORDER_ID_LENGTH 7 // 1 prefix + 5 digits + 1 terminator, prefix = 'O'
#define DELIVERY_ID_LENGTH 7 // 1 prefix + 5 digits + 1 terminator, prefix = 'D'

#define DATE_LENGTH 11 // DD-MM-YYYY + 1 terminator
#define TIME_LENGTH 9 // HH:mm:SS + 1 terminator

#define YES 1
#define NO 2

#define FILE_ERROR -1

#define EXIT 0

#define MAX_LINE_LENGTH 256

#endif