#ifndef SHARE_H
#define SHARE_H

#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#include "setting.h"
#include "enums.h"
#include "safe_loop.h"
#include "safe_input.h"

int logout();
int confirmation(const char *prompt);

int ask_exit();
int get_integer_input(const char *prompt);
bool get_float_input(const char *str);
void clear_input_buffer();

#endif