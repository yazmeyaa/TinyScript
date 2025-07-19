#ifndef AST_H
#define AST_H
#include "lexer/lexer.h"

enum ast_node_type
{
    AST_DECLARATION,
    AST_EXPRESSION,
};

enum data_type
{
    DT_INT8,
    DT_INT16,
    DT_INT32,
    DT_INT64,
    DT_UINT8,
    DT_UINT16,
    DT_UINT32,
    DT_UINT64,
    DT_FLOAT,
    DT_DOUBLE,
    DT_STRING,
    DT_CHAR,
    DT_BOOLEAN,
    DT_LIST,
};

struct ast_node;

struct type_annotation
{
    const char* type_name;
    struct type_annotation** generic_types; // Изменено на type_annotation*
    size_t generic_count;
};

struct declaration_statement
{
    const char* ident;
    struct type_annotation* type;
    struct ast_node* expression;
};

struct list
{
    struct ast_node** elements;
    size_t element_count;
};

struct number
{
    double value;
};

struct boolean
{
    int value;
};

struct ident
{
    const char* name;
};

struct binary
{
    struct ast_node* left;
    struct ast_node* right;
    enum token_type op;
};

struct ast_node
{
    enum ast_node_type type;

    union
    {
        struct declaration_statement declaration;
        struct list list;
        struct number number;
        struct boolean boolean;
        struct ident ident;
        struct binary binary;
    };
};

void print_ast(const struct ast_node*);
void free_ast(struct ast_node*);

#endif