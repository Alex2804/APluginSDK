#if !A_C_UTILS_ONE_SOURCE
#   include "../include/ACUtils/dynarray.h"
#endif

#ifdef A_C_UTILS_DYN_ARRAY_IMPLEMENT

#if A_C_UTILS_TEST
    ST_DATA size_t aCUtilsDynArrayTestMallocCount = 0;
    ST_DATA bool aCUtilsDynArrayTestMallocFail = false;
    ST_DATA size_t aCUtilsDynArrayTestReallocCount = 0;
    ST_DATA bool aCUtilsDynArrayTestReallocFail = false;
    ST_DATA size_t aCUtilsDynArrayTestFreeCount = 0;

    ST_FUNC void* aCUtilsDynArrayTestMalloc(size_t size) {
        if(!aCUtilsDynArrayTestMallocFail) {
            void* tmp = malloc(size);
            if(tmp != NULL) {
                ++aCUtilsDynArrayTestMallocCount;
            }
            return tmp;
        } else {
            return NULL;
        }
    }
    ST_FUNC void* aCUtilsDynArrayTestRealloc(void* ptr, size_t size) {
        if(!aCUtilsDynArrayTestReallocFail) {
            void* tmp = realloc(ptr, size);
            if(tmp != NULL) {
                ++aCUtilsDynArrayTestReallocCount;
            }
            return tmp;
        } else {
            return NULL;
        }
    }
    ST_FUNC void aCUtilsDynArrayTestFree(void* ptr) {
        if(ptr != NULL) {
            ++aCUtilsDynArrayTestFreeCount;
        }
        free(ptr);
    }

    #define malloc(size) aCUtilsDynArrayTestMalloc(size)
    #define realloc(ptr, size) aCUtilsDynArrayTestRealloc(ptr, size)
    #define free(ptr) aCUtilsDynArrayTestFree(ptr)
#endif

struct aCUtilsDynArrayPrototype
{
    char* buffer;
    size_t size;
    size_t capacity;
    size_t(*calculateCapacity)(size_t);
};

ST_FUNC size_t aCUtilsCalculateCapacityGeneric(size_t requiredSize, size_t minCapacity, size_t maxCapacity, size_t multiplier) {
    size_t multiplierExponent, capacity;
    if(requiredSize <= minCapacity)
        return minCapacity;
    multiplierExponent = ceil(log((double) requiredSize / minCapacity) / log(multiplier));
    capacity = minCapacity * pow(multiplier, multiplierExponent);
    if(capacity > maxCapacity)
        return maxCapacity;
    return capacity;
}
size_t aCUtilsCalculateCapacityDefault(size_t requiredSize) {
    return aCUtilsCalculateCapacityGeneric(requiredSize, 8, 8388608, 2); /* maxCapacity = minCapacity * pow(multiplier, 20) */
}


ST_FUNC void* aCUtilsDynArrayConstruct(size_t typeSize)
{
    struct aCUtilsDynArrayPrototype* prototype = (struct aCUtilsDynArrayPrototype*) malloc(sizeof(struct aCUtilsDynArrayPrototype));
    if(prototype != NULL) {
        prototype->size = 0;
        prototype->calculateCapacity = aCUtilsCalculateCapacityDefault;
        prototype->capacity = prototype->calculateCapacity(prototype->size);
        prototype->buffer = (char*) malloc(prototype->capacity * typeSize);
        if(prototype->buffer == NULL) {
            free(prototype);
        }
    }
    return prototype;
}

ST_FUNC void aCUtilsDynArrayDestruct(void *dynArray)
{
    if(dynArray != NULL) {
        free(((struct aCUtilsDynArrayPrototype*) dynArray)->buffer);
        free(dynArray);
    }
}

ST_FUNC size_t aCUtilsDynArraySize(void *dynArray)
{
    return (dynArray == NULL) ? 0 : ((struct aCUtilsDynArrayPrototype*) dynArray)->size;
}

ST_FUNC bool aCUtilsDynArrayReserve(void *dynArray, size_t reserveSize, size_t typeSize)
{
    if(dynArray != NULL) {
        struct aCUtilsDynArrayPrototype* prototype = (struct aCUtilsDynArrayPrototype*) dynArray;
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

ST_FUNC bool aCUtilsDynArrayShrinkToFit(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        struct aCUtilsDynArrayPrototype* prototype = (struct aCUtilsDynArrayPrototype*) dynArray;
        if(prototype->calculateCapacity != NULL) {
            if(prototype->capacity > prototype->calculateCapacity(prototype->size)) {
                size_t capacityBackup = prototype->capacity;
                prototype->capacity = (prototype->size == 0) ? 0 : prototype->size - 1;
                if(!aCUtilsDynArrayReserve(dynArray, prototype->capacity + 1, typeSize)) {
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

ST_FUNC bool aCUtilsDynArrayClear(void *dynArray, size_t typeSize)
{
    if(dynArray != NULL) {
        ((struct aCUtilsDynArrayPrototype*) dynArray)->size = 0;
        return aCUtilsDynArrayShrinkToFit(dynArray, typeSize);
    }
    return false;
}

ST_FUNC bool aCUtilsDynArrayShiftElements(void *dynArray, size_t index, size_t count, size_t typeSize)
{
    size_t dynArraySize = aCUtilsDynArraySize(dynArray);
    if(index >= dynArraySize) {
        return true;
    } else if(aCUtilsDynArrayReserve(dynArray, dynArraySize + count, typeSize)) {
        char *buffer = ((struct aCUtilsDynArrayPrototype*) dynArray)->buffer + (index * typeSize);
        memmove(buffer + (count * typeSize), buffer, (dynArraySize - index) * typeSize);
        return true;
    }
    return false;
}
ST_FUNC bool aCUtilsDynArrayPrepareInsertion(void* dynArray, size_t *index, size_t valueCount, size_t typeSize)
{
    if(dynArray != NULL && valueCount > 0) {
        struct aCUtilsDynArrayPrototype* prototype = (struct aCUtilsDynArrayPrototype*) dynArray;
        if(*index >= prototype->size) {
            *index = prototype->size;
            if(!aCUtilsDynArrayReserve(dynArray, prototype->size + valueCount, typeSize)) {
                return false;
            }
        } else if(!aCUtilsDynArrayShiftElements(dynArray, *index, valueCount, typeSize)) {
            return false;
        }
        prototype->size += valueCount;
        return true;
    }
    return valueCount == 0;
}
ST_FUNC bool aCUtilsDynArrayInsertArray(void *dynArray, size_t index, const void *array, size_t arraySize, size_t typeSize)
{
    if(array == NULL) {
        arraySize = 0;
    }
    if(aCUtilsDynArrayPrepareInsertion(dynArray, &index, arraySize, typeSize)) {
        struct aCUtilsDynArrayPrototype* prototype = (struct aCUtilsDynArrayPrototype*) dynArray;
        memcpy(prototype->buffer + (index * typeSize), array, arraySize * typeSize);
        return true;
    }
    return false;
}

ST_FUNC void aCUtilsDynArrayRemove(void *dynArray, size_t index, size_t count, size_t typeSize)
{
    struct aCUtilsDynArrayPrototype* prototype = (struct aCUtilsDynArrayPrototype*) dynArray;
    if(dynArray != NULL && count > 0 && index >= 0 && index < prototype->size) {
        if(index + count <= prototype->size) {
            memmove(prototype->buffer + (index * typeSize), prototype->buffer + ((index + count) * typeSize), (prototype->size - index - count) * typeSize);
            prototype->size -= count;
        } else {
            prototype->size = index;
        }
    }
}

#endif /* A_C_UTILS_DYN_ARRAY_IMPLEMENT */
