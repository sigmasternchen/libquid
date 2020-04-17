#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <stdbool.h>
#include <stdlib.h>

#define lambda(r, f) ({r __fn__ f __fn__; })

typedef unsigned long long raw_t;

typedef struct supply {
	bool hasMore;
	raw_t data;
} supply_t;

typedef raw_t (*function_t)(raw_t);
typedef void (*consumer_t)(raw_t);
typedef void (*biconsumer_t)(raw_t, raw_t);
typedef bool (*predicate_t)(raw_t);
typedef raw_t (*supplier_t)(void);
typedef int (*comparator_t)(raw_t, raw_t);
typedef supply_t (*multisupplier_t)(void);

#define Function(i, f) lambda(raw_t, (raw_t i) f)
#define Consumer(i, f) lambda(void, (raw_t i) f)
#define Predicate(i, f) lambda(bool, (raw_t i) f)
#define BiConsumer(i1, i2, f) lambda(void, (raw_t i1, raw_t i2) f)
#define Supplier(f) lambda(raw_t, () f)
#define Comparator(i1, i2, f) lambda(int, (raw_t i1, raw_t i2) f)

#define qFunction(i, f) Function(i, { return f; })
#define qConsumer(i, f) Consumer(i, { f; })
#define qPredicate(i, f) Predicate(i, { return f; })
#define qBiConsumer(i1, i2, f) BiConsumer(i1, i2, { f; })
#define qSupplier(f) Supplier({ return f; })
#define qComparator(i1, i2, f) Comparator(i1, i2, { return f; })

typedef struct collector {
	supplier_t supplier;
	biconsumer_t accumulator;
	function_t finisher;
} collector_t;

enum iteratorResultStepType {
	CONTINUE,
	BREAK
};

enum iteratorResultModifyType {
	KEEP,
	REPLACE,
	DELETE
};

typedef struct iteratorResult {
	enum iteratorResultStepType step;
	enum iteratorResultModifyType modify;
	raw_t data;
} iter_result_t;

#define ITERATOR_CONTINUE ((iter_result_t) { .step = CONTINUE, .modify = KEEP })
#define ITERATOR_BREAK ((iter_result_t) { .step = BREAK, .modify = KEEP })
#define ITERATOR_DELETE ((iter_result_t) { .step = CONTINUE, .modify = DELETE })
#define ITERATOR_REPLACE(d) ((iter_result_t) { .step = CONTINUE, .modify = REPLACE, .data = d })

typedef iter_result_t (*iterator_t)(size_t, raw_t);

#define Iterator(i, d, f) lambda(iter_result_t, (size_t i, raw_t d) f)


typedef struct optional {
	raw_t value;
	bool present;
} optional_t;

#endif
