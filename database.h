#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <string.h>

#include "user.h"
#include "setting.h"

int find_user_by_credentials(const char *email, const char *password, User *u);

#endif
