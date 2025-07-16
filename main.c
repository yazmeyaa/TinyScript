#include "utils/list.h"
#include "lexer/lexer.h"
#include <stdio.h>
#include <string.h>



int main(int argc, char **argv)
{
    struct list list = create_list(sizeof(int));
    if (argc < 2)
    {
        fprintf(stderr, "Missing entrypoint argument.\n");
        abort();
    }
    FILE *fptr;
    char *filename = argv[1];
    fptr = fopen(filename, "rb");
    if (fptr == NULL)
    {
        fprintf(stderr, "Cannot open file %s.\n", filename);
        abort();
    }

    if (fseek(fptr, 0, SEEK_END) != 0)
    {
        fclose(fptr);
        fprintf(stderr, "Error seeking in file.\n");
        abort();
    }

    long text_len = ftell(fptr);
    if (text_len < 0)
    {
        fclose(fptr);
        fprintf(stderr, "Error telling file size.\n");
        abort();
    }

    rewind(fptr);

    char *code = malloc(text_len + 1);
    if (!code)
    {
        fclose(fptr);
        fprintf(stderr, "Allocation error.\n");
        abort();
    }

    size_t read_size = fread(code, 1, text_len, fptr);
    fclose(fptr);

    if (read_size != (size_t)text_len)
    {
        fprintf(stderr, "Read size mismatch.\n");
        free(code);
        abort();
    }

    code[text_len] = '\0';

    size_t lexems_len;
    struct lex_token *lexems = parse_text(code, text_len, &lexems_len);

    for (int i = 0; i < lexems_len; i++)
    {
        struct lex_token t = lexems[i];
        char *token_str = token_type_to_str(t.type);
        printf("type: %s\tposition: %x\tlength:%d\n", token_str, t.start, t.length);
    }

    return 0;
}