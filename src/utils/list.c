#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#define INITIAL_LIST_CAPACITY 4

struct list create_list(size_t items_size)
{
    struct list list;
    list.items_size = items_size;
    list.capacity = INITIAL_LIST_CAPACITY;
    list.length = 0;
    list.items = malloc(list.items_size * INITIAL_LIST_CAPACITY);
    if (list.items == NULL)
    {
        fprintf(stderr, "Failed to allocate memory in create_list\n");
        abort();
    }

    return list;
}

void list_add_item(struct list *list, const void *item)
{
    if (list->length >= list->capacity)
    {
        list->capacity *= 2;
        void *new_list = realloc(list->items, list->items_size * list->capacity);
        if (list->items == NULL)
        {
            fprintf(stderr, "Failed to reallocate memory in add_item\n");
            abort();
        }
        list->items = new_list;
    }
    if (list->items == NULL)
    {
        fprintf(stderr, "Failed to allocate memory in add_item\n");
        abort();
    }

    void *target = (char *)list->items + (list->length * list->items_size);
    memcpy(target, item, list->items_size);
    list->length++;
}

void list_get_item(const struct list *list, const size_t index, void *dist)
{
    if (index >= list->length)
    {
        fprintf(stderr, "Index %zu out of bounds in get_item\n", index);
        abort();
    }
    const void *item = (char *)list->items + (index * list->items_size);
    memcpy(dist, item, list->items_size);
}

void list_set_item(const struct list *list, const size_t index, const void *item)
{
    if (index >= list->length)
    {
        fprintf(stderr, "Index %zu out of bounds in get_item\n", index);
        abort();
    }
    void *target = (char *)list->items + (index * list->items_size);
    memcpy(target, item, list->items_size);
}

void *list_to_array(const struct list *list, size_t *len)
{
    size_t total_size = list->items_size * list->length;
    void *arr = malloc(total_size);
    if(arr == NULL) {
        fprintf(stderr, "Failed to allocate memory in list_to_array\n");
        abort();
    }

    memcpy(arr, list->items, total_size);
    
    *len = list->length;
    return arr;
}
