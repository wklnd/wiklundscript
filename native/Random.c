// Random native module
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
const char *Random_getRandomInt(size_t argc, const char **argv) {
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

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    char *copy = malloc(strlen(buf) + 1);
    strcpy(copy, buf);
    return copy;
}
