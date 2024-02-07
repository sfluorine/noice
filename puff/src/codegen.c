#include "codegen.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SYMTABLE_CAP 1024

typedef struct {
    token_t token;
    int is_fun_args;
    int sp_offset;
} symbol_t;

static int initialized = 0;

static symbol_t locals[SYMTABLE_CAP];
static int locals_len = 0;
static int sp_offset = -1;

typedef struct {
    topdecl_fun_t* fun;
    int ip;
} function_t;

static function_t functions[SYMTABLE_CAP];
static int functions_len = 0;

static int locals_lookup(token_t ident)
{
    for (int i = 0; i < locals_len; i++) {
        if (strncmp(ident.start, locals[i].token.start, ident.length) == 0) {
            return i;
        }
    }

    return -1;
}

static int functions_lookup(token_t name)
{
    for (int i = 0; i < functions_len; i++) {
        if (strncmp(name.start, functions[i].fun->name.start, name.length) == 0) {
            return i;
        }
    }

    return -1;
}

void codegen_init(npb_t* pb)
{
    npb_init(pb);
    initialized = 1;
}

void codegen_expr(npb_t* pb, expr_t* expr)
{
    assert(initialized);

    switch (expr->kind) {
        case EXPR_IDENTIFIER: {
            expr_ident_t* ident = (expr_ident_t*)expr;
            int index = locals_lookup(ident->ident);
            
            if (index == -1) {
                fprintf(stderr, "ERROR: use of undeclared variable '%.*s'\n", ident->ident.length, ident->ident.start);
                exit(1);
            }

            if (locals[index].is_fun_args) {
                npb_loadarg(pb, locals[index].sp_offset);
            } else {
                npb_dup(pb, locals[index].sp_offset);
            }
        } break;
        case EXPR_NUMBER: {
            expr_num_t* num = (expr_num_t*)expr;
            npb_dpush(pb, num->number);
        } break;
        case EXPR_UNARY: {
            expr_unary_t* unary = (expr_unary_t*)expr;
            codegen_expr(pb, unary->operand);

            switch (unary->op) {
                case '-':
                    npb_dpush(pb, -1.0);
                    npb_dmul(pb);
                    break;
                default:
                    fprintf(stderr, "ERROR: unknown unary op: '%c'\n", unary->op);
                    exit(1);
            }
        } break;
        case EXPR_BINARY: {
            expr_binary_t* binary = (expr_binary_t*)expr;
            codegen_expr(pb, binary->lhs);
            codegen_expr(pb, binary->rhs);

            switch (binary->op) {
                case '+': npb_dadd(pb); break;
                case '-': npb_dsub(pb); break;
                case '*': npb_dmul(pb); break;
                case '/': npb_ddiv(pb); break;
                default: {
                    fprintf(stderr, "ERROR: unknown binary op: '%c'\n", binary->op);
                    exit(1);
                }
            }
        } break;
        case EXPR_FUNCALL: {
            expr_funcall_t* funcall = (expr_funcall_t*)expr;

            if (strncmp(funcall->name.start, "print", funcall->name.length) == 0) {
                if (funcall->args_len != 1) {
                    fprintf(stderr, "ERROR: builtin function 'print' requires 1 argument\n");
                    exit(1);
                }

                codegen_expr(pb, funcall->args[0]);
                npb_print(pb);

                return;
            }

            int index = functions_lookup(funcall->name);
            if (index == -1) {
                fprintf(stderr, "ERROR: there's no such function '%.*s'\n", funcall->name.length, funcall->name.start);
                exit(1);
            }

            function_t function = functions[index];
            if (funcall->args_len != function.fun->args_len) {
                fprintf(stderr, "ERROR: function '%.*s' expecting %d argument(s)\n", function.fun->name.length, function.fun->name.start, function.fun->args_len);
                exit(1);
            }

            for (int i = 0; i < funcall->args_len; i++)
                codegen_expr(pb, funcall->args[i]);

            npb_call(pb, function.ip, funcall->args_len);
        } break;
    }
}

void codegen_stmt(npb_t* pb, stmt_t* stmt)
{
    assert(initialized);

    switch (stmt->kind) {
        case STMT_VARDECL: {
            stmt_vardecl_t* vardecl = (stmt_vardecl_t*)stmt;

            if (locals_lookup(vardecl->ident) != -1) {
                fprintf(stderr, "ERROR: symbol '%.*s' already exist\n", vardecl->ident.length, vardecl->ident.start);
                exit(1);
            }

            locals[locals_len++] = (symbol_t) {
                .token = vardecl->ident,
                .is_fun_args = 0,
                .sp_offset = ++sp_offset,
            };

            codegen_expr(pb, vardecl->expr);
        } break;
        case STMT_EXPR: {
            stmt_expr_t* expr = (stmt_expr_t*)stmt;

            codegen_expr(pb, expr->expr);
        } break;
        case STMT_RETURN: {
            stmt_return_t* ret = (stmt_return_t*)stmt;
            codegen_expr(pb, ret->expr);
            npb_ret(pb);
        } break;
    }
}

static void codegen_funbody(npb_t* pb, fun_body_t* funbody)
{
    if (!funbody)
        return;

    codegen_stmt(pb, funbody->stmt);
    codegen_funbody(pb, funbody->next);
}

void codegen_topdecl(npb_t* pb, topdecl_t* topdecl)
{
    assert(initialized);

    switch (topdecl->kind) {
        case TOPDECL_FUN: {
            topdecl_fun_t* fun = (topdecl_fun_t*)topdecl;

            if (functions_lookup(fun->name) != -1) {
                fprintf(stderr, "ERROR: function '%.*s' already exist\n", fun->name.length, fun->name.start);
                exit(1);
            }

            locals_len = 0;

            for (int i = 0; i < fun->args_len; i++) {
                locals[locals_len++] = (symbol_t) {
                    .token = fun->args[i],
                    .is_fun_args = 1,
                    .sp_offset = i,
                };
            }

            functions[functions_len++] = (function_t) {
                .fun = fun,
                .ip = pb->program_len,
            };

            if (strncmp(fun->name.start, "main", fun->name.length) != 0) {
                sp_offset += 3 + fun->args_len;
            }

            int start_sp = sp_offset;
            codegen_funbody(pb, fun->funbody);
            sp_offset = start_sp;

            if (strncmp(fun->name.start, "main", fun->name.length) != 0) {
                sp_offset -= 3;
            }
        } break;
    }
}

static void codegen_program_internal(npb_t* pb, program_t* program)
{
    assert(initialized);

    if (!program) {
        npb_halt(pb);
        return;
    }

    codegen_topdecl(pb, program->topdecl);
    codegen_program(pb, program->next);
}

int codegen_program(npb_t* pb, program_t* program)
{
    codegen_program_internal(pb, program);
    int main_index = functions_lookup((token_t) { .length = 4, .start = "main" });

    if (main_index != -1) {
        return functions[main_index].ip;
    }

    return -1;
}
