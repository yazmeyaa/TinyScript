#include "lexer/lexer.h"
#include <stdio.h>
#include "utils/fs.h"
#include <string.h>

#include "parser/ast.h"
#include "parser/parser.h"

int main(const int argc, const char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Missing entrypoint argument.\n");
        abort();
    }
    const char *filename = argv[1];
    const char *file_contents = read_file(filename);
    if (!file_contents) {
        fprintf(stderr, "read_file returned NULL\n");
        return 1;
    }
    const size_t file_size = strlen(file_contents);

    size_t lex_token_size;
    const struct lex_token *lex_token = parse_text(file_contents, file_size, &lex_token_size);


    // for (int i = 0; i < lex_token_size; i++)
    // {
    //     const struct lex_token t = lex_token[i];
    //     const char *token_str = token_type_to_str(t.type);
    //     char raw_token_text[t.length];
    //     memcpy(raw_token_text, t.start, t.length);
    //     raw_token_text[t.length] = '\0';
    //     printf("token_type: %s: \"%s\"\n", token_str, raw_token_text);
    // }

    const struct ast_node *ast_node = parse(lex_token, lex_token_size);
    print_ast(ast_node);

    return 0;
}