//
// Created by evgen on 19.07.2025.
//

#include "str.h"
#include <stdlib.h>
#include <string.h>

char* ts_strndup(const char* s, size_t n) {
    char* p = malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}