#ifndef LINKED_H
#define LINKED_H

#include "functional.h"

struct linkedElement {
	struct linkedElement* next;
	raw_t data;
};

typedef struct linked {
	struct linkedElement* head;
} linked_t;

linked_t linked_empty();
linked_t linked_fromArray(void**, size_t);
linked_t linked_fromSupplier(multisupplier_t);

size_t linked_add(linked_t* list, raw_t data);
size_t linked_size(linked_t* list);
raw_t linked_get(linked_t*, size_t);

void linked_iterate(linked_t* list, iterator_t iterator);
void linked_clear(linked_t* list);

void* linked_toArray(linked_t*, size_t*);
void* linked_toPrimitiveArray(linked_t*, size_t, size_t*);

#endif
