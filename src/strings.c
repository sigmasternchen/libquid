#include "strings.h"
#include "arrays.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

stream_t charStream(const char* string) {
	return primitiveStream((char*) string, strlen(string), ofType(char));
}

char* strclone(const char* string) {
	char* clone = malloc(strlen(string) + 1);
	if (clone == NULL) {
		fprintf(stderr, "Fuck!\n");
		exit(1);
	}
	strcpy(clone, string);
	return clone;
}
