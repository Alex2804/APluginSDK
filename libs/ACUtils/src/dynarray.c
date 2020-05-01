#if !ACUTILS_ONE_SOURCE
#   include "../include/ACUtils/dynarray.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef PRIVATE_ACUTILS_DYN_ARRAY_IMPLEMENT

#if PRIVATE_ACUTILS_TEST
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_DATA size_t private_ACUtils_DynArray_testMallocCount = 0;
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_DATA bool private_ACUtils_DynArray_testMallocFail = false;
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_DATA size_t private_ACUtils_DynArray_testReallocCount = 0;
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_DATA bool private_ACUtils_DynArray_testReallocFail = false;
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_DATA size_t private_ACUtils_DynArray_testFreeCount = 0;

    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC void* private_ACUtils_DynArray_testMalloc(size_t size) {
        if(!private_ACUtils_DynArray_testMallocFail) {
            void* tmp = malloc(size);
            if(tmp != NULL) {
                ++private_ACUtils_DynArray_testMallocCount;
            }
            return tmp;
        } else {
            return NULL;
        }
    }
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC void* private_ACUtils_DynArray_testRealloc(void* ptr, size_t size) {
        if(!private_ACUtils_DynArray_testReallocFail) {
            void* tmp = realloc(ptr, size);
            if(tmp != NULL) {
                ++private_ACUtils_DynArray_testReallocCount;
            }
            return tmp;
        } else {
            return NULL;
        }
    }
    ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC void private_ACUtils_DynArray_testFree(void* ptr) {
        if(ptr != NULL) {
            ++private_ACUtils_DynArray_testFreeCount;
        }
        free(ptr);
    }

    #define malloc(size) private_ACUtils_DynArray_testMalloc(size)
    #define realloc(ptr, size) private_ACUtils_DynArray_testRealloc(ptr, size)
    #define free(ptr) private_ACUtils_DynArray_testFree(ptr)
#endif

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC size_t _private_ACUtils_DynArray_calculateCapacityGeneric(size_t requiredSize, size_t minCapacity, size_t maxCapacity, double multiplier) {
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
    return _private_ACUtils_DynArray_calculateCapacityGeneric(requiredSize, 8, 8388608, 2); /* maxCapacity = minCapacity * pow(multiplier, 20) */
}


struct private_ACUtils_DynArray_Prototype
{
    char* buffer;
    size_t size;
    size_t capacity;
    size_t(*calculateCapacity)(size_t);
};

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC void* _private_ACUtils_DynArray_construct(size_t typeSize)
{
    struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) malloc(sizeof(struct private_ACUtils_DynArray_Prototype));
    if(prototype != NULL) {
        prototype->size = 0;
        prototype->calculateCapacity = private_ACUtils_DynArray_calculateCapacityDefault;
        prototype->capacity = prototype->calculateCapacity(prototype->size);
        prototype->buffer = (char*) malloc(prototype->capacity * typeSize);
        if(prototype->buffer == NULL) {
            free(prototype);
            return NULL;
        }
    }
    return prototype;
}

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC void _private_ACUtils_DynArray_destruct(void *dynArray)
{
    if(dynArray != NULL) {
        free(((struct private_ACUtils_DynArray_Prototype*) dynArray)->buffer);
        free(dynArray);
    }
}

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC size_t _private_ACUtils_DynArray_size(void *dynArray)
{
    return (dynArray == NULL) ? 0 : ((struct private_ACUtils_DynArray_Prototype*) dynArray)->size;
}

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC bool _private_ACUtils_DynArray_reserve(void *dynArray, size_t reserveSize, size_t typeSize)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(prototype->capacity < reserveSize || prototype->buffer == NULL) {
            if(prototype->calculateCapacity != NULL) {
                size_t aimedCapacity = prototype->calculateCapacity(reserveSize);
                if(aimedCapacity >= reserveSize) {
                    char* tmpBuffer = (char*) realloc(prototype->buffer, aimedCapacity * typeSize);
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

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC bool _private_ACUtils_DynArray_shrinkToFit(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(prototype->calculateCapacity != NULL) {
            if(prototype->capacity > prototype->calculateCapacity(prototype->size)) {
                size_t capacityBackup = prototype->capacity;
                prototype->capacity = (prototype->size == 0) ? 0 : prototype->size - 1;
                if(!_private_ACUtils_DynArray_reserve(dynArray, prototype->capacity + 1, typeSize)) {
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

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC bool _private_ACUtils_DynArray_clear(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        ((struct private_ACUtils_DynArray_Prototype*) dynArray)->size = 0;
        return _private_ACUtils_DynArray_shrinkToFit(dynArray, typeSize);
    }
    return false;
}

static bool private_ACUtils_DynArray_shiftElements(void *dynArray, size_t index, size_t count, size_t typeSize)
{
    size_t dynArraySize = _private_ACUtils_DynArray_size(dynArray);
    if(index >= dynArraySize) {
        return true;
    } else if(_private_ACUtils_DynArray_reserve(dynArray, dynArraySize + count, typeSize)) {
        char *buffer = ((struct private_ACUtils_DynArray_Prototype*) dynArray)->buffer + (index * typeSize);
        memmove(buffer + (count * typeSize), buffer, (dynArraySize - index) * typeSize);
        return true;
    }
    return false;
}
ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC bool _private_ACUtils_DynArray_prepareInsertion(void* dynArray, size_t index, size_t valueCount, size_t typeSize)
{
    if(dynArray != NULL && valueCount > 0) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        if(index >= prototype->size) {
            if(!_private_ACUtils_DynArray_reserve(dynArray, prototype->size + valueCount, typeSize))
                return false;
        } else if(!private_ACUtils_DynArray_shiftElements(dynArray, index, valueCount, typeSize)) {
            return false;
        }
        prototype->size += valueCount;
        return true;
    }
    return valueCount == 0;
}
ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC bool _private_ACUtils_DynArray_insertArray(void *dynArray, size_t index, const void *array, size_t arraySize, size_t typeSize)
{
    if(array == NULL)
        arraySize = 0;
    if(_private_ACUtils_DynArray_prepareInsertion(dynArray, index, arraySize, typeSize)) {
        struct private_ACUtils_DynArray_Prototype* prototype = (struct private_ACUtils_DynArray_Prototype*) dynArray;
        memcpy(prototype->buffer + ((index >= prototype->size ? prototype->size - arraySize : index) * typeSize), array, arraySize * typeSize);
        return true;
    }
    return false;
}

ACUTILS_SYMBOL_ATTRIBUTES PRIVATE_ACUTILS_ST_FUNC void _private_ACUtils_DynArray_remove(void *dynArray, size_t index, size_t count, size_t typeSize)
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

#endif /* PRIVATE_ACUTILS_DYN_ARRAY_IMPLEMENT */
