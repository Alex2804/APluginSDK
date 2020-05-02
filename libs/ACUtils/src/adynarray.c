#ifndef ACUTILS_ONE_SOURCE
#   include "../include/ACUtils/adynarray.h"
#endif

#ifdef ACUTILS_ADYNARRAY_H /* if compiled as one source and not included from header, the definitions are excluded */

#include <stdlib.h>
#include <string.h>
#include <math.h>

ACUTILS_ST_FUNC size_t private_ACUtils_ADynArray_calculateCapacityGeneric(size_t requiredSize, size_t minCapacity, size_t maxCapacity, double multiplier) {
    double multiplierExponent;
    size_t capacity;
    if(requiredSize <= minCapacity)
        return minCapacity;
    multiplierExponent = ceil(log((double) requiredSize / minCapacity) / log(multiplier));
    capacity = (size_t) (minCapacity * pow(multiplier, multiplierExponent));
    if(capacity > maxCapacity)
        return maxCapacity;
    return capacity;
}
static size_t private_ACUtils_DynArray_calculateCapacityDefault(size_t requiredSize) {
    return private_ACUtils_ADynArray_calculateCapacityGeneric(requiredSize, 8, 8388608, 2); /* maxCapacity = minCapacity * pow(multiplier, 20) */
}


A_DYNAMIC_ARRAY_DEFINITION(private_ACUtils_DynArray_Prototype, char);

ACUTILS_ST_FUNC void* private_ACUtils_ADynArray_construct(size_t typeSize)
{
    return private_ACUtils_ADynArray_constructWithAllocator(typeSize, realloc, free);
}
ACUTILS_ST_FUNC void* private_ACUtils_ADynArray_constructWithAllocator(size_t typeSize, ACUtilsReallocator reallocator,
                                                                       ACUtilsDeallocator deallocator)
{
    struct private_ACUtils_DynArray_Prototype* prototype;
    if(reallocator == NULL || deallocator == NULL)
        return NULL;
    prototype = (struct private_ACUtils_DynArray_Prototype*) reallocator(NULL, sizeof(struct private_ACUtils_DynArray_Prototype));
    if(prototype != NULL) {
        struct private_ACUtils_DynArray_Prototype tmpPrototype = {.reallocator = reallocator,
                .deallocator = deallocator, .calculateCapacity = private_ACUtils_DynArray_calculateCapacityDefault,
                .size = 0};
        memcpy(prototype, &tmpPrototype, sizeof(struct private_ACUtils_DynArray_Prototype));
        prototype->capacity = prototype->calculateCapacity(prototype->size);
        prototype->buffer = (char*) prototype->reallocator(NULL, prototype->capacity * typeSize);
        if(prototype->buffer == NULL) {
            private_ACUtils_ADynArray_destruct(prototype);
            return NULL;
        }
    }
    return prototype;
}

ACUTILS_ST_FUNC void private_ACUtils_ADynArray_destruct(void *dynArray)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        prototype->deallocator(prototype->buffer);
        prototype->deallocator(prototype);
    }
}

ACUTILS_ST_FUNC size_t private_ACUtils_ADynArray_size(void *dynArray)
{
    return (dynArray == NULL) ? 0 : ((struct private_ACUtils_DynArray_Prototype*) dynArray)->size;
}

ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_reserve(void *dynArray, size_t reserveSize, size_t typeSize)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(prototype->capacity < reserveSize || prototype->buffer == NULL) {
            if(prototype->calculateCapacity != NULL) {
                size_t aimedCapacity = prototype->calculateCapacity(reserveSize);
                if(aimedCapacity >= reserveSize) {
                    char* tmpBuffer = (char*) prototype->reallocator(prototype->buffer, aimedCapacity * typeSize);
                    if(tmpBuffer != NULL) {
                        prototype->capacity = aimedCapacity;
                        prototype->buffer = tmpBuffer;
                        return true;
                    }
                }
            }
        } else {
            return true;
        }
    }
    return false;
}

ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_shrinkToFit(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(prototype->calculateCapacity != NULL) {
            if(prototype->capacity > prototype->calculateCapacity(prototype->size)) {
                size_t capacityBackup = prototype->capacity;
                prototype->capacity = (prototype->size == 0) ? 0 : prototype->size - 1;
                if(!private_ACUtils_ADynArray_reserve(dynArray, prototype->capacity + 1, typeSize)) {
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

ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_clear(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        ((struct private_ACUtils_DynArray_Prototype*) dynArray)->size = 0;
        return private_ACUtils_ADynArray_shrinkToFit(dynArray, typeSize);
    }
    return false;
}

static bool private_ACUtils_DynArray_shiftElements(void *dynArray, size_t index, size_t count, size_t typeSize)
{
    size_t dynArraySize = private_ACUtils_ADynArray_size(dynArray);
    if(index >= dynArraySize) {
        return true;
    } else if(private_ACUtils_ADynArray_reserve(dynArray, dynArraySize + count, typeSize)) {
        char *buffer = ((struct private_ACUtils_DynArray_Prototype*) dynArray)->buffer + (index * typeSize);
        memmove(buffer + (count * typeSize), buffer, (dynArraySize - index) * typeSize);
        return true;
    }
    return false;
}
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_prepareInsertion(void* dynArray, size_t index, size_t valueCount, size_t typeSize)
{
    if(dynArray != NULL && valueCount > 0) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(index >= prototype->size) {
            if(!private_ACUtils_ADynArray_reserve(dynArray, prototype->size + valueCount, typeSize))
                return false;
        } else if(!private_ACUtils_DynArray_shiftElements(dynArray, index, valueCount, typeSize)) {
            return false;
        }
        prototype->size += valueCount;
        return true;
    }
    return valueCount == 0;
}
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_insertArray(void *dynArray, size_t index, const void *array, size_t arraySize, size_t typeSize)
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

ACUTILS_ST_FUNC void private_ACUtils_ADynArray_remove(void *dynArray, size_t index, size_t count, size_t typeSize)
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

#endif /* ACUTILS_ADYNARRAY_H */
