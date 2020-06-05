#ifndef ACUTILS_ASTRING_H
#define ACUTILS_ASTRING_H

#include "macros.h"
#include "types.h"
#include "adynarray.h"

ACUTILS_OPEN_EXTERN_C

struct AString;
A_DYNAMIC_ARRAY_DEFINITION(ASplittedString, struct AString*);

ACUTILS_HD_FUNC struct AString* AString_construct(void);
ACUTILS_HD_FUNC struct AString* AString_constructWithAllocator(ACUtilsReallocator reallocator, ACUtilsDeallocator deallocator);
ACUTILS_HD_FUNC struct AString* AString_constructWithCapacityAndAllocator(size_t capacity, ACUtilsReallocator reallocator, ACUtilsDeallocator deallocator);
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
ACUTILS_HD_FUNC void AString_trim(struct AString *str, char c);
ACUTILS_HD_FUNC void AString_trimFront(struct AString *str, char c);
ACUTILS_HD_FUNC void AString_trimBack(struct AString *str, char c);

ACUTILS_HD_FUNC bool AString_insert(struct AString *str, size_t index, char c);
ACUTILS_HD_FUNC bool AString_insertCString(struct AString *str, size_t index, const char *cstr, size_t len);
ACUTILS_HD_FUNC bool AString_insertAString(struct AString *destStr, size_t index, const struct AString *srcStr);
ACUTILS_HD_FUNC bool AString_append(struct AString *str, char c);
ACUTILS_HD_FUNC bool AString_appendCString(struct AString *str, const char *cstr, size_t len);
ACUTILS_HD_FUNC bool AString_appendAString(struct AString *destStr, const struct AString *srcStr);

ACUTILS_HD_FUNC char AString_get(const struct AString *str, size_t index);
ACUTILS_HD_FUNC bool AString_set(struct AString *str, size_t index, char c);
ACUTILS_HD_FUNC bool AString_setRange(struct AString *str, size_t index, size_t count, char c);

ACUTILS_HD_FUNC bool AString_replaceRange(struct AString *str, size_t index, size_t count, char c, size_t len);
ACUTILS_HD_FUNC bool AString_replaceRangeCString(struct AString *str, size_t index, size_t count, const char *cstr, size_t len);
ACUTILS_HD_FUNC bool AString_replaceRangeAString(struct AString *str, size_t index, size_t count, const struct AString *rep);

ACUTILS_HD_FUNC void AString_replace(struct AString *str, char old, char rep, size_t count);
ACUTILS_HD_FUNC bool AString_replaceCString(struct AString *str, const char *old, size_t oldLen, const char *rep, size_t newLen, size_t count);
ACUTILS_HD_FUNC bool AString_replaceAString(struct AString *str, const struct AString *old, const struct AString *rep, size_t count);

ACUTILS_HD_FUNC bool AString_equals(const struct AString *str1, const struct AString *str2);
ACUTILS_HD_FUNC bool AString_equalsCString(const struct AString *str, const char *cstr);
ACUTILS_HD_FUNC int AString_compare(const struct AString *str1, const struct AString *str2);
ACUTILS_HD_FUNC int AString_compareCString(const struct AString *str, const char *cstr);

ACUTILS_HD_FUNC struct AString* AString_clone(const struct AString *str);
ACUTILS_HD_FUNC struct AString* AString_substring(const struct AString *str, size_t index, size_t count);

ACUTILS_HD_FUNC struct ASplittedString* AString_split(const struct AString *str, char c, bool discardEmpty);
ACUTILS_HD_FUNC void AString_freeSplitted(struct ASplittedString *splitted);

#ifdef ACUTILS_ONE_SOURCE
#   include "../../src/astring.c"
#endif

ACUTILS_CLOSE_EXTERN_C

#endif /* ACUTILS_ASTRING_H */
