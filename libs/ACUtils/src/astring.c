#ifndef ACUTILS_ONE_SOURCE
#   include "../include/ACUtils/astring.h"
#endif

#ifdef ACUTILS_ASTRING_H /* if compiled as one source and not included from header, the definitions are excluded */

#include "stdlib.h"
#include "string.h"
#include "math.h"

struct AString
{
    const ACUtilsReallocator reallocator;
    const ACUtilsDeallocator deallocator;
    size_t size;
    size_t capacity;
    char *buffer;
};

static const size_t private_ACUtils_AString_capacityMin = 8;
static const size_t private_ACUtils_AString_capacityMul = 2;
static const size_t private_ACUtils_AString_capacityAllocMax = 1024;

ACUTILS_HD_FUNC struct AString* AString_construct(void)
{
    return AString_constructWithAllocator(realloc, free);
}
ACUTILS_HD_FUNC struct AString* AString_constructWithAllocator(ACUtilsReallocator reallocator, ACUtilsDeallocator deallocator)
{
    return AString_constructWithCapacityAndAllocator(private_ACUtils_AString_capacityMin, reallocator, deallocator);
}
ACUTILS_HD_FUNC struct AString* AString_constructWithCapacityAndAllocator(size_t capacity, ACUtilsReallocator reallocator, ACUtilsDeallocator deallocator)
{
    struct AString *string;
    if(reallocator == nullptr || deallocator == nullptr)
        return nullptr;
    if(capacity < private_ACUtils_AString_capacityMin)
        capacity = private_ACUtils_AString_capacityMin;
    string = (struct AString*) reallocator(nullptr, sizeof(struct AString));
    if(string != nullptr) {
        struct AString tmpString = { reallocator, deallocator};
        memcpy(string, &tmpString, sizeof(struct AString));
        string->size = 0;
        string->capacity = capacity;
        string->buffer = (char*) string->reallocator(nullptr, (string->capacity + 1) * sizeof(char));
        if(string->buffer == nullptr) {
            AString_destruct(string);
            return nullptr;
        }
        string->buffer[0] = '\0';
    }
    return string;
}
ACUTILS_HD_FUNC void AString_destruct(struct AString *str)
{
    if(str != nullptr) {
        str->deallocator(str->buffer);
        str->deallocator(str);
    }
}

ACUTILS_HD_FUNC ACUtilsReallocator AString_reallocator(const struct AString *str)
{
    return str == nullptr ? nullptr : str->reallocator;
}
ACUTILS_HD_FUNC ACUtilsDeallocator AString_deallocator(const struct AString *str)
{
    return str == nullptr ? nullptr : str->deallocator;
}

ACUTILS_HD_FUNC const char* AString_buffer(const struct AString *str)
{
    return str == nullptr ? nullptr : str->buffer;
}
ACUTILS_HD_FUNC size_t AString_capacity(const struct AString *str)
{
    return str == nullptr ? 0 : str->capacity;
}
ACUTILS_HD_FUNC size_t AString_size(const struct AString *str)
{
    return str == nullptr ? 0 : str->size;
}

ACUTILS_HD_FUNC bool AString_reserve(struct AString *str, size_t reserveSize)
{
    if(str != nullptr) {
        if(reserveSize > str->capacity) {
            size_t aimedCapacity = private_ACUtils_AString_capacityMin;
            if(reserveSize >= private_ACUtils_AString_capacityMin) {
                double multiplierExponent =
                        (ceil(log((double) reserveSize / 2) / log(private_ACUtils_AString_capacityMul)));
                aimedCapacity = (size_t) (2 * pow((double) private_ACUtils_AString_capacityMul, multiplierExponent));
                if(aimedCapacity - reserveSize > private_ACUtils_AString_capacityAllocMax)
                    aimedCapacity = reserveSize + private_ACUtils_AString_capacityAllocMax;
            }
            if(aimedCapacity >= reserveSize) {
                char *tmpBuffer = (char*) str->reallocator(str->buffer, (aimedCapacity + 1) * sizeof(char));
                if(tmpBuffer != nullptr) {
                    str->capacity = aimedCapacity;
                    str->buffer = tmpBuffer;
                    return true;
                }
            }
        } else {
            return true;
        }
    }
    return false;
}
ACUTILS_HD_FUNC bool AString_shrinkToFit(struct AString *str)
{
    if(str != nullptr) {
        size_t newCapacity = (str->size < private_ACUtils_AString_capacityMin) ? private_ACUtils_AString_capacityMin : str->size;
        if(newCapacity < str->capacity) {
            char *tmpBuffer = (char *) str->reallocator(str->buffer, (newCapacity + 1) * sizeof(char));
            if(tmpBuffer != nullptr) {
                str->capacity = newCapacity;
                str->buffer = tmpBuffer;
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}

ACUTILS_HD_FUNC void AString_clear(struct AString *str)
{
    if(str != nullptr) {
        str->size = 0;
        str->buffer[0] = '\0';
    }
}
ACUTILS_HD_FUNC void AString_remove(struct AString *str, size_t index, size_t count)
{
    if(str != nullptr && index < str->size) {
        if(count >= ((size_t) 0) - index - 1 || index + count >= str->size) {
            str->size = index;
            str->buffer[str->size] = '\0';
        } else {
            str->size -= count;
            memmove(str->buffer + index, str->buffer + index + count, (str->size - index + 1) * sizeof(char)); /* +1 for '\0' */
        }
    }
}
ACUTILS_HD_FUNC void AString_trim(struct AString *str, char c)
{
    AString_trimBack(str, c);
    AString_trimFront(str, c);
}
ACUTILS_HD_FUNC void AString_trimFront(struct AString *str, char c)
{
    size_t trimCount = 0;
    if(str == nullptr)
        return;
    while(str->buffer[trimCount] == c && trimCount < str->size)
        ++trimCount;
    str->size -= trimCount;
    if(str->size > 0)
        memmove(str->buffer, str->buffer + trimCount, str->size);
    str->buffer[str->size] = '\0';
}
ACUTILS_HD_FUNC void AString_trimBack(struct AString *str, char c)
{
    size_t trimmedSize;
    if(str == nullptr || str->size == 0)
        return;
    trimmedSize = str->size;
    while(trimmedSize > 0 && str->buffer[trimmedSize - 1] == c)
        --trimmedSize;
    str->size = trimmedSize;
    str->buffer[str->size] = '\0';
}

ACUTILS_HD_FUNC bool AString_insert(struct AString *str, size_t index, char c)
{
    if(c == '\0') {
        AString_remove(str, index, -1);
        return true;
    }
    return AString_insertCString(str, index, &c, 1);
}
ACUTILS_HD_FUNC bool AString_insertCString(struct AString *str, size_t index, const char *cstr, size_t len)
{
    if(str != nullptr && cstr != nullptr && AString_reserve(str, str->size + len)) {
        char *insertPtr;
        if(index > str->size)
            index = str->size;
        insertPtr = str->buffer + index;
        if(index < str->size)
            memmove(insertPtr + len, insertPtr, str->size - index);
        memcpy(insertPtr, cstr, len);
        str->size += len;
        str->buffer[str->size] = '\0';
        return true;
    }
    return str != nullptr && cstr == nullptr;
}
ACUTILS_HD_FUNC bool AString_insertAString(struct AString *destStr, size_t index, const struct AString *srcStr)
{
    if(srcStr != nullptr)
        return AString_insertCString(destStr, index, srcStr->buffer, srcStr->size);
    return destStr != nullptr;
}
ACUTILS_HD_FUNC bool AString_append(struct AString *str, char c)
{
    return AString_insert(str, -1, c);
}
ACUTILS_HD_FUNC bool AString_appendCString(struct AString *str, const char *cstr, size_t len)
{
    return AString_insertCString(str, -1, cstr, len);
}
ACUTILS_HD_FUNC bool AString_appendAString(struct AString *destStr, const struct AString *srcStr)
{
    return AString_insertAString(destStr, -1, srcStr);
}

ACUTILS_HD_FUNC char AString_get(const struct AString *str, size_t index)
{
    if(str == nullptr || index >= str->size)
        return '\0';
    return str->buffer[index];
}
ACUTILS_HD_FUNC bool AString_set(struct AString *str, size_t index, char c)
{
    if(str == nullptr)
        return false;
    else if(index >= str->size)
        return AString_append(str, c);
    str->buffer[index] = c;
    return true;
}
ACUTILS_HD_FUNC bool AString_setRange(struct AString *str, size_t index, size_t count, char c)
{
    size_t appendCount = 0, i, endIndex;
    if(str == nullptr)
        return false;
    if(index > str->size)
        index = str->size;
    if(count >= ((size_t) 0) - index - 1 || index + count >= str->size)
        appendCount = index + count - str->size;
    if(!AString_reserve(str, str->size + appendCount))
        return false;
    str->size += appendCount;
    endIndex = index + count;
    for(i = index; i < endIndex; ++i)
        str->buffer[i] = c;
    str->buffer[str->size] = '\0';
    return true;
}

ACUTILS_HD_FUNC bool AString_equals(const struct AString *str1, const struct AString *str2)
{
    if(str2 == nullptr || str1 == nullptr)
        return str1 == str2;
    return strcmp(str1->buffer, str2->buffer) == 0;
}
ACUTILS_HD_FUNC bool AString_equalsCString(const struct AString *str, const char *cstr)
{
    if(str == nullptr || cstr == nullptr)
        return str == nullptr && cstr == nullptr;
    return strcmp(str->buffer, cstr) == 0;
}
ACUTILS_HD_FUNC int AString_compare(const struct AString *str1, const struct AString *str2)
{
    if(str2 == nullptr && str1 == nullptr)
        return 0;
    else if(str1 == nullptr)
        return -1;
    else if(str2 == nullptr)
        return 1;
    return strcmp(str1->buffer, str2->buffer);
}
ACUTILS_HD_FUNC int AString_compareCString(const struct AString *str, const char *cstr)
{
    if(str == nullptr && cstr == nullptr)
        return 0;
    else if(str == nullptr)
        return -1;
    else if(cstr == nullptr)
        return 1;
    return strcmp(str->buffer, cstr);
}

ACUTILS_HD_FUNC struct AString* AString_clone(const struct AString *str)
{
    struct AString *cloned;
    if(str == nullptr)
        return nullptr;
    cloned = AString_constructWithCapacityAndAllocator(str->capacity, str->reallocator, str->deallocator);
    if(cloned == nullptr)
        return nullptr;
    memcpy(cloned->buffer, str->buffer, str->size + 1);
    cloned->size = str->size;
    return cloned;
}
ACUTILS_HD_FUNC struct AString* AString_substring(const struct AString *str, size_t index, size_t count)
{
    struct AString *substring;
    if(str == nullptr)
        return nullptr;
    if(index > str->size)
        index = str->size;
    if(count >= ((size_t) 0) - index - 1 || index + count >= str->size)
        count = str->size - index;
    substring = AString_constructWithCapacityAndAllocator(count, str->reallocator, str->deallocator);
    if(substring == nullptr)
        return nullptr;
    memcpy(substring->buffer, str->buffer + index, count);
    substring->buffer[count] = '\0';
    substring->size = count;
    return substring;
}

ACUTILS_HD_FUNC struct ASplittedString* AString_split(const struct AString *str, char c, bool discardEmpty)
{
    size_t i, behindLastDelimiterIndex = 0;
    struct ASplittedString *splitted;
    if(str == nullptr)
        return nullptr;
    splitted = ADynArray_constructWithAllocator(struct ASplittedString, str->reallocator, str->deallocator);
    if(splitted == nullptr)
        return nullptr;
    for(i = 0; i <= str->size; ++i) {
        if(i == str->size || str->buffer[i] == c) {
            if(!discardEmpty || i - behindLastDelimiterIndex > 0) {
                struct AString *substr = AString_substring(str, behindLastDelimiterIndex, i - behindLastDelimiterIndex);
                if(substr == nullptr || !ADynArray_append(splitted, substr)) {
                    AString_freeSplitted(splitted);
                    return nullptr;
                }
            }
            behindLastDelimiterIndex = i + 1;
        }
    }
    return splitted;
}
ACUTILS_HD_FUNC void AString_freeSplitted(struct ASplittedString *splitted)
{
    size_t i, splittedSize = ADynArray_size(splitted);
    for(i = 0; i < splittedSize; ++i)
        AString_destruct(ADynArray_get(splitted, i));
    ADynArray_destruct(splitted);
}

#endif