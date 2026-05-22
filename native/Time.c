// Time native module

#include "native.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

static NativeValue make_date_value(time_t t) {
    struct tm *tm = localtime(&t);
    char buf[64];

    if (!tm) {
        char *empty = malloc(1);
        if (!empty) {
            return NATIVE_STRING_VALUE(NULL);
        }
        empty[0] = '\0';
        return NATIVE_STRING_VALUE(empty);
    }

    strftime(buf, sizeof(buf), "%Y-%m-%d", tm);

    char *copy = malloc(strlen(buf) + 1);
    if (!copy) {
        return NATIVE_STRING_VALUE(NULL);
    }

    strcpy(copy, buf);
    return NATIVE_STRING_VALUE(copy);
}

static void sleep_for_seconds(double seconds) {
    if (seconds <= 0.0) {
        return;
    }

    struct timespec req;
    req.tv_sec = (time_t)seconds;
    req.tv_nsec = (long)((seconds - (double)req.tv_sec) * 1000000000.0);

    if (req.tv_nsec < 0) {
        req.tv_nsec = 0;
    }

    nanosleep(&req, NULL);
}

// Symbol name expected by interpreter: Time_getDate
NativeValue Time_getDate(size_t argc, const char **argv) {
    (void)argc;
    (void)argv;
    return make_date_value(time(NULL));
}

// Symbol name expected by interpreter: Time_getTomorrow
NativeValue Time_getTomorrow(size_t argc, const char **argv) {
    (void)argc;
    (void)argv;
    return make_date_value(time(NULL) + 24 * 60 * 60);
}

// Symbol name expected by interpreter: Time_sleep
NativeValue Time_sleep(size_t argc, const char **argv) {
    double seconds = (argc > 0 && argv[0]) ? atof(argv[0]) : 0.0;
    sleep_for_seconds(seconds);
    return NATIVE_INT_VALUE(0);
}