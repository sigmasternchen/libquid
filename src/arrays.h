#ifndef ARRAYS_H
#define ARRAYS_H

#include "streams.h"

typedef struct arrayWithLength {
	void* array;
	size_t length;
} arrayWithLength_t;

stream_t arrayStream(void*, size_t, size_t);

stream_t ptrStream(raw_t*, size_t);

stream_t primitiveStream(void*, size_t, size_t);

#endif
