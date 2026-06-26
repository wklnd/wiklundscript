// Color native module

#include "native.h"

#include <stdlib.h>
#include <string.h>

static char *dup_text(const char *text) {
	const char *source = text ? text : "";
	char *copy = malloc(strlen(source) + 1);

	if (!copy) {
		return NULL;
	}

	strcpy(copy, source);
	return copy;
}

static char *wrap_ansi(const char *text, const char *prefix) {
	const char *source = text ? text : "";
	const char *suffix = "\033[0m";
	size_t len = strlen(prefix) + strlen(source) + strlen(suffix);
	char *result = malloc(len + 1);

	if (!result) {
		return NULL;
	}

	result[0] = '\0';
	strcat(result, prefix);
	strcat(result, source);
	strcat(result, suffix);
	return result;
}

static NativeValue make_colored(const char *text, const char *prefix) {
	return NATIVE_STRING_VALUE(wrap_ansi(text, prefix));
}

static NativeValue make_plain(const char *text) {
	return NATIVE_STRING_VALUE(dup_text(text));
}

// Symbol name expected by interpreter: Color_colorize
NativeValue Color_colorize(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	const char *name = (argc > 1) ? argv[1] : "";

	if (strcmp(name, "black") == 0) return make_colored(text, "\033[30m");
	if (strcmp(name, "red") == 0) return make_colored(text, "\033[31m");
	if (strcmp(name, "green") == 0) return make_colored(text, "\033[32m");
	if (strcmp(name, "yellow") == 0) return make_colored(text, "\033[33m");
	if (strcmp(name, "blue") == 0) return make_colored(text, "\033[34m");
	if (strcmp(name, "magenta") == 0) return make_colored(text, "\033[35m");
	if (strcmp(name, "cyan") == 0) return make_colored(text, "\033[36m");
	if (strcmp(name, "white") == 0) return make_colored(text, "\033[37m");
	if (strcmp(name, "bold") == 0) return make_colored(text, "\033[1m");
	if (strcmp(name, "dim") == 0) return make_colored(text, "\033[2m");
	if (strcmp(name, "underline") == 0) return make_colored(text, "\033[4m");

	return make_plain(text);
}

// Symbol name expected by interpreter: Color_black
NativeValue Color_black(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[30m");
}

// Symbol name expected by interpreter: Color_red
NativeValue Color_red(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[31m");
}

// Symbol name expected by interpreter: Color_green
NativeValue Color_green(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[32m");
}

// Symbol name expected by interpreter: Color_yellow
NativeValue Color_yellow(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[33m");
}

// Symbol name expected by interpreter: Color_blue
NativeValue Color_blue(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[34m");
}

// Symbol name expected by interpreter: Color_magenta
NativeValue Color_magenta(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[35m");
}

// Symbol name expected by interpreter: Color_cyan
NativeValue Color_cyan(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[36m");
}

// Symbol name expected by interpreter: Color_white
NativeValue Color_white(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[37m");
}

// Symbol name expected by interpreter: Color_bold
NativeValue Color_bold(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[1m");
}

// Symbol name expected by interpreter: Color_dim
NativeValue Color_dim(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[2m");
}

// Symbol name expected by interpreter: Color_underline
NativeValue Color_underline(size_t argc, const char **argv) {
	const char *text = (argc > 0) ? argv[0] : "";
	return make_colored(text, "\033[4m");
}

// Symbol name expected by interpreter: Color_reset
NativeValue Color_reset(size_t argc, const char **argv) {
	(void)argc;
	(void)argv;
	return NATIVE_STRING_VALUE(dup_text("\033[0m"));
}

NativeValue color_colorize(size_t argc, const char **argv) {
	return Color_colorize(argc, argv);
}

NativeValue color_black(size_t argc, const char **argv) {
	return Color_black(argc, argv);
}

NativeValue color_red(size_t argc, const char **argv) {
	return Color_red(argc, argv);
}

NativeValue color_green(size_t argc, const char **argv) {
	return Color_green(argc, argv);
}

NativeValue color_yellow(size_t argc, const char **argv) {
	return Color_yellow(argc, argv);
}

NativeValue color_blue(size_t argc, const char **argv) {
	return Color_blue(argc, argv);
}

NativeValue color_magenta(size_t argc, const char **argv) {
	return Color_magenta(argc, argv);
}

NativeValue color_cyan(size_t argc, const char **argv) {
	return Color_cyan(argc, argv);
}

NativeValue color_white(size_t argc, const char **argv) {
	return Color_white(argc, argv);
}

NativeValue color_bold(size_t argc, const char **argv) {
	return Color_bold(argc, argv);
}

NativeValue color_dim(size_t argc, const char **argv) {
	return Color_dim(argc, argv);
}

NativeValue color_underline(size_t argc, const char **argv) {
	return Color_underline(argc, argv);
}

NativeValue color_reset(size_t argc, const char **argv) {
	return Color_reset(argc, argv);
}
