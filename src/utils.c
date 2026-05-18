#include "utils.h"
#include <stdio.h>

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_header(const char* title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

void wait_for_enter(void) {
    printf("\nНажмите Enter для продолжения...");
    clear_input_buffer();
    getchar();
}