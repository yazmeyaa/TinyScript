#include "parser/parser.h"
#include "ast.h"
#include "lexer/lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils/str.h"

struct parser {
    const struct lex_token* tokens;
    size_t count;
    size_t pos;
};

static const struct lex_token* peek(struct parser* p) {
    return p->pos < p->count ? &p->tokens[p->pos] : NULL;
}

static const struct lex_token* advance(struct parser* p) {
    return p->pos < p->count ? &p->tokens[p->pos++] : NULL;
}

static int match(struct parser* p, enum token_type type) {
    if (peek(p) && peek(p)->type == type) {
        advance(p);
        return 1;
    }
    return 0;
}

static void expect(struct parser* p, enum token_type type) {
    if (!match(p, type)) {
        const char *token1 = token_type_to_str(type);
        const char *token2 = token_type_to_str(peek(p) ? peek(p)->type : -1);
        fprintf(stderr, "Expected token %s, got %s\n", token1, token2);
        exit(1);
    }
}

static struct ast_node* make_binary_node(struct ast_node* left, enum token_type op, struct ast_node* right) {
    struct ast_node* node = malloc(sizeof(*node));
    node->type = AST_EXPRESSION;
    node->binary.left = left;
    node->binary.right = right;
    node->binary.op = op;
    return node;
}

static struct ast_node* parse_expression(struct parser* p);
static struct ast_node* parse_statement(struct parser* p);

static const struct type_annotation* parse_type_annotation(struct parser* p) {
    if (!match(p, TOKEN_TYPE_NAME)) {
        fprintf(stderr, "Expected type name\n");
        exit(1);
    }

    const struct lex_token* tok = &p->tokens[p->pos - 1];
    struct type_annotation* ann = malloc(sizeof(struct type_annotation));
    ann->type_name = ts_strndup(tok->start, tok->length);
    ann->generic_types = NULL;
    ann->generic_count = 0;

    if (match(p, TOKEN_LT)) {
        struct type_annotation** generics = NULL;
        size_t count = 0;
        do {
            generics = realloc(generics, sizeof(struct type_annotation*) * (count + 1));
            generics[count] = parse_type_annotation(p);
            count++;
        } while (match(p, TOKEN_COMMA));
        expect(p, TOKEN_GT);
        ann->generic_types = generics;
        ann->generic_count = count;
    }

    return ann;
}

static struct ast_node* parse_primary(struct parser* p) {
    struct lex_token* tok = peek(p);

    if (!tok) {
        fprintf(stderr, "Unexpected end of input in expression\n");
        exit(1);
    }

    if (tok->type == TOKEN_NUMBER) {
        advance(p);
        struct ast_node* node = malloc(sizeof(*node));
        node->type = AST_EXPRESSION;
        node->number.value = atof(tok->start);
        return node;
    }

    if (tok->type == TOKEN_TRUE || tok->type == TOKEN_FALSE) {
        advance(p);
        struct ast_node* node = malloc(sizeof(*node));
        node->type = AST_EXPRESSION;
        node->boolean.value = (tok->type == TOKEN_TRUE);
        return node;
    }

    if (tok->type == TOKEN_IDENT) {
        advance(p);
        struct ast_node* node = malloc(sizeof(*node));
        node->type = AST_EXPRESSION;
        node->ident.name = ts_strndup(tok->start, tok->length);
        return node;
    }

    if (match(p, TOKEN_LPAREN)) {
        struct ast_node* inner = parse_expression(p);
        expect(p, TOKEN_RPAREN);
        return inner;
    }

    if (match(p, TOKEN_LBRACKET)) {
        struct ast_node* node = malloc(sizeof(*node));
        node->type = AST_EXPRESSION;
        node->list.elements = NULL;
        node->list.element_count = 0;

        if (!match(p, TOKEN_RBRACKET)) {
            do {
                struct ast_node* element = parse_expression(p);
                node->list.elements = realloc(node->list.elements,
                                             sizeof(struct ast_node*) * (node->list.element_count + 1));
                node->list.elements[node->list.element_count++] = element;
            } while (match(p, TOKEN_COMMA));
            expect(p, TOKEN_RBRACKET);
        }
        return node;
    }

    fprintf(stderr, "Unexpected token in expression: %d\n", token_type_to_str(tok->type));
    exit(1);
}

static struct ast_node* parse_unary(struct parser* p) {
    if (match(p, TOKEN_MINUS) || match(p, TOKEN_NOT)) {
        enum token_type op = p->tokens[p->pos - 1].type;
        struct ast_node* expr = parse_unary(p);
        return make_binary_node(NULL, op, expr);
    }
    return parse_primary(p);
}

static struct ast_node* parse_multiplicative(struct parser* p) {
    struct ast_node* left = parse_unary(p);
    while (match(p, TOKEN_STAR) || match(p, TOKEN_SLASH)) {
        enum token_type op = p->tokens[p->pos - 1].type;
        struct ast_node* right = parse_unary(p);
        left = make_binary_node(left, op, right);
    }
    return left;
}

static struct ast_node* parse_additive(struct parser* p) {
    struct ast_node* left = parse_multiplicative(p);
    while (match(p, TOKEN_PLUS) || match(p, TOKEN_MINUS)) {
        enum token_type op = p->tokens[p->pos - 1].type;
        struct ast_node* right = parse_multiplicative(p);
        left = make_binary_node(left, op, right);
    }
    return left;
}

static struct ast_node* parse_comparison(struct parser* p) {
    struct ast_node* left = parse_additive(p);
    while (match(p, TOKEN_LT) || match(p, TOKEN_GT) || match(p, TOKEN_LE) || match(p, TOKEN_GE)) {
        enum token_type op = p->tokens[p->pos - 1].type;
        struct ast_node* right = parse_additive(p);
        left = make_binary_node(left, op, right);
    }
    return left;
}

static struct ast_node* parse_equality(struct parser* p) {
    struct ast_node* left = parse_comparison(p);
    while (match(p, TOKEN_EQ) || match(p, TOKEN_NEQ)) {
        enum token_type op = p->tokens[p->pos - 1].type;
        struct ast_node* right = parse_comparison(p);
        left = make_binary_node(left, op, right);
    }
    return left;
}

static struct ast_node* parse_logical_and(struct parser* p) {
    struct ast_node* left = parse_equality(p);
    while (match(p, TOKEN_AND)) {
        struct ast_node* right = parse_equality(p);
        left = make_binary_node(left, TOKEN_AND, right);
    }
    return left;
}

static struct ast_node* parse_logical_or(struct parser* p) {
    struct ast_node* left = parse_logical_and(p);
    while (match(p, TOKEN_OR)) {
        struct ast_node* right = parse_logical_and(p);
        left = make_binary_node(left, TOKEN_OR, right);
    }
    return left;
}

static struct ast_node* parse_expression(struct parser* p) {
    return parse_logical_or(p);
}

static struct ast_node* parse_declaration(struct parser* p) {
    expect(p, TOKEN_VAR);

    struct lex_token* ident_token = advance(p);
    if (!ident_token || ident_token->type != TOKEN_IDENT) {
        fprintf(stderr, "Expected identifier after 'var'\n");
        exit(1);
    }

    struct type_annotation* type = parse_type_annotation(p);

    struct ast_node* expr = NULL;
    if (match(p, TOKEN_DECL_ASSIGN)) {
        expr = parse_expression(p);
    }

    expect(p, TOKEN_SEMICOLON);

    struct ast_node* node = malloc(sizeof(*node));
    node->type = AST_DECLARATION;
    node->declaration.ident = ts_strndup(ident_token->start, ident_token->length);
    node->declaration.type = type;
    node->declaration.expression = expr;
    return node;
}

static struct ast_node* parse_statement(struct parser* p) {
    if (peek(p)->type == TOKEN_VAR) {
        return parse_declaration(p);
    }

    struct ast_node* expr = parse_expression(p);
    expect(p, TOKEN_SEMICOLON);
    return expr;
}

struct ast_node* parse(const struct lex_token* tokens, size_t count) {
    struct parser p = { .tokens = tokens, .count = count, .pos = 0 };
    return parse_statement(&p);
}
