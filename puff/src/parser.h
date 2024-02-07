#pragma once

#include "ast.h"

void parser_init(const char* source);
expr_t* parse_expression();
stmt_t* parse_statement();
topdecl_t* parse_topdecl();
program_t* parse_program();
