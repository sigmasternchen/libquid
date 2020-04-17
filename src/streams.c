#include "streams.h"

#include "arrays.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_STREAMS (1<<5)
stream_t* streams[MAX_STREAMS];
int currentStream = -1;

stream_t* getCurrentStream() {
	return streams[currentStream];
}

// true for break; false for continue
bool streamApply(stream_t* stream, raw_t data, size_t elementIndex, size_t operationIndex, consumer_t finally) {
	if (operationIndex >= stream->_operationsLength) {
		finally(data);
		return false;
	} else {
		operation_t operation = stream->_operations[operationIndex];
		switch(operation.type) {
			case map:
				data = ((function_t) operation.action)(data);
				break;
			case peek:
				((consumer_t) operation.action)(data);
				break;
			case filter:
				if (!((predicate_t) operation.action)(data)) {
					return false; // abort for this piece of data
				}
				break;
			case limit:
				if (elementIndex >= (size_t) operation.action)
					return true;
				break;
			case skip:
				if (elementIndex < (size_t) operation.action)
					return false;
				break;
		}
		
		stream->_apply(stream, data, elementIndex, operationIndex + 1, finally);
	}
	
	return false;
}

#define OPERATION_CHUNK_SIZE (8)

void streamAddOperation(enum operationType type, void* action) {
	stream_t* stream = getCurrentStream();
	
	if (stream->_operationsLength % OPERATION_CHUNK_SIZE == 0) {
		operation_t* operations = realloc(stream->_operations, sizeof(operation_t) * ((stream->_operationsLength) / OPERATION_CHUNK_SIZE + 1) * OPERATION_CHUNK_SIZE);
		if (operations == NULL) {
			fprintf(stderr, "Fuck\n");
			exit(1);
		}
		stream->_operations = operations;
	}
	
	stream->_operations[stream->_operationsLength++] = (operation_t) {
		.action = action,
		.type = type
	};
}

stream_t streamMap(function_t function) {
	stream_t* stream = getCurrentStream();
	
	stream->_addOperation(map, function);
	
	return *stream;
}

stream_t streamPeek(consumer_t consumer) {
	stream_t* stream = getCurrentStream();
	
	stream->_addOperation(peek, consumer);

	return *stream;
}

stream_t streamFilter(predicate_t predicate) {
	stream_t* stream = getCurrentStream();
	
	stream->_addOperation(filter, predicate);
	
	return *stream;
}

stream_t streamLimit(size_t maxSize) {
	stream_t* stream = getCurrentStream();
	
	stream->_addOperation(limit, (void*) maxSize);
	
	return *stream;
}

stream_t streamSkip(size_t n) {
	stream_t* stream = getCurrentStream();
	
	stream->_addOperation(skip, (void*) n);
	
	return *stream;
}

bool streamAllMatch(predicate_t predicate) {
	stream_t* stream = getCurrentStream();
	
	bool allMatch = true;
	
	size_t i = 0; 
	stream->__iterate(lambda(bool, (raw_t data) {
		bool abort = stream->_apply(stream, data, i++, 0, lambda(void, (raw_t data) {
			if (!predicate(data)) {
				allMatch = false;
			}
		}));
		if (!allMatch)
			abort = true;
		return abort;
	}));
	
	stream->__destroy();
	
	return allMatch;
}

bool streamAnyMatch(predicate_t predicate) {
	stream_t* stream = getCurrentStream();
	
	bool anyMatch = false;
	
	size_t i = 0; 
	stream->__iterate(lambda(bool, (raw_t data) {
		bool abort = stream->_apply(stream, data, i++, 0, lambda(void, (raw_t data) {
			if (predicate(data)) {
				anyMatch = true;
			}
		}));
		if (anyMatch)
			abort = true;
		return abort;
	}));
	
	stream->__destroy();
	
	return anyMatch;
}

void streamForEach(consumer_t consumer) {
	stream_t* stream = getCurrentStream();
	
	size_t i = 0; 
	stream->__iterate(lambda(bool, (raw_t data) {
		return stream->_apply(stream, data, i++, 0, consumer);
	}));
	
	stream->__destroy();
}

size_t streamCount() {
	stream_t* stream = getCurrentStream();
	
	size_t i = 0; 
	stream->__iterate(lambda(bool, (raw_t data) {
		return stream->_apply(stream, data, i++, 0, Consumer(obj, {}));
	}));
	
	stream->__destroy();
	
	return i;
}

optional_t streamMin(comparator_t comparator) {
	stream_t* stream = getCurrentStream();
	
	raw_t current;
	bool present = false;
	stream->__iterate(lambda(bool, (raw_t data) {
		if (!present) {
			current = data;
			present = true;
		} else if (comparator(data, current) < 0) {
			current = data;
		}
		return false;
	}));
	
	stream->__destroy();
	
	return (optional_t) {
		.value = current,
		.present = present,
	};
}

optional_t streamMax(comparator_t comparator) {
	stream_t* stream = getCurrentStream();
	
	raw_t current;
	bool present = false;
	stream->__iterate(lambda(bool, (raw_t data) {
		if (!present) {
			current = data;
			present = true;
		} else if (comparator(data, current) > 0) {
			current = data;
		}
		return false;
	}));
	
	stream->__destroy();
	
	return (optional_t) {
		.value = current,
		.present = present,
	};
}

raw_t streamCollect(collector_t collector) {
	stream_t* stream = getCurrentStream();
	
	raw_t result = collector.supplier();
	
	size_t i = 0;
	stream->__iterate(lambda(bool, (raw_t data) {
		return stream->_apply(stream, data, i++, 0, lambda(void, (raw_t data) {
			collector.accumulator(result, data);
		}));
	}));
	
	result = collector.finisher(result);
	
	stream->__destroy();
	
	return result;
}

void streamDestroy() {
	stream_t* stream = getCurrentStream();
	
	if (stream->_operations != NULL) {
		free(stream->_operations);
	}
	
	free(stream);
	currentStream--;
}

void skeletonStream(void* _stream) {
	stream_t* stream = (stream_t*) _stream;

	stream->_operations = NULL;
	stream->_operationsLength = 0;
		
	stream->_addOperation = &streamAddOperation;
	stream->_destroy = &streamDestroy;
		
	stream->_apply = &streamApply;
	
	stream->__destroy = &streamDestroy;
		
	stream->map = &streamMap;
	stream->peek = &streamPeek;
	stream->filter = &streamFilter;
	stream->limit = &streamLimit;
	stream->skip = &streamSkip;
		
	stream->allMatch = &streamAllMatch;
	stream->anyMatch = &streamAnyMatch;
	stream->forEach = &streamForEach;
	stream->count = &streamCount;
	stream->min = &streamMin;
	stream->max = &streamMax;
	stream->collect = &streamCollect;
	
	streams[++currentStream] = stream;
}

typedef struct {
	stream_t stream;
	raw_t seed;
	function_t function;
} baseIterateStream_t;

typedef struct {
	stream_t stream;
	supplier_t supplier;
} baseGenerateStream_t;

typedef struct {
	stream_t stream;
	stream_t source1;
	stream_t source2;
} baseConcatStream_t;

void iterateStream_iterate(predicate_t predicate) {
	baseIterateStream_t* stream = (baseIterateStream_t*) getCurrentStream();
	
	raw_t current = stream->seed;
	
	while(true) {
		if (predicate(current)) {
			break;
		}
		current = stream->function(current);	
	}
}

stream_t stream_iterate(raw_t seed, function_t function) {
	baseIterateStream_t* stream = malloc(sizeof(baseIterateStream_t));
	if (stream == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	skeletonStream(stream);
	
	stream->seed = seed;
	stream->function = function;
	
	stream->stream.__iterate = &iterateStream_iterate;
	
	return *((stream_t*) stream);
}

void generateStream_iterate(predicate_t predicate) {
	baseGenerateStream_t* stream = (baseGenerateStream_t*) getCurrentStream();
	
	while(true) {
		if (predicate(stream->supplier())) {
			break;
		}
	}
}

stream_t stream_generate(supplier_t supplier) {
	baseGenerateStream_t* stream = malloc(sizeof(baseGenerateStream_t));
	if (stream == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	skeletonStream(stream);
	
	stream->supplier = supplier;
	
	stream->stream.__iterate = &generateStream_iterate;
	
	return *((stream_t*) stream);
}

void emptyStream_iterate(predicate_t predicate) {
	// do nothing
}

stream_t stream_empty(supplier_t supplier) {
	stream_t* stream = malloc(sizeof(stream_t));
	if (stream == NULL) {
		fprintf(stderr, "Fuck\n");
		exit(1);
	}
	skeletonStream(stream);
	
	stream->__iterate = &emptyStream_iterate;
	
	return *stream;
}

const struct globalStream Stream = {
	.iterate = &stream_iterate,
	.generate = &stream_generate,
	.empty = &stream_empty,
	//.concat = &stream_concat,
};
