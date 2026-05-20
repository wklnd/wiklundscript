// src/parser.h
#pragma once

#include "lexer.h"
#include "ast.h"

Program parser_parse(TokenList tokens);