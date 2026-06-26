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
    expect(p, TOKEN_INDENT, "expected indented block");
    skip_newlines(p);
    while (!is_at_end(p) && !check(p, TOKEN_DEDENT)) {
        stmtlist_push(&sl, parse_statement(p));
        skip_newlines(p);
    }
    if (check(p, TOKEN_DEDENT)) advance(p);
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

    // Array literal: [expr, expr, ...]
    if (t->type == TOKEN_LBRACKET) {
        advance(p);

        Expr **items = NULL;
        size_t count = 0;
        size_t capacity = 4;

        if (!check(p, TOKEN_RBRACKET)) {
            items = malloc(capacity * sizeof(Expr *));
            while (!check(p, TOKEN_RBRACKET) && !is_at_end(p)) {
                if (count >= capacity) {
                    capacity *= 2;
                    items = realloc(items, capacity * sizeof(Expr *));
                }

                items[count++] = parse_expression(p);

                if (check(p, TOKEN_COMMA)) {
                    advance(p);
                } else {
                    break;
                }
            }
        }

        expect(p, TOKEN_RBRACKET, "expected ']'");

        Expr *e = make_expr(EXPR_ARRAY);
        e->array.items = items;
        e->array.count = count;
        return e;
    }

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

static Expr *parse_postfix(Parser *p) {
    Expr *expr = parse_primary(p);

    while (check(p, TOKEN_LBRACKET)) {
        advance(p);
        Expr *index = parse_expression(p);
        expect(p, TOKEN_RBRACKET, "expected ']'");

        Expr *e = make_expr(EXPR_INDEX);
        e->index.array = expr;
        e->index.index = index;
        expr = e;
    }

    return expr;
}

static Expr *parse_unary(Parser *p) {
    if (check(p, TOKEN_MINUS)) {
        Token *op = advance(p);
        Expr *right = parse_unary(p);
        Expr *zero = make_expr(EXPR_LITERAL);
        zero->literal.value = strdup("0");
        zero->literal.type = TYPE_INT;

        Expr *e = make_expr(EXPR_BINARY);
        e->binary.left = zero;
        e->binary.op = strdup(op->value);
        e->binary.right = right;
        return e;
    }

    return parse_postfix(p);
}

static Expr *parse_muldiv(Parser *p) {
    Expr *left = parse_unary(p);

    while (check(p, TOKEN_STAR) || check(p, TOKEN_SLASH)) {
        Token *op    = advance(p);
        Expr  *right = parse_unary(p);
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
           check(p, TOKEN_GTE) || check(p, TOKEN_LTE) ||
           check(p, TOKEN_IS)) {
        Token *op    = advance(p);
        Expr  *right = parse_addsub(p);
        Expr  *e     = make_expr(EXPR_BINARY);
        e->binary.left  = left;
        e->binary.op    = op->type == TOKEN_IS ? strdup("==") : strdup(op->value);
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
    advance(p); // consume 'print' or 'say'

    Expr *value;
    if (check(p, TOKEN_LPAREN)) {
        advance(p);
        value = parse_expression(p);
        expect(p, TOKEN_RPAREN, "expected ')'");
    } else {
        value = parse_expression(p);
    }

    Stmt *s = make_stmt(STMT_PRINT, line);
    s->print.value = value;
    return s;
}

static Stmt *parse_if(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'if'
    Expr *condition = parse_expression(p);
    skip_newlines(p);

    StmtList body = parse_block(p);

    StmtList else_body;
    stmtlist_init(&else_body);

    skip_newlines(p);
    if (check(p, TOKEN_ELSE)) {
        advance(p);
        skip_newlines(p);
        else_body = parse_block(p);
    }

    Stmt *s = make_stmt(STMT_IF, line);
    s->if_stmt.condition  = condition;
    s->if_stmt.body       = body.items;
    s->if_stmt.body_count = body.count;
    s->if_stmt.else_body  = else_body.items;
    s->if_stmt.else_count = else_body.count;
    return s;
}

static Stmt *parse_loop(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'loop'

    Expr  *count    = parse_expression(p);
    expect(p, TOKEN_AS, "expected 'as'");
    Token *iterator = expect(p, TOKEN_IDENTIFIER, "expected iterator name");
    skip_newlines(p);
    StmtList sl = parse_block(p);

    Stmt *s = make_stmt(STMT_REPEAT_TIMES, line);
    s->repeat_times.count      = count;
    s->repeat_times.iterator   = strdup(iterator->value);
    s->repeat_times.body       = sl.items;
    s->repeat_times.body_count = sl.count;
    return s;
}

static Stmt *parse_while(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'while'

    Expr     *condition = parse_expression(p);
    skip_newlines(p);
    StmtList  sl        = parse_block(p);

    Stmt *s = make_stmt(STMT_REPEAT_WHILE, line);
    s->repeat_while.condition  = condition;
    s->repeat_while.body       = sl.items;
    s->repeat_while.body_count = sl.count;
    return s;
}

static Stmt *parse_func_decl(Parser *p) {
    int   line = peek(p)->line;
    advance(p); // consume 'func'
    Token *name = expect(p, TOKEN_IDENTIFIER, "expected function name");

    char  **params      = malloc(8 * sizeof(char *));
    size_t  param_count = 0;
    size_t  param_cap   = 8;

    if (check(p, TOKEN_LPAREN)) {
        advance(p);
        while (!check(p, TOKEN_RPAREN) && !is_at_end(p)) {
            if (param_count >= param_cap) {
                param_cap *= 2;
                params = realloc(params, param_cap * sizeof(char *));
            }
            params[param_count++] = strdup(expect(p, TOKEN_IDENTIFIER, "expected parameter name")->value);
            if (check(p, TOKEN_COMMA)) advance(p);
        }
        expect(p, TOKEN_RPAREN, "expected ')'");
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
    skip_newlines(p);

    FieldDef *fields     = malloc(8 * sizeof(FieldDef));
    size_t    field_count = 0;
    size_t    field_cap   = 8;

    expect(p, TOKEN_INDENT, "expected indented block");
    skip_newlines(p);

    while (!is_at_end(p) && !check(p, TOKEN_DEDENT)) {
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
    if (check(p, TOKEN_DEDENT)) advance(p);

    Stmt *s = make_stmt(STMT_OBJECT_DECL, line);
    s->object_decl.name        = strdup(name->value);
    s->object_decl.fields      = fields;
    s->object_decl.field_count = field_count;
    return s;
}

static Stmt *parse_import(Parser *p) {
    int line = peek(p)->line;
    advance(p); // consume 'import'
    Token *module = expect(p, TOKEN_IDENTIFIER, "expected module name");

    Stmt *s = make_stmt(STMT_IMPORT, line);
    s->import_stmt.module      = strdup(module->value);
    s->import_stmt.fields      = NULL;
    s->import_stmt.field_count = 0;
    s->import_stmt.is_wildcard = 0;

    // bare import with no dot: treat as wildcard
    if (!check(p, TOKEN_DOT)) {
        s->import_stmt.is_wildcard = 1;
        return s;
    }
    advance(p); // consume '.'

    // wildcard: Module.*
    if (check(p, TOKEN_STAR)) {
        advance(p);
        s->import_stmt.is_wildcard = 1;
        return s;
    }

    // specific fields
    Token *field = expect(p, TOKEN_IDENTIFIER, "expected field name");
    size_t cap = 4;
    s->import_stmt.fields = malloc(cap * sizeof(char *));
    s->import_stmt.fields[s->import_stmt.field_count++] = strdup(field->value);

    while (check(p, TOKEN_COMMA)) {
        advance(p);
        Token *f = expect(p, TOKEN_IDENTIFIER, "expected field name");
        if (s->import_stmt.field_count >= cap) {
            cap *= 2;
            s->import_stmt.fields = realloc(s->import_stmt.fields, cap * sizeof(char *));
        }
        s->import_stmt.fields[s->import_stmt.field_count++] = strdup(f->value);
    }

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
        case TOKEN_PRINT:
        case TOKEN_SAY:        return parse_print(p);
        case TOKEN_IF:         return parse_if(p);
        case TOKEN_LOOP:       return parse_loop(p);
        case TOKEN_WHILE:      return parse_while(p);
        case TOKEN_FUNC:       return parse_func_decl(p);
        case TOKEN_OBJECT:     return parse_object_decl(p);
        case TOKEN_IMPORT:     return parse_import(p);
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