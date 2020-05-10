#ifndef ACUTILS_ASTRING_H
#define ACUTILS_ASTRING_H

#include "macros.h"
#include "types.h"

struct AString;

ACUTILS_HD_FUNC struct AString* AString_construct(void);
ACUTILS_HD_FUNC struct AString* AString_constructWithAllocator(ACUtilsReallocator reallocator, ACUtilsDeallocator deallocator);
ACUTILS_HD_FUNC void AString_destruct(struct AString *str);

ACUTILS_HD_FUNC ACUtilsReallocator AString_reallocator(const struct AString *str);
ACUTILS_HD_FUNC ACUtilsDeallocator AString_deallocator(const struct AString *str);

ACUTILS_HD_FUNC const char* AString_buffer(const struct AString *str);
ACUTILS_HD_FUNC size_t AString_capacity(const struct AString *str);
ACUTILS_HD_FUNC size_t AString_size(const struct AString *str);

ACUTILS_HD_FUNC bool AString_reserve(struct AString *str, size_t reserveSize);
ACUTILS_HD_FUNC bool AString_shrinkToFit(struct AString *str);

ACUTILS_HD_FUNC void AString_clear(struct AString *str);
ACUTILS_HD_FUNC void AString_remove(struct AString *str, size_t index, size_t count);

ACUTILS_HD_FUNC bool AString_insert(struct AString *str, size_t index, char c);
ACUTILS_HD_FUNC bool AString_insertCString(struct AString *str, size_t index, const char *cstr, size_t len);
ACUTILS_HD_FUNC bool AString_insertAString(struct AString *destStr, size_t index, const struct AString *srcStr);
ACUTILS_HD_FUNC bool AString_append(struct AString *str, char c);
ACUTILS_HD_FUNC bool AString_appendCString(struct AString *str, const char *cstr, size_t len);
ACUTILS_HD_FUNC bool AString_appendAString(struct AString *destStr, const struct AString *srcStr);

ACUTILS_HD_FUNC char AString_get(const struct AString *str, size_t index);
ACUTILS_HD_FUNC bool AString_set(struct AString *str, size_t index, char c);
ACUTILS_HD_FUNC bool AString_setRange(struct AString *str, size_t index, size_t count, char c);

#ifdef ACUTILS_ONE_SOURCE
#   include "../../src/astring.c"
#endif

#endif /* ACUTILS_ASTRING_H */
