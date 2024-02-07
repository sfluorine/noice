#pragma once

#include "token.h"

void lexer_init(const char* source);

token_t get_token();
