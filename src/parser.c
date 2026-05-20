// src/parser.c
#include "parser.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  Parser State 

typedef struct {
    TokenList tokens;
    size_t    pos;
} Parser;

//  Utilities 

static Token *peek(Parser *p) {
    return &p->tokens.tokens[p->pos];
}

static Token *advance(Parser *p) {
    return &p->tokens.tokens[p->pos++];
}

static int check(Parser *p, TokenType type) {
    return peek(p)->type == type;
}

static int is_at_end(Parser *p) {
    return peek(p)->type == TOKEN_EOF;
}

static Token *expect(Parser *p, TokenType type, const char *msg) {
    if (!check(p, type)) {
        ERROR_F(peek(p)->line, "%s (got '%s')", msg, peek(p)->value);
    }
    return advance(p);
}

static void skip_newlines(Parser *p) {
    while (check(p, TOKEN_NEWLINE)) advance(p);
}

static TypeName parse_type_name(Parser *p) {
    Token *t = expect(p, TOKEN_IDENTIFIER, "expected type name");
    if (strcmp(t->value, "String") == 0) return TYPE_STRING;
    if (strcmp(t->value, "Int")    == 0) return TYPE_INT;
    if (strcmp(t->value, "Float")  == 0) return TYPE_FLOAT;
    if (strcmp(t->value, "Bool")   == 0) return TYPE_BOOL;
    ERROR_F(t->line, "unknown type '%s'", t->value);
    return TYPE_UNKNOWN;
}

//  Dynamic Arrays 

typedef struct {
    Stmt  **items;
    size_t  count;
    size_t  capacity;
} StmtList;

static void stmtlist_init(StmtList *sl) {
    sl->items    = malloc(8 * sizeof(Stmt *));
    sl->count    = 0;
    sl->capacity = 8;
}

static void stmtlist_push(StmtList *sl, Stmt *s) {
    if (sl->count >= sl->capacity) {
        sl->capacity *= 2;
        sl->items = realloc(sl->items, sl->capacity * sizeof(Stmt *));
    }
    sl->items[sl->count++] = s;
}

typedef struct {
    Expr  **items;
    size_t  count;
    size_t  capacity;
} ExprList;

static void exprlist_init(ExprList *el) {
    el->items    = malloc(4 * sizeof(Expr *));
    el->count    = 0;
    el->capacity = 4;
}

static void exprlist_push(ExprList *el, Expr *e) {
    if (el->count >= el->capacity) {
        el->capacity *= 2;
        el->items = realloc(el->items, el->capacity * sizeof(Expr *));
    }
    el->items[el->count++] = e;
}

//  Forward Declarations 

static Stmt *parse_statement(Parser *p);
static Expr *parse_expression(Parser *p);

//  Block Parsing 

static StmtList parse_block(Parser *p) {
    StmtList sl;
    stmtlist_init(&sl);
    skip_newlines(p);
    while (!is_at_end(p) && !check(p, TOKEN_END)) {
        stmtlist_push(&sl, parse_statement(p));
        skip_newlines(p);
    }
    expect(p, TOKEN_END, "expected 'end'");
    return sl;
}

//  Argument List ────────────────────────────────────────────────────────────

static ExprList parse_arg_list(Parser *p) {
    ExprList el;
    exprlist_init(&el);
    while (!check(p, TOKEN_RPAREN) && !is_at_end(p)) {
        exprlist_push(&el, parse_expression(p));
        if (check(p, TOKEN_COMMA)) advance(p);
    }
    return el;
}

//  Expression Parsers ───────────────────────────────────────────────────────

static Expr *make_expr(ExprKind kind) {
    Expr *e = calloc(1, sizeof(Expr));
    e->kind = kind;
    return e;
}

static Expr *parse_primary(Parser *p) {
    Token *t = peek(p);

    // String literal
    if (t->type == TOKEN_STRING) {
        advance(p);
        Expr *e = make_expr(EXPR_LITERAL);
        e->literal.value = strdup(t->value);
        e->literal.type  = TYPE_STRING;
        return e;
    }

    // Number literal
    if (t->type == TOKEN_NUMBER) {
        advance(p);
        Expr *e = make_expr(EXPR_LITERAL);
        e->literal.value = strdup(t->value);
        e->literal.type  = strchr(t->value, '.') ? TYPE_FLOAT : TYPE_INT;
        return e;
    }

    // Boolean literals
    if (t->type == TOKEN_TRUE) {
        advance(p);
        Expr *e = make_expr(EXPR_LITERAL);
        e->literal.value = strdup("true");
        e->literal.type  = TYPE_BOOL;
        return e;
    }

    if (t->type == TOKEN_FALSE) {
        advance(p);
        Expr *e = make_expr(EXPR_LITERAL);
        e->literal.value = strdup("false");
        e->literal.type  = TYPE_BOOL;
        return e;
    }

    // Identifier, member access, or function call
    if (t->type == TOKEN_IDENTIFIER) {
        advance(p);

        // member access: name.field
        if (check(p, TOKEN_DOT)) {
            advance(p);
            Token *field = expect(p, TOKEN_IDENTIFIER, "expected field name");
            Expr  *e     = make_expr(EXPR_MEMBER);
            e->member.object = strdup(t->value);
            e->member.field  = strdup(field->value);
            return e;
        }

        // function call: name(args)
        if (check(p, TOKEN_LPAREN)) {
            advance(p);
            ExprList el = parse_arg_list(p);
            expect(p, TOKEN_RPAREN, "expected ')'");
            Expr *e = make_expr(EXPR_CALL);
            e->call.name      = strdup(t->value);
            e->call.args      = el.items;
            e->call.arg_count = el.count;
            return e;
        }

        // plain variable
        Expr *e = make_expr(EXPR_VARIABLE);
        e->variable.name = strdup(t->value);
        return e;
    }

    // Grouped expression: (expr)
    if (t->type == TOKEN_LPAREN) {
        advance(p);
        Expr *e = parse_expression(p);
        expect(p, TOKEN_RPAREN, "expected ')'");
        return e;
    }

    ERROR_F(t->line, "unexpected token '%s'", t->value);
    return NULL;
}

static Expr *parse_muldiv(Parser *p) {
    Expr *left = parse_primary(p);

    while (check(p, TOKEN_STAR) || check(p, TOKEN_SLASH)) {
        Token *op    = advance(p);
        Expr  *right = parse_primary(p);
        Expr  *e     = make_expr(EXPR_BINARY);
        e->binary.left  = left;
        e->binary.op    = strdup(op->value);
        e->binary.right = right;
        left = e;
    }

    return left;
}

static Expr *parse_addsub(Parser *p) {
    Expr *left = parse_muldiv(p);

    while (check(p, TOKEN_PLUS) || check(p, TOKEN_MINUS)) {
        Token *op    = advance(p);
        Expr  *right = parse_muldiv(p);
        Expr  *e     = make_expr(EXPR_BINARY);
        e->binary.left  = left;
        e->binary.op    = strdup(op->value);
        e->binary.right = right;
        left = e;
    }

    return left;
}

static Expr *parse_comparison(Parser *p) {
    Expr *left = parse_addsub(p);

    while (check(p, TOKEN_EQ)  || check(p, TOKEN_NEQ) ||
           check(p, TOKEN_GT)  || check(p, TOKEN_LT)  ||
           check(p, TOKEN_GTE) || check(p, TOKEN_LTE)) {
        Token *op    = advance(p);
        Expr  *right = parse_addsub(p);
        Expr  *e     = make_expr(EXPR_BINARY);
        e->binary.left  = left;
        e->binary.op    = strdup(op->value);
        e->binary.right = right;
        left = e;
    }

    return left;
}

static Expr *parse_expression(Parser *p) {
    return parse_comparison(p);
}

//  Statement Parsers 

static Stmt *make_stmt(StmtKind kind, int line) {
    Stmt *s = calloc(1, sizeof(Stmt));
    s->kind = kind;
    s->line = line;
    return s;
}

static Stmt *parse_var_decl(Parser *p) {
    int   line    = peek(p)->line;
    int   mutable = advance(p)->type == TOKEN_VAR; // VAR=1, CONST=0
    Token *name   = expect(p, TOKEN_IDENTIFIER, "expected variable name");
    expect(p, TOKEN_ASSIGN, "expected '='");
    Expr  *value  = parse_expression(p);

    Stmt *s = make_stmt(STMT_VAR_DECL, line);
    s->var_decl.name    = strdup(name->value);
    s->var_decl.value   = value;
    s->var_decl.is_mutable = mutable;
    return s;
}

static Stmt *parse_print(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'print'
    expect(p, TOKEN_LPAREN, "expected '('");
    Expr *value = parse_expression(p);
    expect(p, TOKEN_RPAREN, "expected ')'");

    Stmt *s = make_stmt(STMT_PRINT, line);
    s->print.value = value;
    return s;
}

static Stmt *parse_if(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'if'
    Expr *condition = parse_expression(p);
    skip_newlines(p);

    StmtList body, else_body;
    stmtlist_init(&body);
    stmtlist_init(&else_body);
    int in_else = 0;

    while (!is_at_end(p) && !check(p, TOKEN_END)) {
        if (check(p, TOKEN_ELSE)) {
            advance(p);
            skip_newlines(p);
            in_else = 1;
            continue;
        }
        if (in_else) {
            stmtlist_push(&else_body, parse_statement(p));
        } else {
            stmtlist_push(&body, parse_statement(p));
        }
        skip_newlines(p);
    }
    expect(p, TOKEN_END, "expected 'end'");

    Stmt *s = make_stmt(STMT_IF, line);
    s->if_stmt.condition  = condition;
    s->if_stmt.body       = body.items;
    s->if_stmt.body_count = body.count;
    s->if_stmt.else_body  = else_body.items;
    s->if_stmt.else_count = else_body.count;
    return s;
}

static Stmt *parse_repeat(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'repeat'

    // repeat while <condition>
    if (check(p, TOKEN_WHILE)) {
        advance(p);
        Expr     *condition = parse_expression(p);
        skip_newlines(p);
        StmtList  sl        = parse_block(p);

        Stmt *s = make_stmt(STMT_REPEAT_WHILE, line);
        s->repeat_while.condition  = condition;
        s->repeat_while.body       = sl.items;
        s->repeat_while.body_count = sl.count;
        return s;
    }

    // repeat <N> times as <i>
    Expr  *count    = parse_expression(p);
    expect(p, TOKEN_TIMES, "expected 'times'");
    expect(p, TOKEN_AS,    "expected 'as'");
    Token *iterator = expect(p, TOKEN_IDENTIFIER, "expected iterator name");

    // One-liner: something on the same line
    if (!check(p, TOKEN_NEWLINE) && !is_at_end(p)) {
        Stmt *body_stmt = parse_statement(p);
        Stmt *s = make_stmt(STMT_REPEAT_TIMES, line);
        s->repeat_times.count      = count;
        s->repeat_times.iterator   = strdup(iterator->value);
        s->repeat_times.body       = malloc(sizeof(Stmt *));
        s->repeat_times.body[0]    = body_stmt;
        s->repeat_times.body_count = 1;
        return s;
    }

    // Block form
    skip_newlines(p);
    StmtList sl = parse_block(p);

    Stmt *s = make_stmt(STMT_REPEAT_TIMES, line);
    s->repeat_times.count      = count;
    s->repeat_times.iterator   = strdup(iterator->value);
    s->repeat_times.body       = sl.items;
    s->repeat_times.body_count = sl.count;
    return s;
}

static Stmt *parse_func_decl(Parser *p) {
    int   line = peek(p)->line;
    advance(p); // consume 'func'
    Token *name = expect(p, TOKEN_IDENTIFIER, "expected function name");

    // Collect params (identifiers before newline)
    char  **params      = malloc(8 * sizeof(char *));
    size_t  param_count = 0;
    size_t  param_cap   = 8;
    while (check(p, TOKEN_IDENTIFIER)) {
        if (param_count >= param_cap) {
            param_cap *= 2;
            params = realloc(params, param_cap * sizeof(char *));
        }
        params[param_count++] = strdup(advance(p)->value);
    }

    skip_newlines(p);
    StmtList sl = parse_block(p);

    Stmt *s = make_stmt(STMT_FUNC_DECL, line);
    s->func_decl.name        = strdup(name->value);
    s->func_decl.params      = params;
    s->func_decl.param_count = param_count;
    s->func_decl.body        = sl.items;
    s->func_decl.body_count  = sl.count;
    return s;
}

static Stmt *parse_object_decl(Parser *p) {
    int   line = peek(p)->line;
    advance(p); // consume 'object'
    Token *name = expect(p, TOKEN_IDENTIFIER, "expected object name");
    expect(p, TOKEN_LBRACE, "expected '{'");
    skip_newlines(p);

    FieldDef *fields     = malloc(8 * sizeof(FieldDef));
    size_t    field_count = 0;
    size_t    field_cap   = 8;

    while (!is_at_end(p) && !check(p, TOKEN_RBRACE)) {
        if (field_count >= field_cap) {
            field_cap *= 2;
            fields = realloc(fields, field_cap * sizeof(FieldDef));
        }
        Token   *fname = expect(p, TOKEN_IDENTIFIER, "expected field name");
        expect(p, TOKEN_ASSIGN, "expected '='");
        TypeName  ftype = parse_type_name(p);
        fields[field_count].name = strdup(fname->value);
        fields[field_count].type = ftype;
        field_count++;
        skip_newlines(p);
    }
    expect(p, TOKEN_RBRACE, "expected '}'");

    Stmt *s = make_stmt(STMT_OBJECT_DECL, line);
    s->object_decl.name        = strdup(name->value);
    s->object_decl.fields      = fields;
    s->object_decl.field_count = field_count;
    return s;
}

static Stmt *parse_return(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'return'

    Expr *value = NULL;
    if (!check(p, TOKEN_NEWLINE) && !is_at_end(p)) {
        value = parse_expression(p);
    }

    Stmt *s = make_stmt(STMT_RETURN, line);
    s->ret.value = value;
    return s;
}

static Stmt *parse_identifier_stmt(Parser *p) {
    int    line = peek(p)->line;
    Token *name = advance(p);

    // member assign: name.field = expr
    if (check(p, TOKEN_DOT)) {
        advance(p);
        Token *field = expect(p, TOKEN_IDENTIFIER, "expected field name");
        expect(p, TOKEN_ASSIGN, "expected '='");
        Expr  *value = parse_expression(p);

        Stmt *s = make_stmt(STMT_MEMBER_ASSIGN, line);
        s->member_assign.object = strdup(name->value);
        s->member_assign.field  = strdup(field->value);
        s->member_assign.value  = value;
        return s;
    }

    // function call: name(args)
    if (check(p, TOKEN_LPAREN)) {
        advance(p);
        ExprList el = parse_arg_list(p);
        expect(p, TOKEN_RPAREN, "expected ')'");

        Stmt *s = make_stmt(STMT_FUNC_CALL, line);
        s->func_call.name      = strdup(name->value);
        s->func_call.args      = el.items;
        s->func_call.arg_count = el.count;
        return s;
    }

    // plain reassignment: name = expr
    if (check(p, TOKEN_ASSIGN)) {
        advance(p);
        Expr *value = parse_expression(p);

        Stmt *s = make_stmt(STMT_ASSIGN, line);
        s->assign.target = strdup(name->value);
        s->assign.value  = value;
        return s;
    }

    ERROR_F(line, "unexpected use of '%s'", name->value);
    return NULL;
}

static Stmt *parse_statement(Parser *p) {
    switch (peek(p)->type) {
        case TOKEN_VAR:
        case TOKEN_CONST:      return parse_var_decl(p);
        case TOKEN_PRINT:      return parse_print(p);
        case TOKEN_IF:         return parse_if(p);
        case TOKEN_REPEAT:     return parse_repeat(p);
        case TOKEN_FUNC:       return parse_func_decl(p);
        case TOKEN_OBJECT:     return parse_object_decl(p);
        case TOKEN_RETURN:     return parse_return(p);
        case TOKEN_IDENTIFIER: return parse_identifier_stmt(p);
        default:
            ERROR_F(peek(p)->line, "unexpected token '%s'", peek(p)->value);
            return NULL;
    }
}

//  Entry Point 

Program parser_parse(TokenList tokens) {
    Parser p = { .tokens = tokens, .pos = 0 };

    Stmt  **stmts    = malloc(64 * sizeof(Stmt *));
    size_t  count    = 0;
    size_t  capacity = 64;

    skip_newlines(&p);
    while (!is_at_end(&p)) {
        if (count >= capacity) {
            capacity *= 2;
            stmts = realloc(stmts, capacity * sizeof(Stmt *));
        }
        stmts[count++] = parse_statement(&p);
        skip_newlines(&p);
    }

    return (Program){ .stmts = stmts, .count = count };
}