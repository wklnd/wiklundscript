#include "interpreter.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>

#include "../native/native.h"

#define LOOP_LIMIT 100000

//  Runtime Values 

typedef enum {
    VAL_STRING,
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_OBJECT,
} ValKind;

typedef struct Value Value;  // Forward declaration

static Value make_string(const char *s);
static Value make_int(long n);
static Value make_float(double f);
static Value make_bool(int b);
static char *value_to_string(Value v);

typedef struct {
    char    *type_name;
    char   **field_names;
    Value   *field_values;
    size_t   field_count;
} ObjectVal;

typedef struct Value {
    ValKind kind;
    union {
        char     *string;
        long      integer;
        double    floating;
        int       boolean;
        ObjectVal object;
    };
} Value;

//  Variable Store 

typedef struct {
    char    *name;
    Value    value;
    int      is_mutable;
} VarEntry;

typedef struct {
    VarEntry *entries;
    size_t    count;
    size_t    capacity;
} VarStore;

static void varstore_init(VarStore *vs) {
    vs->entries  = malloc(16 * sizeof(VarEntry));
    vs->count    = 0;
    vs->capacity = 16;
}

static void varstore_free_entries(VarStore *vs);

static VarEntry *varstore_get(VarStore *vs, const char *name) {
    for (size_t i = 0; i < vs->count; i++) {
        if (strcmp(vs->entries[i].name, name) == 0) {
            return &vs->entries[i];
        }
    }
    return NULL;
}

static void varstore_set(VarStore *vs, const char *name, Value value, int is_mutable) {
    for (size_t i = 0; i < vs->count; i++) {
        if (strcmp(vs->entries[i].name, name) == 0) {
            vs->entries[i].value      = value;
            vs->entries[i].is_mutable = is_mutable;
            return;
        }
    }
    if (vs->count >= vs->capacity) {
        vs->capacity *= 2;
        vs->entries = realloc(vs->entries, vs->capacity * sizeof(VarEntry));
    }
    vs->entries[vs->count++] = (VarEntry){
        .name       = strdup(name),
        .value      = value,
        .is_mutable = is_mutable,
    };
}

//  Function Store 

typedef struct {
    char    *name;
    char   **params;
    size_t   param_count;
    Stmt   **body;
    size_t   body_count;
    int      is_native;
    NativeValue (*native)(size_t argc, const char **argv);
} FuncEntry;

typedef struct {
    FuncEntry *entries;
    size_t     count;
    size_t     capacity;
} FuncStore;

typedef struct Interp Interp;

static Value eval_expr(Interp *interp, Expr *expr);
static FuncEntry *resolve_native_from_modules(Interp *interp, const char *name);

static void funcstore_init(FuncStore *fs) {
    fs->entries  = malloc(8 * sizeof(FuncEntry));
    fs->count    = 0;
    fs->capacity = 8;
}

static void funcstore_set_native(FuncStore *fs, const char *name,
                                 NativeValue (*native)(size_t argc, const char **argv)) {
    if (fs->count >= fs->capacity) {
        fs->capacity *= 2;
        fs->entries = realloc(fs->entries, fs->capacity * sizeof(FuncEntry));
    }
    fs->entries[fs->count++] = (FuncEntry){
        .name = strdup(name),
        .params = NULL,
        .param_count = 0,
        .body = NULL,
        .body_count = 0,
        .is_native = 1,
        .native = native,
    };
}

static Value value_from_native_result(NativeValue res) {
    switch (res.type) {
        case NATIVE_INT:
            return make_int(res.as.integer);
        case NATIVE_FLOAT:
            return make_float(res.as.floating);
        case NATIVE_BOOL:
            return make_bool(res.as.boolean);
        case NATIVE_STRING: {
            Value value = make_string(res.as.string ? res.as.string : "");
            free(res.as.string);
            return value;
        }
    }

    return make_string("");
}

static Value call_native(FuncEntry *func, Interp *interp, Expr **args, size_t arg_count) {
    char **arg_strings = NULL;
    if (arg_count > 0) {
        arg_strings = malloc(arg_count * sizeof(char *));
        for (size_t i = 0; i < arg_count; i++) {
            Value arg = eval_expr(interp, args[i]);
            arg_strings[i] = value_to_string(arg);
        }
    }

    const char **argv = NULL;
    if (arg_count > 0) {
        argv = malloc(arg_count * sizeof(char *));
        for (size_t i = 0; i < arg_count; i++) {
            argv[i] = arg_strings[i];
        }
    }

    NativeValue res = func->native(arg_count, argv);

    for (size_t i = 0; i < arg_count; i++) {
        free(arg_strings[i]);
    }
    free(arg_strings);
    free(argv);

    return value_from_native_result(res);
}

static FuncEntry *funcstore_get(FuncStore *fs, const char *name) {
    for (size_t i = 0; i < fs->count; i++) {
        if (strcmp(fs->entries[i].name, name) == 0) {
            return &fs->entries[i];
        }
    }
    return NULL;
}

static void funcstore_set(FuncStore *fs, const char *name,
                          char **params, size_t param_count,
                          Stmt **body,   size_t body_count) {
    if (fs->count >= fs->capacity) {
        fs->capacity *= 2;
        fs->entries = realloc(fs->entries, fs->capacity * sizeof(FuncEntry));
    }
    fs->entries[fs->count++] = (FuncEntry){
        .name        = strdup(name),
        .params      = params,
        .param_count = param_count,
        .body        = body,
        .body_count  = body_count,
    };
}

//  Blueprint Store 

typedef struct {
    char     *name;
    FieldDef *fields;
    size_t    field_count;
} Blueprint;

typedef struct {
    Blueprint *entries;
    size_t     count;
    size_t     capacity;
} BlueprintStore;

// Dynamic library handles with module names
typedef struct {
    char **names;
    void  **handles;
    size_t count;
    size_t capacity;
} HandleStore;

static void handlestore_init(HandleStore *hs) {
    hs->names = malloc(4 * sizeof(char*));
    hs->handles = malloc(4 * sizeof(void*));
    hs->count = 0;
    hs->capacity = 4;
}

static void handlestore_push(HandleStore *hs, const char *name, void *h) {
    if (hs->count >= hs->capacity) {
        hs->capacity *= 2;
        hs->names = realloc(hs->names, hs->capacity * sizeof(char*));
        hs->handles = realloc(hs->handles, hs->capacity * sizeof(void*));
    }
    hs->names[hs->count] = strdup(name);
    hs->handles[hs->count++] = h;
}

static void blueprintstore_init(BlueprintStore *bs) {
    bs->entries  = malloc(8 * sizeof(Blueprint));
    bs->count    = 0;
    bs->capacity = 8;
}

static Blueprint *blueprintstore_get(BlueprintStore *bs, const char *name) {
    for (size_t i = 0; i < bs->count; i++) {
        if (strcmp(bs->entries[i].name, name) == 0) {
            return &bs->entries[i];
        }
    }
    return NULL;
}

static void blueprintstore_set(BlueprintStore *bs, const char *name,
                                FieldDef *fields, size_t field_count) {
    if (bs->count >= bs->capacity) {
        bs->capacity *= 2;
        bs->entries = realloc(bs->entries, bs->capacity * sizeof(Blueprint));
    }
    bs->entries[bs->count++] = (Blueprint){
        .name        = strdup(name),
        .fields      = fields,
        .field_count = field_count,
    };
}

//  Interpreter State 

struct Interp {
    VarStore       vars;
    FuncStore      funcs;
    BlueprintStore blueprints;
    HandleStore    handles;
    int            returning;
    Value          return_value;
};

static FuncEntry *resolve_native_from_modules(Interp *interp, const char *name) {
    for (size_t mi = 0; mi < interp->handles.count; mi++) {
        const char *module = interp->handles.names[mi];
        void *handle = interp->handles.handles[mi];
        char sym[256];
        snprintf(sym, sizeof(sym), "%s_%s", module, name);
        void *raw = dlsym(handle, sym);
        if (raw) {
            NativeValue (*fn)(size_t, const char **) = (NativeValue (*)(size_t, const char **))raw;
            funcstore_set_native(&interp->funcs, name, fn);
            return funcstore_get(&interp->funcs, name);
        }
    }
    return NULL;
}

//  Value Helpers 

static ValKind typename_to_valkind(TypeName t) {
    switch (t) {
        case TYPE_STRING: return VAL_STRING;
        case TYPE_INT:    return VAL_INT;
        case TYPE_FLOAT:  return VAL_FLOAT;
        case TYPE_BOOL:   return VAL_BOOL;
        default:          return VAL_STRING;
    }
}

static const char *valkind_name(ValKind k) {
    switch (k) {
        case VAL_STRING: return "String";
        case VAL_INT:    return "Int";
        case VAL_FLOAT:  return "Float";
        case VAL_BOOL:   return "Bool";
        case VAL_OBJECT: return "Object";
    }
    return "Unknown";
}

static Value make_string(const char *s) {
    return (Value){ .kind = VAL_STRING, .string = strdup(s) };
}

static Value make_int(long n) {
    return (Value){ .kind = VAL_INT, .integer = n };
}

static Value make_float(double f) {
    return (Value){ .kind = VAL_FLOAT, .floating = f };
}

static Value make_bool(int b) {
    return (Value){ .kind = VAL_BOOL, .boolean = b };
}

static char *value_to_string(Value v) {
    char buf[64];
    switch (v.kind) {
        case VAL_STRING: return strdup(v.string);
        case VAL_INT:    snprintf(buf, sizeof(buf), "%ld", v.integer);  return strdup(buf);
        case VAL_FLOAT:  snprintf(buf, sizeof(buf), "%g",  v.floating); return strdup(buf);
        case VAL_BOOL:   return strdup(v.boolean ? "true" : "false");
        case VAL_OBJECT: snprintf(buf, sizeof(buf), "[%s]", v.object.type_name); return strdup(buf);
    }
    return strdup("");
}

static int value_is_truthy(Value v) {
    switch (v.kind) {
        case VAL_BOOL:   return v.boolean;
        case VAL_INT:    return v.integer != 0;
        case VAL_FLOAT:  return v.floating != 0.0;
        case VAL_STRING: return strlen(v.string) > 0;
        default:         return 0;
    }
}

//  Object Instantiation 

static Value instantiate(Interp *interp, const char *type_name, int line) {
    Blueprint *bp = blueprintstore_get(&interp->blueprints, type_name);
    if (!bp) ERROR_F(line, "unknown object type '%s'", type_name);

    ObjectVal obj;
    obj.type_name    = strdup(type_name);
    obj.field_count  = bp->field_count;
    obj.field_names  = malloc(bp->field_count * sizeof(char *));
    obj.field_values = malloc(bp->field_count * sizeof(Value));

    for (size_t i = 0; i < bp->field_count; i++) {
        obj.field_names[i] = strdup(bp->fields[i].name);
        switch (bp->fields[i].type) {
            case TYPE_STRING: obj.field_values[i] = make_string(""); break;
            case TYPE_INT:    obj.field_values[i] = make_int(0);     break;
            case TYPE_FLOAT:  obj.field_values[i] = make_float(0.0); break;
            case TYPE_BOOL:   obj.field_values[i] = make_bool(0);    break;
            default:          obj.field_values[i] = make_string(""); break;
        }
    }

    return (Value){ .kind = VAL_OBJECT, .object = obj };
}

//  Forward Declarations 

static Value eval_expr(Interp *interp, Expr *expr);
static void  exec_stmt(Interp *interp, Stmt *stmt);
static void  exec_block(Interp *interp, Stmt **body, size_t count);

//  Expression Evaluation 

static Value eval_expr(Interp *interp, Expr *expr) {
    switch (expr->kind) {

        case EXPR_LITERAL: {
            switch (expr->literal.type) {
                case TYPE_STRING: return make_string(expr->literal.value);
                case TYPE_INT:    return make_int((long)atol(expr->literal.value));
                case TYPE_FLOAT:  return make_float(atof(expr->literal.value));
                case TYPE_BOOL:   return make_bool(strcmp(expr->literal.value, "true") == 0);
                default:          return make_string(expr->literal.value);
            }
        }

        case EXPR_VARIABLE: {
            if (blueprintstore_get(&interp->blueprints, expr->variable.name)) {
                return instantiate(interp, expr->variable.name, 0);
            }
            VarEntry *entry = varstore_get(&interp->vars, expr->variable.name);
            if (!entry) ERROR_F(0, "undefined variable '%s'", expr->variable.name);
            return entry->value;
        }

        case EXPR_MEMBER: {
            VarEntry *entry = varstore_get(&interp->vars, expr->member.object);
            if (!entry) ERROR_F(0, "undefined variable '%s'", expr->member.object);
            if (entry->value.kind != VAL_OBJECT) {
                ERROR_F(0, "'%s' is not an object", expr->member.object);
            }
            ObjectVal *obj = &entry->value.object;
            for (size_t i = 0; i < obj->field_count; i++) {
                if (strcmp(obj->field_names[i], expr->member.field) == 0) {
                    return obj->field_values[i];
                }
            }
            ERROR_F(0, "unknown field '%s' on '%s'", expr->member.field, expr->member.object);
            return make_int(0);
        }

        case EXPR_BINARY: {
            Value l = eval_expr(interp, expr->binary.left);
            Value r = eval_expr(interp, expr->binary.right);
            const char *op = expr->binary.op;

            if (strcmp(op, "+") == 0 &&
                (l.kind == VAL_STRING || r.kind == VAL_STRING)) {
                char  *ls  = value_to_string(l);
                char  *rs  = value_to_string(r);
                size_t len = strlen(ls) + strlen(rs) + 1;
                char  *buf = malloc(len);
                snprintf(buf, len, "%s%s", ls, rs);
                Value result = make_string(buf);
                free(ls); free(rs); free(buf);
                return result;
            }

            if (strcmp(op, "+") == 0) {
                if (l.kind == VAL_FLOAT || r.kind == VAL_FLOAT)
                    return make_float((l.kind == VAL_FLOAT ? l.floating : l.integer) +
                                      (r.kind == VAL_FLOAT ? r.floating : r.integer));
                return make_int(l.integer + r.integer);
            }
            if (strcmp(op, "-") == 0) {
                if (l.kind == VAL_FLOAT || r.kind == VAL_FLOAT)
                    return make_float((l.kind == VAL_FLOAT ? l.floating : l.integer) -
                                      (r.kind == VAL_FLOAT ? r.floating : r.integer));
                return make_int(l.integer - r.integer);
            }
            if (strcmp(op, "*") == 0) {
                if (l.kind == VAL_FLOAT || r.kind == VAL_FLOAT)
                    return make_float((l.kind == VAL_FLOAT ? l.floating : l.integer) *
                                      (r.kind == VAL_FLOAT ? r.floating : r.integer));
                return make_int(l.integer * r.integer);
            }
            if (strcmp(op, "/") == 0) {
                double divisor = r.kind == VAL_FLOAT ? r.floating : r.integer;
                if (divisor == 0.0) ERROR_F(0, "division by zero");
                return make_float((l.kind == VAL_FLOAT ? l.floating : l.integer) / divisor);
            }

            if (strcmp(op, "==") == 0) {
                if (l.kind == VAL_STRING && r.kind == VAL_STRING)
                    return make_bool(strcmp(l.string, r.string) == 0);
                if (l.kind == VAL_BOOL && r.kind == VAL_BOOL)
                    return make_bool(l.boolean == r.boolean);
                double lv = l.kind == VAL_FLOAT ? l.floating : l.integer;
                double rv = r.kind == VAL_FLOAT ? r.floating : r.integer;
                return make_bool(lv == rv);
            }
            if (strcmp(op, "!=") == 0) {
                if (l.kind == VAL_STRING && r.kind == VAL_STRING)
                    return make_bool(strcmp(l.string, r.string) != 0);
                double lv = l.kind == VAL_FLOAT ? l.floating : l.integer;
                double rv = r.kind == VAL_FLOAT ? r.floating : r.integer;
                return make_bool(lv != rv);
            }
            if (strcmp(op, ">")  == 0) {
                double lv = l.kind == VAL_FLOAT ? l.floating : l.integer;
                double rv = r.kind == VAL_FLOAT ? r.floating : r.integer;
                return make_bool(lv > rv);
            }
            if (strcmp(op, "<")  == 0) {
                double lv = l.kind == VAL_FLOAT ? l.floating : l.integer;
                double rv = r.kind == VAL_FLOAT ? r.floating : r.integer;
                return make_bool(lv < rv);
            }
            if (strcmp(op, ">=") == 0) {
                double lv = l.kind == VAL_FLOAT ? l.floating : l.integer;
                double rv = r.kind == VAL_FLOAT ? r.floating : r.integer;
                return make_bool(lv >= rv);
            }
            if (strcmp(op, "<=") == 0) {
                double lv = l.kind == VAL_FLOAT ? l.floating : l.integer;
                double rv = r.kind == VAL_FLOAT ? r.floating : r.integer;
                return make_bool(lv <= rv);
            }

            ERROR_F(0, "unknown operator '%s'", op);
            return make_int(0);
        }

        case EXPR_CALL: {
            FuncEntry *func = funcstore_get(&interp->funcs, expr->call.name);
            if (!func) {
                func = resolve_native_from_modules(interp, expr->call.name);
            }
            if (!func) ERROR_F(0, "undefined function '%s'", expr->call.name);

            if (func->is_native) {
                return call_native(func, interp, expr->call.args, expr->call.arg_count);
            }

            VarStore prev = interp->vars;
            VarStore call_scope;
            varstore_init(&call_scope);

            for (size_t i = 0; i < prev.count; i++) {
                varstore_set(&call_scope, prev.entries[i].name,
                             prev.entries[i].value,
                             prev.entries[i].is_mutable);
            }

            for (size_t i = 0; i < func->param_count; i++) {
                Value arg = i < expr->call.arg_count
                    ? eval_expr(interp, expr->call.args[i])
                    : make_int(0);
                varstore_set(&call_scope, func->params[i], arg, 1);
            }

            interp->vars = call_scope;
            exec_block(interp, func->body, func->body_count);

            Value ret = interp->returning ? interp->return_value : make_int(0);
            interp->returning = 0;

            varstore_free_entries(&interp->vars);
            interp->vars = prev;

            return ret;
        }
    }

    return make_int(0);
}

//  Statement Execution 

static void exec_stmt(Interp *interp, Stmt *stmt) {
    switch (stmt->kind) {

        case STMT_VAR_DECL: {
            VarEntry *existing = varstore_get(&interp->vars, stmt->var_decl.name);

            if (existing && !existing->is_mutable) {
                ERROR_F(stmt->line, "cannot reassign constant '%s'", stmt->var_decl.name);
            }

            Value value = eval_expr(interp, stmt->var_decl.value);

            if (existing && existing->value.kind != value.kind) {
                ERROR_F(stmt->line,
                    "type error: '%s' is %s, cannot assign %s",
                    stmt->var_decl.name,
                    valkind_name(existing->value.kind),
                    valkind_name(value.kind));
            }

            varstore_set(&interp->vars, stmt->var_decl.name, value, stmt->var_decl.is_mutable);
            break;
        }

        case STMT_ASSIGN: {
            VarEntry *existing = varstore_get(&interp->vars, stmt->assign.target);
            if (!existing)              ERROR_F(stmt->line, "undefined variable '%s'", stmt->assign.target);
            if (!existing->is_mutable)  ERROR_F(stmt->line, "cannot reassign constant '%s'", stmt->assign.target);

            Value value = eval_expr(interp, stmt->assign.value);
            if (existing->value.kind != value.kind) {
                ERROR_F(stmt->line,
                    "type error: '%s' is %s, cannot assign %s",
                    stmt->assign.target,
                    valkind_name(existing->value.kind),
                    valkind_name(value.kind));
            }
            existing->value = value;
            break;
        }

        case STMT_MEMBER_ASSIGN: {
            VarEntry *entry = varstore_get(&interp->vars, stmt->member_assign.object);
            if (!entry) ERROR_F(stmt->line, "undefined variable '%s'", stmt->member_assign.object);
            if (entry->value.kind != VAL_OBJECT) {
                ERROR_F(stmt->line, "'%s' is not an object", stmt->member_assign.object);
            }

            ObjectVal *obj = &entry->value.object;
            Value      val = eval_expr(interp, stmt->member_assign.value);

            Blueprint *bp = blueprintstore_get(&interp->blueprints, obj->type_name);
            for (size_t i = 0; i < obj->field_count; i++) {
                if (strcmp(obj->field_names[i], stmt->member_assign.field) == 0) {
                    ValKind expected = typename_to_valkind(bp->fields[i].type);
                    if (val.kind != expected) {
                        ERROR_F(stmt->line,
                            "type error: '%s.%s' expects %s, got %s",
                            stmt->member_assign.object,
                            stmt->member_assign.field,
                            valkind_name(expected),
                            valkind_name(val.kind));
                    }
                    obj->field_values[i] = val;
                    return;
                }
            }
            ERROR_F(stmt->line, "unknown field '%s' on '%s'",
                stmt->member_assign.field, stmt->member_assign.object);
            break;
        }

        case STMT_PRINT: {
            Value  val = eval_expr(interp, stmt->print.value);
            char  *str = value_to_string(val);
            printf("%s\n", str);
            free(str);
            break;
        }

        case STMT_IF: {
            Value cond = eval_expr(interp, stmt->if_stmt.condition);
            if (value_is_truthy(cond)) {
                exec_block(interp, stmt->if_stmt.body, stmt->if_stmt.body_count);
            } else {
                exec_block(interp, stmt->if_stmt.else_body, stmt->if_stmt.else_count);
            }
            break;
        }

        case STMT_REPEAT_TIMES: {
            Value count_val = eval_expr(interp, stmt->repeat_times.count);
            if (count_val.kind != VAL_INT) {
                ERROR_F(stmt->line, "'repeat' count must be Int");
            }
            long count = count_val.integer;
            for (long i = 0; i < count; i++) {
                varstore_set(&interp->vars, stmt->repeat_times.iterator, make_int(i), 0);
                exec_block(interp, stmt->repeat_times.body, stmt->repeat_times.body_count);
                if (interp->returning) break;
            }
            break;
        }

        case STMT_REPEAT_WHILE: {
            int iterations = 0;
            while (value_is_truthy(eval_expr(interp, stmt->repeat_while.condition))) {
                exec_block(interp, stmt->repeat_while.body, stmt->repeat_while.body_count);
                if (interp->returning) break;
                if (++iterations > LOOP_LIMIT) {
                    ERROR_F(stmt->line, "infinite loop detected (exceeded %d iterations)", LOOP_LIMIT);
                }
            }
            break;
        }

        case STMT_FUNC_DECL: {
            funcstore_set(&interp->funcs,
                stmt->func_decl.name,
                stmt->func_decl.params,
                stmt->func_decl.param_count,
                stmt->func_decl.body,
                stmt->func_decl.body_count);
            break;
        }

        case STMT_IMPORT: {
            const char *module = stmt->import_stmt.module;

            char libname[256];
            snprintf(libname, sizeof(libname), "lib%s.so", module);

            void *handle = dlopen(libname, RTLD_NOW);
            if (!handle) {
                // try without 'lib' prefix
                snprintf(libname, sizeof(libname), "%s.so", module);
                handle = dlopen(libname, RTLD_NOW);
            }
            if (!handle) {
                snprintf(libname, sizeof(libname), "native/lib%s.so", module);
                handle = dlopen(libname, RTLD_NOW);
            }
            if (!handle) {
                snprintf(libname, sizeof(libname), "native/%s.so", module);
                handle = dlopen(libname, RTLD_NOW);
            }
            if (!handle) {
                ERROR_F(stmt->line, "failed to open module '%s': %s", module, dlerror());
            }

            // wildcard imports: just record the module handle for lazy resolution
            if (stmt->import_stmt.is_wildcard) {
                handlestore_push(&interp->handles, module, handle);
                break;
            }

            void **resolved = NULL;
            if (stmt->import_stmt.field_count > 0) {
                resolved = malloc(stmt->import_stmt.field_count * sizeof(void *));
            }

            // resolve specific fields first so import is atomic
            for (size_t i = 0; i < stmt->import_stmt.field_count; i++) {
                const char *field = stmt->import_stmt.fields[i];
                char sym[256];
                snprintf(sym, sizeof(sym), "%s_%s", module, field);
                void *raw = dlsym(handle, sym);
                if (!raw) {
                    free(resolved);
                    dlclose(handle);
                    ERROR_F(stmt->line, "symbol '%s' not found in module '%s'", sym, module);
                }
                resolved[i] = raw;
            }

            for (size_t i = 0; i < stmt->import_stmt.field_count; i++) {
                const char *field = stmt->import_stmt.fields[i];
                NativeValue (*fn)(size_t, const char **) = (NativeValue (*)(size_t, const char **))resolved[i];
                funcstore_set_native(&interp->funcs, field, fn);
            }
            free(resolved);
            handlestore_push(&interp->handles, module, handle);
            break;
        }

        case STMT_FUNC_CALL: {
            FuncEntry *func = funcstore_get(&interp->funcs, stmt->func_call.name);
            if (!func) {
                func = resolve_native_from_modules(interp, stmt->func_call.name);
            }
            if (!func) ERROR_F(stmt->line, "undefined function '%s'", stmt->func_call.name);

            if (func->is_native) {
                (void)call_native(func, interp, stmt->func_call.args, stmt->func_call.arg_count);
                break;
            }

            VarStore prev = interp->vars;
            VarStore call_scope;
            varstore_init(&call_scope);

            for (size_t i = 0; i < prev.count; i++) {
                varstore_set(&call_scope, prev.entries[i].name,
                             prev.entries[i].value,
                             prev.entries[i].is_mutable);
            }

            for (size_t i = 0; i < func->param_count; i++) {
                Value arg = i < stmt->func_call.arg_count
                    ? eval_expr(interp, stmt->func_call.args[i])
                    : make_int(0);
                varstore_set(&call_scope, func->params[i], arg, 1);
            }

            interp->vars = call_scope;
            exec_block(interp, func->body, func->body_count);
            interp->returning = 0;

            varstore_free_entries(&interp->vars);
            interp->vars = prev;
            break;
        }

        case STMT_OBJECT_DECL: {
            blueprintstore_set(&interp->blueprints,
                stmt->object_decl.name,
                stmt->object_decl.fields,
                stmt->object_decl.field_count);
            break;
        }

        case STMT_RETURN: {
            interp->return_value = stmt->ret.value
                ? eval_expr(interp, stmt->ret.value)
                : make_int(0);
            interp->returning = 1;
            break;
        }
    }
}

static void exec_block(Interp *interp, Stmt **body, size_t count) {
    for (size_t i = 0; i < count; i++) {
        exec_stmt(interp, body[i]);
        if (interp->returning) break;
    }
}

//  Cleanup 

static void varstore_free_entries(VarStore *vs) {
    for (size_t i = 0; i < vs->count; i++) {
        free(vs->entries[i].name);
    }
    free(vs->entries);
}

//  Entry Point 

void interpreter_run(Program *program) {
    Interp interp = {0};
    varstore_init(&interp.vars);
    funcstore_init(&interp.funcs);
    blueprintstore_init(&interp.blueprints);
    handlestore_init(&interp.handles);

    exec_block(&interp, program->stmts, program->count);

    varstore_free_entries(&interp.vars);
    free(interp.funcs.entries);
    free(interp.blueprints.entries);
    for (size_t i = 0; i < interp.handles.count; i++) {
        dlclose(interp.handles.handles[i]);
        free(interp.handles.names[i]);
    }
    free(interp.handles.handles);
    free(interp.handles.names);
}