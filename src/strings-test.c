#include <viscocity.h>

#include "streams.h"
#include "strings.h"
#include "arrays.h"

test(string_strclone, "strclone creates copy of string") {
	const char* string = "Hello World";
	
	char* clone = strclone(string);
	
	assertNotNull(clone, "clone is not null");
	assertStringEqual(clone, string, "strings are equal");
	
	free(clone);
}

test(string_stringEqualsPredicate, "stringEqualsPredicate") {
	const char* array[] = {"Hello", "World"};
	
	assertCallExact(2, "forEach call assertion");
	
	ptrStream((void*) array, 2)
		.filter(stringEqualsPredicate("World"))
		.forEach(Consumer(s, {
			assertCallExact(2, "forEach call assertion");
			
			assertStringEqual((char*) s, "World", "string is \"World\"");
		}));
}

test(string_stringNotEqualsPredicate, "stringNotEqualsPredicate") {
	const char* array[] = {"Hello", "World"};
	
	assertCallExact(2, "forEach call assertion");
	
	ptrStream((void*) array, 2)
		.filter(stringNotEqualsPredicate("World"))
		.forEach(Consumer(s, {
			assertCallExact(2, "forEach call assertion");
			
			assertStringEqual((char*) s, "Hello", "string is \"Hello\"");
		}));
}

test(string_stringEmptyPredicate, "stringEmptyPredicate") {
	const char* array[] = {"Hello", "", "World"};
	
	assertCallExact(2, "forEach call assertion");
	
	ptrStream((void*) array, 3)
		.filter(stringEmptyPredicate())
		.forEach(Consumer(s, {
			assertCallExact(2, "forEach call assertion");
			
			assertStringEqual((char*) s, "", "string is empty");
		}));
}

test(string_stringNotEmptyPredicate, "stringNotEmptyPredicate") {
	const char* array[] = {"Hello", "", "World"};
	
	assertCallExact(3, "forEach call assertion");
	
	ptrStream((void*) array, 3)
		.filter(stringNotEmptyPredicate())
		.forEach(Consumer(s, {
			assertCallExact(3, "forEach call assertion");
			
			assertTrue(strcmp((char*) s, "Hello") == 0 || strcmp((char*) s, "World") == 0, "s is one of the elements in the stream");
		}));
}
