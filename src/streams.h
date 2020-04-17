#ifndef STREAMS_H
#define STREAMS_H

#include "functional.h"

#include <stdlib.h>
#include <stdio.h>

#define ARRAY_COLLECTOR_CHUNK_SIZE (8)

#define ofType(t) (sizeof(t))

#define arrayCollector(s) ((collector_t) {\
	.supplier = Supplier({\
		arrayWithLength_t* internal = malloc(sizeof(struct arrayWithLength));\
		if (internal == NULL) {\
			fprintf(stderr, "Fuck\n");\
			exit(1);\
		}\
		internal->array = NULL;\
		internal->length = 0;\
		return (raw_t) internal;\
	}),\
	.accumulator = BiConsumer(_internal, data, {\
		arrayWithLength_t* internal = (void*) _internal;\
		if (internal->length % ARRAY_COLLECTOR_CHUNK_SIZE == 0) {\
			void* array = realloc(internal->array, s * ((internal->length / ARRAY_COLLECTOR_CHUNK_SIZE) + 1) * ARRAY_COLLECTOR_CHUNK_SIZE);\
			if (array == NULL) {\
				fprintf(stderr, "Fuck\n");\
				exit(1);\
			}\
			internal->array = array;\
		}\
		for (size_t i = 0; i < s; i++) {\
			((char*) (internal->array))[internal->length * s + i] = (data >> (i * 8)) & 0xff;\
		}\
		internal->length++;\
	}),\
	.finisher = qFunction(_internal, _internal)\
})

#define stringCollector() ({\
	collector_t collector = arrayCollector(ofType(char));\
	collector.finisher = Function(_internal, {\
		arrayWithLength_t* internal = (void*) _internal;\
		if (internal->length == 0 || ((char*) (internal->array))[internal->length - 1] != '\0') {\
			collector.accumulator(_internal, '\0');\
		}\
		raw_t result = (raw_t) internal->array;\
		free(internal);\
		return result;\
	});\
	collector;\
})

#define sumCollector() ((collector_t) {\
	.supplier = qSupplier(0),\
	.accumulator = qBiConsumer(sum, element, sum + element),\
	.finisher = qFunction(sum, sum)\
})

enum operationType {
	peek, map, filter, limit, skip
};


typedef struct operation {
	enum operationType type;
	void* action;
} operation_t;

typedef struct stream {
	operation_t* _operations;
	size_t _operationsLength;
	
	void (*_addOperation)(enum operationType, void*);
	bool (*_apply)(struct stream*, raw_t, size_t, size_t, consumer_t);
	void (*_destroy)();
	
	void (*__destroy)();
	void (*__iterate)(predicate_t);
	
	struct stream (*map)(function_t);
	struct stream (*peek)(consumer_t);
	struct stream (*filter)(predicate_t);
	struct stream (*limit)(size_t);
	struct stream (*skip)(size_t);
	optional_t (*min)(comparator_t);
	optional_t (*max)(comparator_t);
	
	bool (*allMatch)(predicate_t);
	bool (*anyMatch)(predicate_t);
	void (*forEach)(consumer_t);
	size_t (*count)();
	raw_t (*collect)(collector_t);
} stream_t;

const extern struct globalStream {
	stream_t (*iterate)(raw_t, function_t);
	stream_t (*generate)(supplier_t);
	stream_t (*empty)();
} Stream;

stream_t* getCurrentStream();

void skeletonStream(void*);

#endif
