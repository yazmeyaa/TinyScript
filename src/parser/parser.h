//
// Created by evgen on 18.07.2025.
//

#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>
#include "lexer/lexer.h"
struct ast_node* parse(const struct lex_token* tokens, size_t count);
#endif //PARSER_H
