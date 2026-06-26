#include "lexer.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INDENT_MAX 256

typedef struct {
    const char *source;
    size_t      pos;
    int         line;
    int         at_line_start;
    int         indent_stack[INDENT_MAX];
    int         indent_top;
    TokenList   list;
} LexerState;

static void push_token(LexerState *s, TokenType type, const char *value, int line) {
    if (s->list.count >= s->list.capacity) {
        s->list.capacity *= 2;
        s->list.tokens = realloc(s->list.tokens, s->list.capacity * sizeof(Token));
    }
    s->list.tokens[s->list.count++] = (Token){
        .type  = type,
        .value = strdup(value),
        .line  = line,
    };
}

static void skip_comment(LexerState *s) {
    while (s->source[s->pos] != '\n' && s->source[s->pos] != '\0') {
        s->pos++;
    }
}

static void read_string(LexerState *s) {
    int line = s->line;
    s->pos++;

    size_t start = s->pos;
    while (s->source[s->pos] != '"' && s->source[s->pos] != '\0') {
        s->pos++;
    }

    if (s->source[s->pos] != '"') {
        ERROR("unterminated string", line);
    }

    size_t len = s->pos - start;
    char  *buf = malloc(len + 1);
    memcpy(buf, s->source + start, len);
    buf[len] = '\0';

    push_token(s, TOKEN_STRING, buf, line);
    free(buf);
    s->pos++;
}

static void read_number(LexerState *s) {
    int    line  = s->line;
    size_t start = s->pos;

    while (isdigit(s->source[s->pos]) || s->source[s->pos] == '.') {
        s->pos++;
    }

    size_t len = s->pos - start;
    char  *buf = malloc(len + 1);
    memcpy(buf, s->source + start, len);
    buf[len] = '\0';

    push_token(s, TOKEN_NUMBER, buf, line);
    free(buf);
}

static void read_word(LexerState *s) {
    int    line  = s->line;
    size_t start = s->pos;

    while (isalnum(s->source[s->pos]) || s->source[s->pos] == '_') {
        s->pos++;
    }

    size_t len = s->pos - start;
    char  *buf = malloc(len + 1);
    memcpy(buf, s->source + start, len);
    buf[len] = '\0';

    TokenType type = TOKEN_IDENTIFIER;
    if      (strcmp(buf, "var")    == 0) type = TOKEN_VAR;
    else if (strcmp(buf, "const")  == 0) type = TOKEN_CONST;
    else if (strcmp(buf, "print")  == 0) type = TOKEN_PRINT;
    else if (strcmp(buf, "say")    == 0) type = TOKEN_SAY;
    else if (strcmp(buf, "if")     == 0) type = TOKEN_IF;
    else if (strcmp(buf, "else")   == 0) type = TOKEN_ELSE;
    else if (strcmp(buf, "loop")   == 0) type = TOKEN_LOOP;
    else if (strcmp(buf, "as")     == 0) type = TOKEN_AS;
    else if (strcmp(buf, "while")  == 0) type = TOKEN_WHILE;
    else if (strcmp(buf, "func")   == 0) type = TOKEN_FUNC;
    else if (strcmp(buf, "object") == 0) type = TOKEN_OBJECT;
    else if (strcmp(buf, "return") == 0) type = TOKEN_RETURN;
    else if (strcmp(buf, "true")   == 0) type = TOKEN_TRUE;
    else if (strcmp(buf, "false")  == 0) type = TOKEN_FALSE;
    else if (strcmp(buf, "import") == 0) type = TOKEN_IMPORT;
    else if (strcmp(buf, "is")     == 0) type = TOKEN_IS;

    push_token(s, type, buf, line);
    free(buf);
}

TokenList lexer_tokenize(const char *source) {
    LexerState s = {
        .source        = source,
        .pos           = 0,
        .line          = 1,
        .at_line_start = 1,
        .indent_stack  = {0},
        .indent_top    = 0,
        .list          = {
            .tokens   = malloc(64 * sizeof(Token)),
            .count    = 0,
            .capacity = 64,
        },
    };

    while (s.source[s.pos] != '\0') {

        // At the start of a new line: handle indentation
        if (s.at_line_start) {
            int indent = 0;
            while (s.source[s.pos] == ' ' || s.source[s.pos] == '\t') {
                indent++;
                s.pos++;
            }

            char c = s.source[s.pos];

            // Blank line or comment — skip entirely, stay at line start
            if (c == '\n') { s.line++; s.pos++; continue; }
            if (c == '#')  { skip_comment(&s); continue; }
            if (c == '\0') break;

            // Emit INDENT or DEDENT(s) based on indent change
            int current = s.indent_stack[s.indent_top];
            if (indent > current) {
                s.indent_stack[++s.indent_top] = indent;
                push_token(&s, TOKEN_INDENT, "", s.line);
            } else {
                while (indent < s.indent_stack[s.indent_top]) {
                    s.indent_top--;
                    push_token(&s, TOKEN_DEDENT, "", s.line);
                }
            }

            s.at_line_start = 0;
            continue;
        }

        char c = s.source[s.pos];

        if (c == ' ' || c == '\t' || c == '\r') { s.pos++; continue; }

        if (c == '\n') {
            push_token(&s, TOKEN_NEWLINE, "\n", s.line);
            s.line++;
            s.pos++;
            s.at_line_start = 1;
            continue;
        }

        if (c == '#')  { skip_comment(&s); continue; }
        if (c == '"')  { read_string(&s); continue; }
        if (isdigit(c)){ read_number(&s); continue; }
        if (isalpha(c) || c == '_') { read_word(&s); continue; }

        char next = s.source[s.pos + 1];
        if (c == '=' && next == '=') { push_token(&s, TOKEN_EQ,  "==", s.line); s.pos += 2; continue; }
        if (c == '!' && next == '=') { push_token(&s, TOKEN_NEQ, "!=", s.line); s.pos += 2; continue; }
        if (c == '>' && next == '=') { push_token(&s, TOKEN_GTE, ">=", s.line); s.pos += 2; continue; }
        if (c == '<' && next == '=') { push_token(&s, TOKEN_LTE, "<=", s.line); s.pos += 2; continue; }

        s.pos++;
        switch (c) {
            case '=': push_token(&s, TOKEN_ASSIGN,   "=",  s.line); break;
            case '+': push_token(&s, TOKEN_PLUS,     "+",  s.line); break;
            case '-': push_token(&s, TOKEN_MINUS,    "-",  s.line); break;
            case '*': push_token(&s, TOKEN_STAR,     "*",  s.line); break;
            case '/': push_token(&s, TOKEN_SLASH,    "/",  s.line); break;
            case '>': push_token(&s, TOKEN_GT,       ">",  s.line); break;
            case '<': push_token(&s, TOKEN_LT,       "<",  s.line); break;
            case '(': push_token(&s, TOKEN_LPAREN,   "(",  s.line); break;
            case ')': push_token(&s, TOKEN_RPAREN,   ")",  s.line); break;
            case '[': push_token(&s, TOKEN_LBRACKET, "[",  s.line); break;
            case ']': push_token(&s, TOKEN_RBRACKET, "]",  s.line); break;
            case '.': push_token(&s, TOKEN_DOT,      ".",  s.line); break;
            case ',': push_token(&s, TOKEN_COMMA,    ",",  s.line); break;
            default:
                ERROR_F(s.line, "unexpected character '%c'", c);
        }
    }

    // Emit remaining DEDENTs at end of file
    while (s.indent_top > 0) {
        s.indent_top--;
        push_token(&s, TOKEN_DEDENT, "", s.line);
    }

    push_token(&s, TOKEN_EOF, "", s.line);
    return s.list;
}

void lexer_free(TokenList *list) {
    for (size_t i = 0; i < list->count; i++) {
        free(list->tokens[i].value);
    }
    free(list->tokens);
    list->tokens   = NULL;
    list->count    = 0;
    list->capacity = 0;
}
