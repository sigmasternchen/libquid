#include <viscocity.h>

#include "functional.h"
#include "linked.h"
#include "arrays.h"

test(linked_empty, "linked_empty creates new linked_t with size 0") {
	linked_t list = linked_empty();
	
	assertTrue(linked_size(&list) == 0, "list has size 0");
}

test(linked_add_get_clear, "add, get and clear elements from list") {
	linked_t list = linked_empty();
	assertTrue(linked_size(&list) == 0, "list has size 0");
	
	linked_add(&list, 42);
	assertTrue(linked_size(&list) == 1, "list has size 1");
	
	assertTrue(linked_get(&list, 0) == 42, "get correct value from list");
	
	linked_add(&list, 1337);
	assertTrue(linked_size(&list) == 2, "list has size 1");
	
	assertTrue(linked_get(&list, 1) == 1337, "get correct value from list");
	
	linked_clear(&list);
	assertTrue(linked_size(&list) == 0, "list has size 0");
}

test(linked_get_empty, "get non-existing entry returns 0") {
	linked_t list = linked_empty();
	assertTrue(linked_size(&list) == 0, "list has size 0");
	
	assertTrue(linked_get(&list, 0) == 0, "get correct value from list");
}

test(linked_fromArray_get, "create list from array and get elements") {
	const char* array[] = {"Hello", "World", "foobar"};
	
	linked_t list = linked_fromArray((void**) array, 3);
	assertTrue(linked_size(&list) == 3, "list has size 3");
	
	assertStringEqual((char*) linked_get(&list, 0), "Hello", "first element in list is \"Hello\"");
	assertStringEqual((char*) linked_get(&list, 1), "World", "second element in list is \"World\"");
	assertStringEqual((char*) linked_get(&list, 2), "foobar", "third element in list is \"foobar\"");
	
	linked_clear(&list);
}

test(linked_fromSupplier_get, "create list from supplier and get elements") {
	const char* array[] = {"Hello", "World", "foobar"};
	int i = 0;
	
	assertCallExact(4, "supplier call assertion");
	
	linked_t list = linked_fromSupplier(lambda(supply_t, () {
		assertCallExact(4, "supplier call assertion");
	
		supply_t supply;
		supply.data = (raw_t) array[i];
		supply.hasMore = ++i < 3;
		
		return supply;
	}));
	
	assertTrue(linked_size(&list) == 3, "list has size 3");
	
	assertStringEqual((char*) linked_get(&list, 0), "Hello", "first element in list is \"Hello\"");
	assertStringEqual((char*) linked_get(&list, 1), "World", "second element in list is \"World\"");
	assertStringEqual((char*) linked_get(&list, 2), "foobar", "third element in list is \"foobar\"");
	
	linked_clear(&list);
}

test(linked_toArray, "get array from list") {
	const char* originalArray[] = {"Hello", "World", "foobar"};
	
	linked_t list = linked_fromArray((void**) originalArray, 3);
	
	size_t size;
	const char** array = linked_toArray(&list, &size);	
	assertTrue(size == 3, "size of new array is correct");

	for (int i = 0; i < size; i++) {
		assertStringEqual(array[0], originalArray[0], "value in new array is correct");
	}

	linked_clear(&list);
	free(array);
}

test(linked_toPrimitiveArray, "get primitive array from list") {
	linked_t list = linked_empty();
	
	linked_add(&list, 42);
	linked_add(&list, 1337);
	linked_add(&list, 420);
	
	size_t size;
	int* array = linked_toPrimitiveArray(&list, ofType(int), &size);
	
	assertTrue(size == 3, "size of new array is correct");
	
	assertTrue(array[0] == 42, "value in array is correct");
	assertTrue(array[1] == 1337, "value in array is correct");
	assertTrue(array[2] == 420, "value in array is correct");
	
	linked_clear(&list);
	free(array);
}

test(linked_iterate_continue, "iterator gets called for all elements in list on continue") {
	const char* array[] = {"Hello", "World", "foobar"};
	
	linked_t list = linked_fromArray((void**) array, 3);
	
	assertCallExact(4, "iterator call assertion");
	
	linked_iterate(&list, lambda(iter_result_t, (size_t i, raw_t data) {
		assertCallExact(4, "iterator call assertion");
	
		assertStringEqual((const char*) data, array[i], "value in array is correct");
		
		return ITERATOR_CONTINUE;
	}));
}
