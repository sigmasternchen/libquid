#ifndef STRINGS_H
#define STRINGS_H

#include "streams.h"

char* strclone(const char*);

stream_t charStream(const char*);

#define stringEqualsPredicate(s) qPredicate(obj, strcmp((char*) obj, s) == 0)
#define stringNotEqualsPredicate(s) qPredicate(obj, strcmp((char*) obj, s) != 0)
#define stringEmptyPredicate(s) qPredicate(obj, strcmp((char*) obj, "") == 0)
#define stringNotEmptyPredicate(s) qPredicate(obj, strcmp((char*) obj, "") != 0)

#endif
