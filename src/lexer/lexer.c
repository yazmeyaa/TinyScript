#include "lexer.h"
#include "utils/list.h"
#include <stdio.h>
#include <ctype.h>

void add_token(struct list *list, enum token_type type, const char *start, size_t length)
{
    struct lex_token token = {
        .length = length,
        .start = start,
        .type = type,
    };

    list_add_item(list, &token);
}

char *token_type_to_str(enum token_type token_type)
{
    if (isalpha)
        switch (token_type)
        {
        case TOKEN_PLUS:
            return "PLUS";
        case TOKEN_MINUS:
            return "MINUS";
        case TOKEN_MULTIPLY:
            return "MULTIPLY";
        case TOKEN_DIVIDE:
            return "DIVIDE";
        case TOKEN_NUMBER:
            return "NUMBER";
        case TOKEN_STRING:
            return "STRING";
        case TOKEN_LPAREN:
            return "LPAREN";
        case TOKEN_RPAREN:
            return "RPAREN";
        case TOKEN_SEMICOLON:
            return "SEMICOLON";
        case TOKEN_UNKNOWN:
            return "UNKNOWN_TOKEN";
        default:
            return "NOTHING";
            break;
        }
}

struct lex_token *parse_text(const char *in_str, const size_t str_len, size_t *out_len)
{
    const size_t STRUCT_SIZE = sizeof(struct lex_token);
    struct list list = create_list(STRUCT_SIZE);

    size_t i = 0;
    while (i < str_len)
    {
        char c = in_str[i];
        if (isspace(c))
        {
            i++;
            continue;
        }

        if (c == '"')
        {
            size_t start = i++;
            while (i < str_len && in_str[i] != '"')
            {
                if (in_str[i] == '\\')
                    i++;
                i++;
            }
            i++;
            add_token(&list, TOKEN_STRING, in_str + start, i - start);
            continue;
        }

        if (isdigit(c))
        {
            size_t start = i;
            while (i < str_len && isdigit(in_str[i]))
                i++;
            if (i < str_len && in_str[i] == '.')
            {
                i++;
                while (i < str_len && isdigit(in_str[i]))
                    i++;
            }
            add_token(&list, TOKEN_NUMBER, in_str + start, i - start);
            continue;
        }

        switch (c)
        {
        case '+':
            add_token(&list, TOKEN_PLUS, in_str + i, 1);
            i++;
            break;
        case '-':
            add_token(&list, TOKEN_MINUS, in_str + i, 1);
            i++;
            break;
        case '*':
            add_token(&list, TOKEN_MULTIPLY, in_str + i, 1);
            i++;
            break;
        case '/':
            add_token(&list, TOKEN_DIVIDE, in_str + i, 1);
            i++;
            break;
        case '(':
            add_token(&list, TOKEN_LPAREN, in_str + i, 1);
            i++;
            break;
        case ')':
            add_token(&list, TOKEN_RPAREN, in_str + i, 1);
            i++;
            break;
        case ',':
            add_token(&list, TOKEN_COMMA, in_str + i, 1);
            i++;
            break;
        case ';':
            add_token(&list, TOKEN_SEMICOLON, in_str + i, 1);
            i++;
            break;
        default:
            add_token(&list, TOKEN_UNKNOWN, in_str + i, 1);
            i++;
            break;
        }
    }

    return list_to_array(&list, out_len);
}