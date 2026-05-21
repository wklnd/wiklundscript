// DateTime native module
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Symbol name expected by interpreter: DateTime_getCurrentDate
const char *DateTime_getCurrentDate(size_t argc, const char **argv) {
    (void)argc;
    (void)argv;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char buf[64];
    if (!tm) {
        char *empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    char *copy = malloc(strlen(buf) + 1);
    strcpy(copy, buf);
    return copy;
}

// get tomorrow's date
const char *DateTime_getTomorrow(size_t argc, const char **argv) {
    (void)argc;
    (void)argv;
    time_t t = time(NULL) + 24 * 60 * 60; // add one day in seconds
    struct tm *tm = localtime(&t);
    char buf[64];
    if (!tm) {
        char *empty = malloc(1);
        empty[0] = '\0';
        return empty;
    }
    strftime(buf, sizeof(buf), "%Y-%m-%d", tm);
    char *copy = malloc(strlen(buf) + 1);
    strcpy(copy, buf);
    return copy;
}
