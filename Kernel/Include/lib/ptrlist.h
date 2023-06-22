#pragma once
#include <lib/types.h>

typedef struct
{
    void** entries;
    size_t count;
} ptrlist_t;

#define NULL_PTRLIST (ptrlist_t){ NULL, 0 }

ptrlist_t ptrlist_create();
ptrlist_t ptrlist_dup(ptrlist_t* list);
void ptrlist_free(ptrlist_t* list, bool free_entries);
void ptrlist_clear(ptrlist_t* list, bool free_entries);
void* ptrlist_add(ptrlist_t* list, void* value);
bool  ptrlist_remove(ptrlist_t* list, int index, bool free_ptr);
void* ptrlist_get(ptrlist_t* list, int index);
