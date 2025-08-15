#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>

#include "setting.h"
#include "enums.h"
#include "share.h"
#include "user.h"
#include "student.h"
#include "restaurant.h"
#include "delivery_personnel.h"
#include "admin.h"
#include "login.h"
#include "registration.h"


int main() {
    User u;

    printf("\nWelcome to campus ordering system!\n");
    
    while (1) {
        for (int ask = 0; ask < 3; ask++) {
            int have_account = get_integer_input("\nDo you have an account? (1. Yes / 2. No): ");
            switch (have_account) {
                case YES:
                    if (login(&u)) {
                        int menu_result = show_main_menu(&u);
                        if (menu_result == -1) {
                            // User logged out, continue to ask again
                            goto ask_again;
                        }
                        return 0;
                    }
                    break;
                case NO:
                    if (registration(&u)) {
                        if (login(&u)) {
                            int menu_result = show_main_menu(&u);
                            if (menu_result == -1) {
                                // User logged out, continue to ask again
                                goto ask_again;
                            }
                            return 0;
                        }
                    }
                    break;
                default:
                    printf("\nInvalid input! Please enter a valid choice.");
            }
        }
        printf("Too many invalid attempts. Please try again.\n");
        ask_again:;
    }

    return -1;
}