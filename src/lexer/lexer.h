#ifndef TS_LEXER_H
#define TS_LEXER_H
#include <stdlib.h>

enum token_type
{
    // Keywords
    TOKEN_VAR,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_TRUE,
    TOKEN_FALSE,

    // Types
    TOKEN_TYPE_NAME,

    // Identifiers
    TOKEN_IDENT,

    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,

    // Symbols
    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )
    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // }
    TOKEN_LBRACKET, // [
    TOKEN_RBRACKET, // ]
    TOKEN_COMMA, // ,
    TOKEN_SEMICOLON, // ;
    TOKEN_COLON, // :

    // Operators
    TOKEN_ASSIGN, // =
    TOKEN_DECL_ASSIGN, // :=
    TOKEN_EQ, // ==
    TOKEN_NEQ, // !=
    TOKEN_LT, // <
    TOKEN_GT, // >
    TOKEN_LE, // <=
    TOKEN_GE, // >=
    TOKEN_PLUS, // +
    TOKEN_MINUS, // -
    TOKEN_STAR, // *
    TOKEN_SLASH, // /
    TOKEN_AND, // &&
    TOKEN_OR, // ||
    TOKEN_NOT, // !

    // Misc
    TOKEN_EOF,
    TOKEN_UNKNOWN
};

struct lex_token
{
    enum token_type type;
    const char* start;
    size_t length;
    int line;
    int column;
};

struct lex_token *parse_text(const char *input, size_t length, size_t *out_len);
const char* token_type_to_str(enum token_type);
#endif
