#include <stdlib.h>

#ifndef TS_LEXER_H
#define TS_LEXER_H

enum token_type
{
    TOKEN_LET,
    TOKEN_ASSIGN,
    TOKEN_ASSERT_INIT,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_STRING,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_UNKNOWN,
};

struct lex_token
{
    enum token_type type;
    const char *start;
    size_t length;
};

struct lex_token *parse_text(const char *in_str, const size_t str_len, size_t *out_len);
char* token_type_to_str(enum token_type);
#endif