#pragma once

#include "ast.h"
#include "vm.h"

void codegen_init(npb_t* pb);
void codegen_expr(npb_t* pb, expr_t* expr);
void codegen_stmt(npb_t* pb, stmt_t* stmt);
void codegen_topdecl(npb_t* pb, topdecl_t* topdecl);
int codegen_program(npb_t* pb, program_t* program);
