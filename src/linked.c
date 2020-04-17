#include "linked.h"

#include <stdlib.h>
#include <stdio.h>

struct linkedElement* linked_createElement(raw_t data) {
	struct linkedElement* element = malloc(sizeof(struct linkedElement));
	if (element == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	
	element->data = data;
	element->next = NULL;
	
	return element;
}

linked_t linked_empty() {
	return (linked_t) { .head = NULL };
}


linked_t linked_fromArray(void** array, size_t length) {
	linked_t list = linked_empty();
	
	struct linkedElement** nextRef = &list.head;
	
	for (size_t i = 0; i < length; i++) {
		*nextRef = linked_createElement((raw_t) array[i]);
		nextRef = &(*nextRef)->next;
	}
	
	return list;
}

linked_t linked_fromSupplier(multisupplier_t supplier) {
	linked_t list = linked_empty();
	
	struct linkedElement** nextRef = &list.head;
	
	supply_t supply = { .hasMore = true, .data = (raw_t) NULL };
	
	while(supply.hasMore) {
		supply = supplier();
		
		*nextRef = linked_createElement(supply.data);
		nextRef = &((*nextRef)->next);
	}
	
	return list;
}

size_t linked_add(linked_t* list, raw_t data) {
	struct linkedElement** nextRef = &(list->head);
	
	size_t position = 0;
	
	while (*nextRef != NULL) {
		nextRef = &((*nextRef)->next);
		position++;
	}
	
	*nextRef = linked_createElement(data);
	
	return position;
}

size_t linked_size(linked_t* list) {
	struct linkedElement* current = list->head;

	size_t size = 0;

	while (current != NULL) {
		size++;
		
		current = current->next;
	}
	
	return size;
}

raw_t linked_get(linked_t* list, size_t index) {
	raw_t result = 0;

	linked_iterate(list, Iterator(i, d, {
		if (i == index) {
			result = d;
			return ITERATOR_BREAK;
		} else {
			return ITERATOR_CONTINUE;
		}	
	}));
	
	return result;
} 

void linked_iterate(linked_t* list, iterator_t iterator) {
	struct linkedElement** refToCurrent = &list->head;
	struct linkedElement* current = list->head;

	size_t index = 0;

	while(current != NULL) {
		iter_result_t result = iterator(index, current->data);
		
		bool skipContinue = false;
		
		switch(result.modify) {
			case DELETE:
				*refToCurrent = current->next;
				
				struct linkedElement* next = current->next;
				free(current);
				current = next;
				
				skipContinue = true;
				break;
				
			case REPLACE:
				current->data = result.data;
				break;
			case KEEP:
				break;
		}
		
		switch(result.step) {
			case BREAK:
				return;
			case CONTINUE:
				if (skipContinue)
					break;
			
				refToCurrent = &current->next;
				current = current->next;
				break;
		}
		
		index++;
	}
}

void linked_clear(linked_t* list) {
	struct linkedElement* current = list->head;
	while (current != NULL) {
		struct linkedElement* next = current->next;
		free(current);
		current = next;
	}
	
	list->head = NULL;
}

void* linked_toArray(linked_t* list, size_t* length) {	
	size_t size = linked_size(list);
	
	if (length != NULL)
		*length = size;
		
	raw_t* array = malloc(sizeof(raw_t) * size);
	if (array == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	
	linked_iterate(list, Iterator(i, data, {
		array[i] = data;
		return ITERATOR_CONTINUE;
	}));
	
	return array;
}

void* linked_toPrimitiveArray(linked_t* list, size_t primitiveSize, size_t* length) {
	size_t size = linked_size(list);
	
	if (length != NULL)
		*length = size;
		
	unsigned char* array = malloc(primitiveSize * size);
	if (array == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	
	linked_iterate(list, Iterator(i, data, {
		for(size_t j = 0; j < primitiveSize; j++) {
			array[i * primitiveSize + j] = (data >> (j * 8)) & 0xff;
		}
		return ITERATOR_CONTINUE;
	}));
	
	return array;
}
