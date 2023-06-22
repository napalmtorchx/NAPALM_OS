#include <lib/ptrlist.h>
#include <core/kernel.h>

ptrlist_t ptrlist_create()
{
    ptrlist_t list = NULL_PTRLIST;
    return list;
}

ptrlist_t ptrlist_dup(ptrlist_t* list)
{
    if (list == NULL || list->entries == NULL || list->count == 0) { return ptrlist_create(); }
    ptrlist_t outlist = (ptrlist_t){ malloc(sizeof(void*) * list->count), list->count };
    memcpy(outlist.entries, list->entries, sizeof(void*) * list->count);
    return outlist;
}

void ptrlist_free(ptrlist_t* list, bool free_entries)
{
    if (list == NULL) { debug_exception("ptrlist_free(%p, %d) - Attempt to free null pointer list", list, free_entries); return; }
    if (list->entries == NULL) { return; }
    if (free_entries) { for (size_t i = 0; i < list->count; i++) { free(list->entries[i]); } }
    free(list->entries);
    *list = NULL_PTRLIST;
}

void ptrlist_clear(ptrlist_t* list, bool free_entries) { ptrlist_free(list, free_entries); }

void* ptrlist_add(ptrlist_t* list, void* value)
{
    if (list == NULL) { debug_exception("ptrlist_add(%p, %p) - Attempt to add entry to null pointer list", list, value); return NULL; }

    void** entries = malloc(sizeof(void*) * (list->count + 1));
    if (list->entries != NULL) { memcpy(entries, list->entries, sizeof(void*) * list->count); free(list->entries); }

    list->entries = entries;
    list->entries[list->count] = value;
    list->count++;
    return list->entries[list->count - 1];
}

bool ptrlist_remove(ptrlist_t* list, int index, bool free_ptr)
{
    if (list == NULL) { debug_exception("ptrlist_remove(00000000, %d, %d) - Attempt to erase entry from null pointer list", index, free_ptr); return false; }
    if (index == 0 && list->count == 1) { ptrlist_free(list, free_ptr); return true; }

    int i, j = 0;
    void** entries = malloc(sizeof(void*) * (list->count - 1));
    for (i = 0; i < list->count; i++) { if ((size_t)i != (size_t)index) { entries[j++] = list->entries[i]; } }

    free(list->entries);
    list->entries = entries;
    list->count--;
    return true;
}

void* ptrlist_get(ptrlist_t* list, int index)
{
    if (list == NULL) { debug_exception("ptrlist_get(%p, %d) - Null list pointer", list, index); return NULL; }
    if (index < 0 || index >= (int)list->count) { debug_exception("ptrlist_get(%p, %d) - Index out of bounds", list, index); return NULL; }
    return list->entries[index];
}

