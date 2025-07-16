#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* filename)
{
    FILE* fd = fopen(filename, "rb");

    if (!fd)
    {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }
    if (fseek(fd, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Could not seek to end of file %s\n", filename);
        fclose(fd);
        return NULL;
    }
    const long size = ftell(fd);
    if (size < 0)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        fclose(fd);
        return NULL;
    }

    char* src = malloc(size + 1);
    if (!src)
    {
        fprintf(stderr, "Could not allocate memory for file %s\n", filename);
        return NULL;
    }

    fseek(fd, 0, SEEK_SET);
    const size_t read_size = fread(src, 1, size, fd);
    fclose(fd);
    src[read_size] = '\0';

    return src;
}
