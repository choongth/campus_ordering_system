#include "safe_loop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize loop counter
LoopCounter* init_loop_counter(const char *loop_name, int max_iterations) {
    LoopCounter *counter = malloc(sizeof(LoopCounter));
    if (!counter) {
        return NULL;
    }
    
    counter->max_iterations = max_iterations;
    counter->current_iteration = 0;
    counter->loop_name = loop_name ? loop_name : "unnamed_loop";
    
    return counter;
}

// Check if loop should continue
int loop_should_continue(LoopCounter *counter) {
    if (!counter) {
        return 0;
    }
    
    counter->current_iteration++;
    
    if (counter->current_iteration >= counter->max_iterations) {
        printf("Warning: Loop '%s' reached maximum iterations (%d). Breaking to prevent infinite loop.\n", 
               counter->loop_name, counter->max_iterations);
        return 0;
    }
    
    return 1;
}

// Free loop counter
void free_loop_counter(LoopCounter *counter) {
    if (counter) {
        free(counter);
    }
}

// Safe input loop with maximum attempts
int safe_input_loop(const char *prompt, 
                   int (*input_function)(void *data), 
                   void *data, 
                   int max_attempts) {
    if (!prompt || !input_function || max_attempts <= 0) {
        return 0;
    }
    
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        printf("%s", prompt);
        
        if (input_function(data)) {
            return 1; // Success
        }
        
        if (attempt < max_attempts - 1) {
            printf("Invalid input. Please try again. (%d/%d attempts)\n", 
                   attempt + 1, max_attempts);
        }
    }
    
    printf("Maximum input attempts (%d) exceeded. Operation cancelled.\n", max_attempts);
    return 0;
}

// Safe menu loop with timeout
int safe_menu_loop(const char *menu_title,
                  const char *menu_options[],
                  int option_count,
                  int (*handler)(int choice),
                  int max_iterations) {
    if (!menu_title || !menu_options || !handler || option_count <= 0) {
        return 0;
    }
    
    LoopCounter *counter = init_loop_counter("menu_loop", max_iterations);
    if (!counter) {
        return 0;
    }
    
    while (loop_should_continue(counter)) {
        // Display menu
        printf("\n%s\n", menu_title);
        for (int i = 0; i < option_count; i++) {
            printf("%d. %s\n", i + 1, menu_options[i]);
        }
        printf("0. Exit\n");
        
        // Get user choice
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1) {
            // Clear invalid input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        
        // Handle choice
        if (choice == 0) {
            free_loop_counter(counter);
            return 0; // User chose to exit
        }
        
        if (choice >= 1 && choice <= option_count) {
            int result = handler(choice);
            if (result == -1) {
                // Handler requested exit
                free_loop_counter(counter);
                return -1;
            }
        } else {
            printf("Invalid choice. Please select a valid option.\n");
        }
    }
    
    free_loop_counter(counter);
    return 0;
}

// Emergency break mechanism
void emergency_loop_break(const char *reason) {
    printf("EMERGENCY LOOP BREAK: %s\n", reason ? reason : "Unknown reason");
    printf("System is preventing potential infinite loop.\n");
    printf("Please contact system administrator if this persists.\n");
}