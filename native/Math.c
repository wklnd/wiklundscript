// Math native module

#include "native.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static double parse_double_arg(const char *text, double fallback) {
    if (!text || *text == '\0') {
        return fallback;
    }
    return atof(text);
}

static int parse_is_float_text(const char *text) {
    if (!text) return 0;
    return strchr(text, '.') != NULL || strchr(text, 'e') != NULL || strchr(text, 'E') != NULL;
}

static NativeValue make_numeric_result(double value, int force_float) {
    if (force_float) {
        return NATIVE_FLOAT_VALUE(value);
    }
    return NATIVE_INT_VALUE((long)value);
}

static void seed_rng(void) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
}

// Symbol name expected by interpreter: Math_floor
NativeValue Math_floor(size_t argc, const char **argv) {
    double value = parse_double_arg((argc > 0) ? argv[0] : NULL, 0.0);
    return NATIVE_INT_VALUE((long)floor(value));
}

// Symbol name expected by interpreter: Math_sqrt
NativeValue Math_sqrt(size_t argc, const char **argv) {
    double value = parse_double_arg((argc > 0) ? argv[0] : NULL, 0.0);
    return NATIVE_FLOAT_VALUE(sqrt(value));
}

// Symbol name expected by interpreter: Math_abs
NativeValue Math_abs(size_t argc, const char **argv) {
    const char *text = (argc > 0) ? argv[0] : NULL;
    double value = parse_double_arg(text, 0.0);
    double result = fabs(value);
    return make_numeric_result(result, parse_is_float_text(text));
}

// Symbol name expected by interpreter: Math_round
NativeValue Math_round(size_t argc, const char **argv) {
    double value = parse_double_arg((argc > 0) ? argv[0] : NULL, 0.0);
    return NATIVE_INT_VALUE((long)llround(value));
}

// Symbol name expected by interpreter: Math_pow
NativeValue Math_pow(size_t argc, const char **argv) {
    double base = parse_double_arg((argc > 0) ? argv[0] : NULL, 0.0);
    double exponent = parse_double_arg((argc > 1) ? argv[1] : NULL, 1.0);
    return NATIVE_FLOAT_VALUE(pow(base, exponent));
}

// Symbol name expected by interpreter: Math_min
NativeValue Math_min(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_INT_VALUE(0);
    }

    double result = parse_double_arg(argv[0], 0.0);
    int force_float = parse_is_float_text(argv[0]);

    for (size_t i = 1; i < argc; i++) {
        double value = parse_double_arg(argv[i], 0.0);
        if (value < result) {
            result = value;
        }
        if (parse_is_float_text(argv[i])) {
            force_float = 1;
        }
    }

    return make_numeric_result(result, force_float);
}

// Symbol name expected by interpreter: Math_max
NativeValue Math_max(size_t argc, const char **argv) {
    if (argc == 0) {
        return NATIVE_INT_VALUE(0);
    }

    double result = parse_double_arg(argv[0], 0.0);
    int force_float = parse_is_float_text(argv[0]);

    for (size_t i = 1; i < argc; i++) {
        double value = parse_double_arg(argv[i], 0.0);
        if (value > result) {
            result = value;
        }
        if (parse_is_float_text(argv[i])) {
            force_float = 1;
        }
    }

    return make_numeric_result(result, force_float);
}

// Symbol name expected by interpreter: Math_random
NativeValue Math_random(size_t argc, const char **argv) {
    (void)argc;
    (void)argv;

    seed_rng();

    double value = (double)rand() / (double)RAND_MAX;
    return NATIVE_FLOAT_VALUE(value);
}