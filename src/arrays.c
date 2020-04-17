#include "arrays.h"
#include "streams.h"

typedef struct {
	stream_t stream;

	raw_t* array;
	size_t length;
	size_t elementSize;
} arrayStream_t;

static inline arrayStream_t* getCurrentArrayStream() {
	return (arrayStream_t*) getCurrentStream();
}

void ptrArrayStreamIterate(predicate_t predicate) {
	arrayStream_t* stream = getCurrentArrayStream();

	for (size_t i = 0; i < stream->length; i++) {
		if (predicate(stream->array[i])) {
			break;
		}
	}
}

void primitiveArrayStreamIterate(predicate_t predicate) {
	arrayStream_t* stream = getCurrentArrayStream();

	for (size_t i = 0; i < stream->length; i++) {
		raw_t data = 0;
		for (size_t j = 0; j < stream->elementSize; j++) {
			data |= ((unsigned char*) stream->array)[i * stream->elementSize + j] << (j * 8);
		}
	
		if (predicate(data)) {
			break;
		}
	}
}

stream_t arrayStream(void* array, size_t size, size_t elementSize) {
	arrayStream_t* stream = malloc(sizeof(arrayStream_t));
	if (stream == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	skeletonStream(stream);
	
	
	stream->array = array;
	stream->length = size;
	stream->elementSize = elementSize;
	
	stream->stream.__iterate = (elementSize == sizeof(void*)) ? &ptrArrayStreamIterate : &primitiveArrayStreamIterate;
	
	return *((stream_t*) stream);
}

stream_t ptrStream(raw_t* array, size_t length) {
	return arrayStream(array, length, sizeof(void*));
}

stream_t primitiveStream(void* array, size_t length, size_t primitiveSize) {
	return arrayStream(array, length, primitiveSize);
}
