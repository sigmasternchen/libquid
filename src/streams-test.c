#include <viscocity.h>

#include "streams.h"
#include "strings.h"
#include "arrays.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


test(stream_forEach, "forEach get's called for every element in stream") {
	const char* strings[] = {"Hello", "World"};
	
	assertCallExact(3, "forEach call assertion");
	
	ptrStream((void*) strings, 2)
		.forEach(Consumer(s, {
			assertCallExact(3, "forEach call assertion");
			
			assertTrue(strcmp((char*) s, "Hello") == 0 || strcmp((char*) s, "World") == 0, "s is one of the elements in the stream");
		}));
}

test(stream_peek, "peek get's called for every element in stream") {
	const char* strings[] = {"Hello", "World"};
	
	assertCallExact(3, "peek call assertion");
	
	ptrStream((void*) strings, 2)
		.peek(Consumer(s, {
			assertCallExact(3, "peek call assertion");
			
			assertTrue(strcmp((char*) s, "Hello") == 0 || strcmp((char*) s, "World") == 0, "s is one of the elements in the stream");
		})).forEach(Consumer(s, {}));
}

test(stream_map, "map sees and replaces all elements in the stream") {
	int numbers[] = {1, 2, 3};
	
	assertCallExact(4, "forEach call assertion");
	
	primitiveStream((void*) numbers, 3, ofType(int))
		.map(qFunction(i, i*i))
		.forEach(Consumer(i, {
			assertCallExact(4, "forEach call assertion");
			assertTrue(i == 1 || i == 4 || i == 9, "i is one of the elements in the stream");
		}));
}

test(stream_filter, "filter removes non-matching elements from stream") {
	const char* strings[] = {"Hello", "World"};
	
	assertCallExact(2, "forEach call assertion");
	
	ptrStream((void*) strings, 2)
		.filter(qPredicate(s, ((char*) s)[0] == 'W'))
		.forEach(Consumer(s, {
			assertStringEqual((char*) s, "World", NULL);
			assertCallExact(2, "forEach call assertion");
		}));
}

test(stream_limit, "limit aborts a stream after a certain number of elements") {
	const char* strings[] = {"Hello", "World", "this", "is", "a", "test"};
	
	assertCallExact(3, "forEach call assertion");
	
	ptrStream((void*) strings, 6)
		.limit(2)
		.forEach(Consumer(s, {
			assertCallExact(3, "forEach call assertion");
			assertTrue(strcmp((char*) s, "Hello") == 0 || strcmp((char*) s, "World") == 0, "s is one of the elements in the stream");
		}));
}

test(stream_skip, "skip over a certain number of elements") {
	const char* strings[] = {"Hello", "World", "this", "is", "a", "test"};
	
	assertCallExact(3, "forEach call assertion");
	
	ptrStream((void*) strings, 6)
		.skip(4)
		.forEach(Consumer(s, {
			assertCallExact(3, "forEach call assertion");
			assertTrue(strcmp((char*) s, "a") == 0 || strcmp((char*) s, "test") == 0, "s is one of the elements in the stream");
		}));
}




test(stream_count, "count reports correct number") {
	const char* strings[] = {"Hello", "World", "this", "is", "a", "test"};
	
	size_t count = ptrStream((void*) strings, 6)
		.count();
		
	assertTrue(count == 6, "count is correct");
}

test(stream_count_zero, "count reports correct number on empty stream") {
	const char* strings[] = {};
	
	size_t count = ptrStream((void*) strings, 0)
		.count();
		
	assertTrue(count == 0, "count is correct");
}

test(stream_max, "get max value of stream") {
	int numbers[] = {42, 1337, 420};
	
	optional_t max = primitiveStream((void*) numbers, 3, ofType(int))
		.max(qComparator(i1, i2, i1 > i2));
		
	assertTrue(max.present, "there is a max value");
	assertTrue(max.value == 1337, "max is correct");
}

test(stream_max_empty, "get max value of empty stream") {
	int numbers[] = {};
	
	optional_t max = primitiveStream((void*) numbers, 0, ofType(int))
		.max(qComparator(i1, i2, i1 > i2));
		
	assertFalse(max.present, "there is no max value");
}

test(stream_min, "get min value of stream") {
	int numbers[] = {42, 1337, 420};
	
	optional_t min = primitiveStream((void*) numbers, 3, ofType(int))
		.min(qComparator(i1, i2, i1 > i2));
	
	assertTrue(min.present, "there is a min value");
	assertTrue(min.value == 42, "max is correct");
}

test(stream_min_empty, "get min value of empty stream") {
	int numbers[] = {};
	
	optional_t min = primitiveStream((void*) numbers, 0, ofType(int))
		.min(qComparator(i1, i2, i1 > i2));
		
	assertFalse(min.present, "there is no min value");
}

test(stream_allMatch_positive, "allMatch correctly reports positive matches") {
	int numbers[] = {2, 2, 3};
	
	assertTrue(primitiveStream((void*) numbers, 3, ofType(int))
		.allMatch(qPredicate(i, i > 1)), "are all numbers greater than 1");
}

test(stream_allMatch_negative, "allMatch correctly reports negative matches") {
	int numbers[] = {2, 1, 3};
	
	assertFalse(primitiveStream((void*) numbers, 3, ofType(int))
		.allMatch(qPredicate(i, i > 1)), "are not all numbers greater than 1");
}

test(stream_anyMatch_positive, "anyMatch correctly reports positive matches") {
	int numbers[] = {2, 2, 3};
	
	assertTrue(primitiveStream((void*) numbers, 3, ofType(int))
		.anyMatch(qPredicate(i, i > 2)), "one of the elements is > 2");
}

test(stream_anyMatch_negative, "anyMatch correctly reports negative matches") {
	int numbers[] = {2, 1, 3};
	
	assertFalse(primitiveStream((void*) numbers, 3, ofType(int))
		.allMatch(qPredicate(i, i > 3)), "none of the elements is > 3");
}

test(stream_collect_array, "arrayCollector correctly builds an array of all elements in the stream") {
	int numbers[] = {42, 1337, 420};
	
	arrayWithLength_t* array = (void*) primitiveStream((void*) numbers, 3, ofType(int))
		.collect(arrayCollector(ofType(int)));
		
	assertTrue(array->length == 3, "resulting array length is okay");
		
	for (int i = 0; i < array->length; i++) {
		assertTrue(((int*) (array->array))[i] == numbers[i], "checking values in array");
	}
	
	free(array->array);
	free(array);
}

test(stream_collect_string, "stringCollector correctly build a string of all elements in the stream") {
	char characters[] = "Hello World";
	
	char* string = (char*) charStream(characters)
		.collect(stringCollector());
		
	assertStringEqual(characters, string, "strings match");
	
	free(string);	
}

test(stream_forEach_empty, "forEach doesn't get called on empty stream") {
	const char* strings[] = {};
	
	assertCallExact(1, "forEach call assertion");
	
	ptrStream((void*) strings, 0)
		.forEach(Consumer(s, {
			assertCallExact(1, "forEach call assertion");
		}));
}

test(stream_allMatch_empty, "allMatch on empty stream returns true") {
	const char* strings[] = {};
	
	assertTrue(ptrStream((void*) strings, 0)
		.allMatch(qPredicate(s, true)), "allMatch returns true");
}

test(stream_collect_array_empty, "arrayCollector returns empty array on empty stream") {
	int numbers[] = {};
	
	arrayWithLength_t* array = (void*) primitiveStream((void*) numbers, 0, ofType(int))
		.collect(arrayCollector(ofType(int)));
		
	assertTrue(array->length == 0, "array length is 0");
	
	free(array->array);
	free(array);
}

test(stream_collect_string_empty, "stringCollector returns empty string on empty stream") {
	char characters[] = "";
	
	char* string = (char*) charStream(characters)
		.collect(stringCollector());
		
	assertNotNull(string, "string is not null");
	assertStringEqual(string, "", "string is empty");
	
	free(string);	
}




test(stream_iterate, "stream from iterate works") {
	assertCallExact(4, "forEach call assertion");

	Stream.iterate(1, qFunction(last, 2*last))
		.limit(3)
		.forEach(Consumer(i, {
			assertCallExact(4, "forEach call assertion");
			
			assertTrue(i == 1 || i == 2 || i == 4, "correct values in stream");
		}));
}

test(stream_generate, "stream from generate works") {
	assertCallExact(5, "supplier call assertion");
	assertCallExact(4, "forEach call assertion");

	int i = 1;
	Stream.generate(Supplier({
			assertCallExact(5, "supplier call assertion");
			
			i *= 2;
			return i;
		})).limit(3)
		.forEach(Consumer(i, {
			assertCallExact(4, "forEach call assertion");
			
			assertTrue(i == 2 || i == 4 || i == 8, "correct values in stream");
		}));
}

test(stream_empty, "empty stream") {
	size_t count = Stream.empty().count();
	
	assertTrue(count == 0, "count is correct");
}
