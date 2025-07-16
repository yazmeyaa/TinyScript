#include <stdlib.h>

#ifndef TS_LIST_H
#define TS_LIST_H

struct list
{
    size_t length;
    size_t capacity;
    size_t items_size;
    void **items;
};

struct list create_list(size_t items_size);
void list_add_item(struct list *list, const void *item);
void list_get_item(const struct list *list, size_t index, void *dist);
void list_set_item(const struct list *list, size_t index, const void *item);
void *list_to_array(const struct list *list, size_t *len);
#endif