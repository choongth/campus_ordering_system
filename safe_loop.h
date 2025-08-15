#ifndef SAFE_LOOP_H
#define SAFE_LOOP_H

// Safe loop utilities to prevent infinite loops

// Loop counter structure
typedef struct {
    int max_iterations;
    int current_iteration;
    const char *loop_name;
} LoopCounter;

// Initialize loop counter
LoopCounter* init_loop_counter(const char *loop_name, int max_iterations);

// Check if loop should continue (returns 1 to continue, 0 to break)
int loop_should_continue(LoopCounter *counter);

// Free loop counter
void free_loop_counter(LoopCounter *counter);

// Safe input loop with maximum attempts
int safe_input_loop(const char *prompt, 
                   int (*input_function)(void *data), 
                   void *data, 
                   int max_attempts);

// Safe menu loop with timeout
int safe_menu_loop(const char *menu_title,
                  const char *menu_options[],
                  int option_count,
                  int (*handler)(int choice),
                  int max_iterations);

// Macros for safe loops
#define SAFE_WHILE(condition, max_iter, loop_name) \
    for (LoopCounter *_lc = init_loop_counter(loop_name, max_iter); \
         _lc && (condition) && loop_should_continue(_lc); \
         ) \
    for (int _once = 1; _once; _once = 0, free_loop_counter(_lc), _lc = NULL)

#define SAFE_INPUT_LOOP(max_attempts) \
    for (int _attempt = 0; _attempt < max_attempts; _attempt++)

// Emergency break mechanism
void emergency_loop_break(const char *reason);

#endif