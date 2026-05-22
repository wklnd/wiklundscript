// Random native module
#include "native.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

static void seed_rng(void) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
}

static int parse_int_arg(const char *text, int fallback) {
    if (!text || *text == '\0') return fallback;
    return (int)strtol(text, NULL, 10);
}

// Symbol name expected by interpreter: Random_getRandomInt
// Usage from wscript: Random.getRandomInt(min, max)
NativeValue Random_getRandomInt(size_t argc, const char **argv) {
    seed_rng();

    int min = 0;
    int max = RAND_MAX;

    if (argc >= 1) min = parse_int_arg(argv[0], 0);
    if (argc >= 2) max = parse_int_arg(argv[1], RAND_MAX);

    if (max < min) {
        int tmp = min;
        min = max;
        max = tmp;
    }

    long span = (long)max - (long)min + 1L;
    int value = min;
    if (span > 0) {
        value = min + (rand() % span);
    }

    return NATIVE_INT_VALUE(value);
}


NativeValue Random_getRandomFloat(size_t argc, const char **argv) {
    seed_rng();

    double min = 0.0;
    double max = 1.0;

    if (argc >= 1) min = atof(argv[0]);
    if (argc >= 2) max = atof(argv[1]);

    if (max < min) {
        double tmp = min;
        min = max;
        max = tmp;
    }

    double value = min + ((double)rand() / (double)RAND_MAX) * (max - min);
    return NATIVE_FLOAT_VALUE(value);
}