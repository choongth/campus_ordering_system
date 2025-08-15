#include "database.h"

int find_user_by_credentials(const char *email, const char *password, User *u) {
    FILE *fp = fopen(USER_FILE, "r");
    if (fp == NULL) {
        printf("Error: Could not open '%s' file.\n", USER_FILE);
        return FILE_ERROR;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        sscanf(line, "%[^,],%d,%[^,],%[^,],%[^,],%s", 
               u->id, (int*)&u->role, u->name, u->contact_number, 
               u->email_address, u->password);

        if (strcmp(email, u->email_address) == 0 && 
            strcmp(password, u->password) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}