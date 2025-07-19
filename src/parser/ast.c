//
// Created by evgen on 18.07.2025.
//
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

static void print_indent(int level) {
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
}

static void print_type_annotation(const struct type_annotation* type, int level) {
    if (!type) {
        print_indent(level);
        printf("null\n");
        return;
    }
    print_indent(level);
    printf("Type: %s", type->type_name);
    if (type->generic_count > 0) {
        printf("<");
        for (size_t i = 0; i < type->generic_count; i++) {
            if (i > 0) printf(", ");
            print_type_annotation(type->generic_types[i], level);
        }
        printf(">");
    }
    printf("\n");
}

void print_ast(const struct ast_node* node) {
    static int indent_level = 0;

    if (!node) {
        print_indent(indent_level);
        printf("null\n");
        return;
    }

    print_indent(indent_level);
    switch (node->type) {
        case AST_DECLARATION: {
            printf("Declaration:\n");
            indent_level++;
            print_indent(indent_level);
            printf("Identifier: %s\n", node->declaration.ident);
            print_indent(indent_level);
            printf("Type Annotation:\n");
            indent_level++;
            print_type_annotation(node->declaration.type, indent_level);
            indent_level--;
            print_indent(indent_level);
            printf("Expression:\n");
            indent_level++;
            print_ast(node->declaration.expression);
            indent_level--;
            indent_level--;
            break;
        }
        case AST_EXPRESSION: {
            printf("Expression:\n");
            indent_level++;
            if (node->number.value) {
                print_indent(indent_level);
                printf("Number: %f\n", node->number.value);
            } else if (node->boolean.value) {
                print_indent(indent_level);
                printf("Boolean: %s\n", node->boolean.value ? "true" : "false");
            } else if (node->ident.name) {
                print_indent(indent_level);
                printf("Identifier: %s\n", node->ident.name);
            } else if (node->list.element_count > 0 || node->list.elements) {
                print_indent(indent_level);
                printf("List:\n");
                indent_level++;
                for (size_t i = 0; i < node->list.element_count; i++) {
                    print_indent(indent_level);
                    printf("Element %zu:\n", i);
                    indent_level++;
                    print_ast(node->list.elements[i]);
                    indent_level--;
                }
                indent_level--;
            } else if (node->binary.op) {
                print_indent(indent_level);
                printf("Binary Operation: %s\n", token_type_to_str(node->binary.op));
                indent_level++;
                print_indent(indent_level);
                printf("Left:\n");
                indent_level++;
                print_ast(node->binary.left);
                indent_level--;
                print_indent(indent_level);
                printf("Right:\n");
                indent_level++;
                print_ast(node->binary.right);
                indent_level--;
                indent_level--;
            } else {
                print_indent(indent_level);
                printf("Empty Expression\n");
            }
            indent_level--;
            break;
        }
        default:
            print_indent(indent_level);
            printf("Unknown node type: %d\n", node->type);
            break;
    }
}

void free_ast(struct ast_node* node) {
    if (!node) return;

    switch (node->type) {
        case AST_DECLARATION: {
            free((char*)node->declaration.ident);
            if (node->declaration.type) {
                free((char*)node->declaration.type->type_name);
                for (size_t i = 0; i < node->declaration.type->generic_count; i++) {
                    if (node->declaration.type->generic_types[i]) {
                        free((char*)node->declaration.type->generic_types[i]->type_name);
                        free(node->declaration.type->generic_types[i]);
                    }
                }
                free(node->declaration.type->generic_types);
                free(node->declaration.type);
            }
            free_ast(node->declaration.expression);
            break;
        }
        case AST_EXPRESSION: {
            if (node->ident.name) {
                free((char*)node->ident.name);
            } else if (node->list.elements) {
                for (size_t i = 0; i < node->list.element_count; i++) {
                    free_ast(node->list.elements[i]);
                }
                free(node->list.elements);
            } else if (node->binary.op) {
                free_ast(node->binary.left);
                free_ast(node->binary.right);
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown node type: %d\n", node->type);
            break;
    }
    free(node);
}