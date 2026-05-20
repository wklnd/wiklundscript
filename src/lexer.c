#include "lexer.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//  Internal State 

typedef struct {
    const char *source;
    size_t      pos;
    int         line;
    TokenList   list;
} LexerState;

//  Helpers 

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

static char peek(LexerState *s) {
    return s->source[s->pos];
}

static char peek_next(LexerState *s) {
    return s->source[s->pos + 1];
}

static char advance(LexerState *s) {
    return s->source[s->pos++];
}

static void skip_spaces(LexerState *s) {
    while (peek(s) == ' ' || peek(s) == '\t' || peek(s) == '\r') {
        s->pos++;
    }
}

static void skip_comment(LexerState *s) {
    while (peek(s) != '\n' && peek(s) != '\0') {
        s->pos++;
    }
}

//  Readers 

static void read_string(LexerState *s) {
    int line = s->line;
    s->pos++; // skip opening "

    size_t start = s->pos;
    while (peek(s) != '"' && peek(s) != '\0') {
        s->pos++;
    }

    if (peek(s) != '"') {
        ERROR("unterminated string", line);
    }

    size_t len = s->pos - start;
    char  *buf = malloc(len + 1);
    memcpy(buf, s->source + start, len);
    buf[len] = '\0';

    push_token(s, TOKEN_STRING, buf, line);
    free(buf);
    s->pos++; // skip closing "
}

static void read_number(LexerState *s) {
    int    line  = s->line;
    size_t start = s->pos;

    while (isdigit(peek(s)) || peek(s) == '.') {
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

    while (isalnum(peek(s)) || peek(s) == '_') {
        s->pos++;
    }

    size_t len = s->pos - start;
    char  *buf = malloc(len + 1);
    memcpy(buf, s->source + start, len);
    buf[len] = '\0';

    // Keyword table
    TokenType type = TOKEN_IDENTIFIER;
    if      (strcmp(buf, "var")    == 0) type = TOKEN_VAR;
    else if (strcmp(buf, "const")  == 0) type = TOKEN_CONST;
    else if (strcmp(buf, "print")  == 0) type = TOKEN_PRINT;
    else if (strcmp(buf, "if")     == 0) type = TOKEN_IF;
    else if (strcmp(buf, "else")   == 0) type = TOKEN_ELSE;
    else if (strcmp(buf, "end")    == 0) type = TOKEN_END;
    else if (strcmp(buf, "repeat") == 0) type = TOKEN_REPEAT;
    else if (strcmp(buf, "times")  == 0) type = TOKEN_TIMES;
    else if (strcmp(buf, "as")     == 0) type = TOKEN_AS;
    else if (strcmp(buf, "while")  == 0) type = TOKEN_WHILE;
    else if (strcmp(buf, "func")   == 0) type = TOKEN_FUNC;
    else if (strcmp(buf, "object") == 0) type = TOKEN_OBJECT;
    else if (strcmp(buf, "return") == 0) type = TOKEN_RETURN;
    else if (strcmp(buf, "true")   == 0) type = TOKEN_TRUE;
    else if (strcmp(buf, "false")  == 0) type = TOKEN_FALSE;

    push_token(s, type, buf, line);
    free(buf);
}

//  Main Tokenizer 

TokenList lexer_tokenize(const char *source) {
    LexerState s = {
        .source = source,
        .pos    = 0,
        .line   = 1,
        .list   = {
            .tokens   = malloc(64 * sizeof(Token)),
            .count    = 0,
            .capacity = 64,
        },
    };

    while (peek(&s) != '\0') {
        skip_spaces(&s);
        if (peek(&s) == '\0') break;

        char c = peek(&s);

        if (c == '#')  { skip_comment(&s); continue; }
        if (c == '\n') { push_token(&s, TOKEN_NEWLINE, "\n", s.line); s.line++; s.pos++; continue; }
        if (c == '"')  { read_string(&s); continue; }
        if (isdigit(c)){ read_number(&s); continue; }
        if (isalpha(c) || c == '_') { read_word(&s); continue; }

        // Two-character operators
        char next = peek_next(&s);
        if (c == '=' && next == '=') { push_token(&s, TOKEN_EQ,  "==", s.line); s.pos += 2; continue; }
        if (c == '!' && next == '=') { push_token(&s, TOKEN_NEQ, "!=", s.line); s.pos += 2; continue; }
        if (c == '>' && next == '=') { push_token(&s, TOKEN_GTE, ">=", s.line); s.pos += 2; continue; }
        if (c == '<' && next == '=') { push_token(&s, TOKEN_LTE, "<=", s.line); s.pos += 2; continue; }

        // Single-character operators
        advance(&s);
        switch (c) {
            case '=': push_token(&s, TOKEN_ASSIGN, "=",  s.line); break;
            case '+': push_token(&s, TOKEN_PLUS,   "+",  s.line); break;
            case '-': push_token(&s, TOKEN_MINUS,  "-",  s.line); break;
            case '*': push_token(&s, TOKEN_STAR,   "*",  s.line); break;
            case '/': push_token(&s, TOKEN_SLASH,  "/",  s.line); break;
            case '>': push_token(&s, TOKEN_GT,     ">",  s.line); break;
            case '<': push_token(&s, TOKEN_LT,     "<",  s.line); break;
            case '(': push_token(&s, TOKEN_LPAREN, "(",  s.line); break;
            case ')': push_token(&s, TOKEN_RPAREN, ")",  s.line); break;
            case '{': push_token(&s, TOKEN_LBRACE, "{",  s.line); break;
            case '}': push_token(&s, TOKEN_RBRACE, "}",  s.line); break;
            case '.': push_token(&s, TOKEN_DOT,    ".",  s.line); break;
            case ',': push_token(&s, TOKEN_COMMA,  ",",  s.line); break;
            default:
                ERROR_F(s.line, "unexpected character '%c'", c);
        }
    }

    push_token(&s, TOKEN_EOF, "", s.line);
    return s.list;
}

void lexer_free(TokenList *list) {
    for (size_t i = 0; i < list->count; i++) {
        free(list->tokens[i].value);
    }
    free(list->tokens);
    list->tokens  = NULL;
    list->count   = 0;
    list->capacity = 0;
}