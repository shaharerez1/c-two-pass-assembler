#include <stdio.h>

/**
 * Reports an error via stdout, with accurate file name and line number.
 * uses ANSI escape codes for terminal colors.
 */
void announce_error(const char* file_name, const int line_number, const char* message) {
    printf("\033[1;31mERROR\033[0m : in file \033[1;35m%s\033[0m, line %d.\n", file_name, line_number);
    printf("%s\n", message);
}