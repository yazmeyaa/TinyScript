#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define MAX_TOKENS 1024

static const char* keywords[] = {"var", "if", "else", "true", "false"};
static enum token_type keyword_tokens[] = {
    TOKEN_VAR, TOKEN_IF, TOKEN_ELSE, TOKEN_TRUE, TOKEN_FALSE
};

static enum token_type match_keyword(const char* start, const size_t length)
{
    for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
    {
        if (strlen(keywords[i]) == length && strncmp(start, keywords[i], length) == 0)
            return keyword_tokens[i];
    }
    return TOKEN_IDENT;
}

static int is_identifier_start(const char c)
{
    return isalpha(c) || c == '_';
}

static int is_identifier_part(const char c)
{
    return isalnum(c) || c == '_';
}

static int is_type_name(const char* start, const size_t length)
{
    return length > 0 && isupper(start[0]);
}

static void add_token(struct lex_token* tokens, size_t* count,
                      const enum token_type type, const char* start, const size_t len,
                      const int line, const int column)
{
    if (*count >= MAX_TOKENS) return;
    tokens[*count].type = type;
    tokens[*count].start = start;
    tokens[*count].length = len;
    tokens[*count].line = line;
    tokens[*count].column = column;
    (*count)++;
}

struct lex_token* parse_text(const char* input, const size_t length, size_t* out_len)
{
    struct lex_token* tokens = calloc(MAX_TOKENS, sizeof(struct lex_token));
    size_t count = 0;
    size_t pos = 0;
    int line = 1, column = 1;

    while (pos < length)
    {
        const char c = input[pos];

        if (isspace(c))
        {
            if (c == '\n')
            {
                line++;
                column = 1;
            }
            else column++;
            pos++;
            continue;
        }

        const char* start = &input[pos];
        const int start_col = column;

        if (is_identifier_start(c))
        {
            const size_t start_pos = pos;
            while (pos < length && is_identifier_part(input[pos]))
            {
                pos++;
                column++;
            }
            const size_t len = pos - start_pos;
            enum token_type type = match_keyword(start, len);
            if (type == TOKEN_IDENT && is_type_name(start, len))
                type = TOKEN_TYPE_NAME;

            add_token(tokens, &count, type, start, len, line, start_col);
            continue;
        }

        if (isdigit(c) || (c == '-' && isdigit(input[pos + 1])))
        {
            const size_t start_pos = pos;
            if (input[pos] == '-')
            {
                pos++;
                column++;
            }
            while (isdigit(input[pos]))
            {
                pos++;
                column++;
            }
            if (input[pos] == '.')
            {
                pos++;
                column++;
                while (isdigit(input[pos]))
                {
                    pos++;
                    column++;
                }
            }
            if (input[pos] == 'e' || input[pos] == 'E')
            {
                pos++;
                column++;
                if (input[pos] == '+' || input[pos] == '-')
                {
                    pos++;
                    column++;
                }
                while (isdigit(input[pos]))
                {
                    pos++;
                    column++;
                }
            }
            add_token(tokens, &count, TOKEN_NUMBER, start, pos - start_pos, line, start_col);
            continue;
        }

        if (c == '"')
        {
            const size_t start_pos = pos++;
            column++;
            while (pos < length && input[pos] != '"')
            {
                if (input[pos] == '\\' && pos + 1 < length) pos++;
                pos++;
                column++;
            }
            if (input[pos] == '"')
            {
                pos++;
                column++;
            }
            else fprintf(stderr, "[lexer] Unterminated string at line %d\n", line);

            add_token(tokens, &count, TOKEN_STRING, start, pos - start_pos, line, start_col);
            continue;
        }

        if (pos + 1 < length)
        {
            const char next = input[pos + 1];
            if (c == ':' && next == '=')
            {
                add_token(tokens, &count, TOKEN_DECL_ASSIGN, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
            if (c == '=' && next == '=')
            {
                add_token(tokens, &count, TOKEN_EQ, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
            if (c == '!' && next == '=')
            {
                add_token(tokens, &count, TOKEN_NEQ, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
            if (c == '<' && next == '=')
            {
                add_token(tokens, &count, TOKEN_LE, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
            if (c == '>' && next == '=')
            {
                add_token(tokens, &count, TOKEN_GE, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
            if (c == '&' && next == '&')
            {
                add_token(tokens, &count, TOKEN_AND, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
            if (c == '|' && next == '|')
            {
                add_token(tokens, &count, TOKEN_OR, start, 2, line, column);
                pos += 2;
                column += 2;
                continue;
            }
        }

        switch (c)
        {
        case '=': add_token(tokens, &count, TOKEN_ASSIGN, start, 1, line, column);
            break;
        case '!': add_token(tokens, &count, TOKEN_NOT, start, 1, line, column);
            break;
        case '+': add_token(tokens, &count, TOKEN_PLUS, start, 1, line, column);
            break;
        case '-': add_token(tokens, &count, TOKEN_MINUS, start, 1, line, column);
            break;
        case '*': add_token(tokens, &count, TOKEN_STAR, start, 1, line, column);
            break;
        case '/': add_token(tokens, &count, TOKEN_SLASH, start, 1, line, column);
            break;
        case '<': add_token(tokens, &count, TOKEN_LT, start, 1, line, column);
            break;
        case '>': add_token(tokens, &count, TOKEN_GT, start, 1, line, column);
            break;
        case '(': add_token(tokens, &count, TOKEN_LPAREN, start, 1, line, column);
            break;
        case ')': add_token(tokens, &count, TOKEN_RPAREN, start, 1, line, column);
            break;
        case '{': add_token(tokens, &count, TOKEN_LBRACE, start, 1, line, column);
            break;
        case '}': add_token(tokens, &count, TOKEN_RBRACE, start, 1, line, column);
            break;
        case '[': add_token(tokens, &count, TOKEN_LBRACKET, start, 1, line, column);
            break;
        case ']': add_token(tokens, &count, TOKEN_RBRACKET, start, 1, line, column);
            break;
        case ',': add_token(tokens, &count, TOKEN_COMMA, start, 1, line, column);
            break;
        case ';': add_token(tokens, &count, TOKEN_SEMICOLON, start, 1, line, column);
            break;
        case ':': add_token(tokens, &count, TOKEN_COLON, start, 1, line, column);
            break;
        case '#':
            while (pos < length && input[pos] != '\n') pos++;
            break;
        default: add_token(tokens, &count, TOKEN_UNKNOWN, start, 1, line, column);
            break;
        }
        pos++;
        column++;
    }

    *out_len = count;
    return tokens;
}

const char* token_type_to_str(enum token_type type) {
    switch (type) {
        // Keywords
        case TOKEN_VAR:        return "TOKEN_VAR";
        case TOKEN_IF:         return "TOKEN_IF";
        case TOKEN_ELSE:       return "TOKEN_ELSE";
        case TOKEN_TRUE:       return "TOKEN_TRUE";
        case TOKEN_FALSE:      return "TOKEN_FALSE";

        // Identifiers & types
        case TOKEN_IDENT:      return "TOKEN_IDENT";
        case TOKEN_TYPE_NAME:  return "TOKEN_TYPE_NAME";

        // Literals
        case TOKEN_NUMBER:     return "TOKEN_NUMBER";
        case TOKEN_STRING:     return "TOKEN_STRING";

        // Symbols
        case TOKEN_LPAREN:     return "TOKEN_LPAREN";
        case TOKEN_RPAREN:     return "TOKEN_RPAREN";
        case TOKEN_LBRACE:     return "TOKEN_LBRACE";
        case TOKEN_RBRACE:     return "TOKEN_RBRACE";
        case TOKEN_LBRACKET:   return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:   return "TOKEN_RBRACKET";
        case TOKEN_COMMA:      return "TOKEN_COMMA";
        case TOKEN_SEMICOLON:  return "TOKEN_SEMICOLON";
        case TOKEN_COLON:      return "TOKEN_COLON";

        // Operators
        case TOKEN_ASSIGN:       return "TOKEN_ASSIGN";
        case TOKEN_DECL_ASSIGN:  return "TOKEN_DECL_ASSIGN";
        case TOKEN_EQ:           return "TOKEN_EQ";
        case TOKEN_NEQ:          return "TOKEN_NEQ";
        case TOKEN_LT:           return "TOKEN_LT";
        case TOKEN_GT:           return "TOKEN_GT";
        case TOKEN_LE:           return "TOKEN_LE";
        case TOKEN_GE:           return "TOKEN_GE";
        case TOKEN_PLUS:         return "TOKEN_PLUS";
        case TOKEN_MINUS:        return "TOKEN_MINUS";
        case TOKEN_STAR:         return "TOKEN_STAR";
        case TOKEN_SLASH:        return "TOKEN_SLASH";
        case TOKEN_AND:          return "TOKEN_AND";
        case TOKEN_OR:           return "TOKEN_OR";
        case TOKEN_NOT:          return "TOKEN_NOT";

        // Special
        case TOKEN_EOF:        return "TOKEN_EOF";
        case TOKEN_UNKNOWN:    return "TOKEN_UNKNOWN";

        default:               return "TOKEN_<INVALID>";
    }
}
