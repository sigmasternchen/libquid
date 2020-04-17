#include "functional.h"

#include "streams.h"
#include "strings.h"
#include "arrays.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define ARRAY_LENGTH (5)

int main(int argc, char** argv) {
	const char* array[ARRAY_LENGTH] = {
		"Hello", "World", "with", "no", "Streams"
	};

	ptrStream((void*) array, ARRAY_LENGTH)
		.peek(qConsumer(obj, printf("%s\n", (char*) obj)))
		.filter(stringNotEqualsPredicate("no"))
		.map(qFunction(obj, (raw_t) charStream((char*) obj)
			.map(qFunction(c, toupper((char) c)))
			.collect(stringCollector())
		)).forEach(Consumer(obj, {
			printf("%s\n", (char*) obj);
			free((char*) obj);
		}));
	

	return 0;
}
