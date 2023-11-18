#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char *op;
    op = malloc(8 * sizeof(char));
    while (true) {
        if (scanf("%s", op) != 1) return 1;
        if (!strncmp(op, "quit", 4)) {
            free(op);
            break;
        }
        else if (!strncmp(op, "insert", 6)) {
            printf("%s\n", op);
        }
        else if (!strncmp(op, "delete", 6)) {
            printf("%s\n", op);
        }
        else if (!strncmp(op, "decrease", 8)) {
            printf("%s\n", op);
        }
        else if (!strncmp(op, "extract", 7)) {
            printf("%s\n", op);
        }
        else {
            printf("Invalid instruction. Please try again, or enter 'quit' to exit.\n");
        }
    }
    return 0;
}
