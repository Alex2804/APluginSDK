#ifndef ACUTILS_ONE_SOURCE
#   include "../include/ACUtils/adynarray.h"
#else
#   include "../include/ACUtils/macros.h"
#   include "../include/ACUtils/types.h"
    ACUTILS_HD_FUNC void* private_ACUtils_ADynArray_constructWithAllocator(size_t, ACUtilsReallocator, ACUtilsDeallocator);
    ACUTILS_HD_FUNC void private_ACUtils_ADynArray_destruct(void*);
#endif

#ifdef ACUTILS_ADYNARRAY_H /* if compiled as one source and not included from header, the definitions are excluded */

#include <stdlib.h>
#include <string.h>
#include <math.h>

ACUTILS_HD_FUNC size_t private_ACUtils_ADynArray_growStrategyGeneric(size_t requiredSize, size_t minCapacity, size_t maxCapacity, double multiplier, size_t maxDifSize) {
    double multiplierExponent;
    size_t capacity;
    if(requiredSize <= minCapacity)
        return minCapacity;
    multiplierExponent = ceil(log((double) requiredSize / minCapacity) / log(multiplier));
    capacity = (size_t) (minCapacity * pow(multiplier, multiplierExponent));
    if(capacity - requiredSize > maxDifSize)
        capacity = requiredSize + maxDifSize;
    if(capacity > maxCapacity)
        capacity = maxCapacity;
    return capacity;
}
static size_t private_ACUtils_ADynArray_growStrategyDefault(size_t requiredSize, size_t typeSize) {
    (void) typeSize; /* suppress unused warning */
    return private_ACUtils_ADynArray_growStrategyGeneric(requiredSize, 8, -1, 2, 1000000);
}


A_DYNAMIC_ARRAY_DEFINITION(private_ACUtils_DynArray_Prototype, char);

ACUTILS_HD_FUNC void* private_ACUtils_ADynArray_construct(size_t typeSize)
{
    return private_ACUtils_ADynArray_constructWithAllocator(typeSize, realloc, free);
}
ACUTILS_HD_FUNC void* private_ACUtils_ADynArray_constructWithAllocator(size_t typeSize, ACUtilsReallocator reallocator,
                                                                       ACUtilsDeallocator deallocator)
{
    struct private_ACUtils_DynArray_Prototype* prototype;
    if(reallocator == NULL || deallocator == NULL)
        return NULL;
    prototype = (struct private_ACUtils_DynArray_Prototype*) reallocator(NULL, sizeof(struct private_ACUtils_DynArray_Prototype));
    if(prototype != NULL) {
        struct private_ACUtils_DynArray_Prototype tmpPrototype = {reallocator, deallocator};
        memcpy(prototype, &tmpPrototype, sizeof(struct private_ACUtils_DynArray_Prototype));
        prototype->growStrategy = private_ACUtils_ADynArray_growStrategyDefault;
        prototype->size = 0;
        prototype->capacity = prototype->growStrategy(prototype->size, typeSize);
        prototype->buffer = (char*) prototype->reallocator(NULL, prototype->capacity * typeSize);
        if(prototype->buffer == NULL) {
            private_ACUtils_ADynArray_destruct(prototype);
            return NULL;
        }
    }
    return prototype;
}
ACUTILS_HD_FUNC void private_ACUtils_ADynArray_destruct(void *dynArray)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        prototype->deallocator(prototype->buffer);
        prototype->deallocator(prototype);
    }
}

ACUTILS_HD_FUNC void private_ACUtils_ADynArray_setGrowStrategy(void* dynArray, ACUtilsGrowStrategy growStrategy)
{
    if(dynArray != NULL)
        ((struct private_ACUtils_DynArray_Prototype*) dynArray)->growStrategy = growStrategy;
}

ACUTILS_HD_FUNC size_t private_ACUtils_ADynArray_size(const void *dynArray)
{
    return (dynArray == NULL) ? 0 : ((struct private_ACUtils_DynArray_Prototype*) dynArray)->size;
}
ACUTILS_HD_FUNC size_t private_ACUtils_ADynArray_capacity(const void *dynArray)
{
    return (dynArray == NULL) ? 0 : ((struct private_ACUtils_DynArray_Prototype*) dynArray)->capacity;
}

ACUTILS_HD_FUNC bool private_ACUtils_ADynArray_reserve(void *dynArray, size_t reserveSize, bool forceExactSize, size_t typeSize)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(prototype->capacity < reserveSize || prototype->buffer == NULL) {
            size_t aimedCapacity = reserveSize;
            if(prototype->growStrategy != NULL && (!forceExactSize || reserveSize < prototype->growStrategy(0, typeSize)))
                aimedCapacity = prototype->growStrategy(reserveSize, typeSize);
            if(aimedCapacity >= reserveSize) {
                char* tmpBuffer = (char*) prototype->reallocator(prototype->buffer, aimedCapacity * typeSize);
                if(tmpBuffer != NULL) {
                    prototype->capacity = aimedCapacity;
                    prototype->buffer = tmpBuffer;
                    return true;
                }
            }
        } else {
            return true;
        }
    }
    return false;
}
ACUTILS_HD_FUNC bool private_ACUtils_ADynArray_shrinkToFit(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(prototype->growStrategy != NULL) {
            if(prototype->capacity > prototype->growStrategy(prototype->size, typeSize)) {
                size_t capacityBackup = prototype->capacity;
                prototype->capacity = (prototype->size == 0) ? 0 : prototype->size - 1;
                if(!private_ACUtils_ADynArray_reserve(dynArray, prototype->capacity + 1, true, typeSize)) {
                    prototype->capacity = capacityBackup;
                } else {
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    return false;
}

ACUTILS_HD_FUNC void private_ACUtils_ADynArray_clear(void *dynArray)
{
    if(dynArray != NULL)
        ((struct private_ACUtils_DynArray_Prototype*) dynArray)->size = 0;
}
ACUTILS_HD_FUNC void private_ACUtils_ADynArray_remove(void *dynArray, size_t index, size_t count, size_t typeSize)
{
    struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
    if(dynArray != NULL && count > 0 && index >= 0 && index < prototype->size) {
        if(index + count <= prototype->size) {
            memmove(prototype->buffer + (index * typeSize), prototype->buffer + ((index + count) * typeSize), (prototype->size - index - count) * typeSize);
            prototype->size -= count;
        } else {
            prototype->size = index;
        }
    }
}

static bool private_ACUtils_DynArray_shiftElements(void *dynArray, size_t index, size_t count, size_t typeSize)
{
    size_t dynArraySize = private_ACUtils_ADynArray_size(dynArray);
    if(index >= dynArraySize) {
        return true;
    } else if(private_ACUtils_ADynArray_reserve(dynArray, dynArraySize + count, false, typeSize)) {
        char *buffer = ((struct private_ACUtils_DynArray_Prototype*) dynArray)->buffer + (index * typeSize);
        memmove(buffer + (count * typeSize), buffer, (dynArraySize - index) * typeSize);
        return true;
    }
    return false;
}
ACUTILS_HD_FUNC bool private_ACUtils_ADynArray_prepareInsertion(void* dynArray, size_t index, size_t valueCount, size_t typeSize)
{
    if(dynArray != NULL && valueCount > 0) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(index >= prototype->size) {
            if(!private_ACUtils_ADynArray_reserve(dynArray, prototype->size + valueCount, false, typeSize))
                return false;
        } else if(!private_ACUtils_DynArray_shiftElements(dynArray, index, valueCount, typeSize)) {
            return false;
        }
        prototype->size += valueCount;
        return true;
    }
    return valueCount == 0;
}
ACUTILS_HD_FUNC bool private_ACUtils_ADynArray_insertArray(void *dynArray, size_t index, const void *array, size_t arraySize, size_t typeSize)
{
    if(array == NULL)
        arraySize = 0;
    if(private_ACUtils_ADynArray_prepareInsertion(dynArray, index, arraySize, typeSize)) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        memcpy(prototype->buffer + ((index >= prototype->size ? prototype->size - arraySize : index) * typeSize), array, arraySize * typeSize);
        return true;
    }
    return false;
}

ACUTILS_HD_FUNC bool private_ACUtils_ADynArray_setRange(void *dynArray, size_t index, size_t count, void *value, size_t typeSize)
{
    size_t appendCount = 0, i;
    struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
    if(dynArray == NULL)
        return false;
    if(index > prototype->size)
        index = prototype->size;
    if(index + count >= prototype->size)
        appendCount = index + count - prototype->size;
    if(!private_ACUtils_ADynArray_reserve(dynArray, prototype->size + appendCount, false, typeSize))
        return false;
    prototype->size += appendCount;
    for(i = 0; i < count; ++i)
        memcpy(prototype->buffer + ((index + i) * typeSize), value, typeSize);
    return true;
}

#endif /* ACUTILS_ADYNARRAY_H */
