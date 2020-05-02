#ifndef ACUTILS_ADYNARRAY_H
#define ACUTILS_ADYNARRAY_H

#include <stddef.h>

#include "macros.h"
#include "types.h"

/**
 * Creates the struct declaration for an dynamic array.
 */
#define A_DYNAMIC_ARRAY_DEFINITION(name, type) \
    struct name \
    { \
        const ACUtilsReallocator reallocator; \
        const ACUtilsDeallocator deallocator; \
        size_t(*calculateCapacity)(size_t); \
        size_t size; \
        size_t capacity; \
        type* buffer; \
    }

/**
 * Constructs a dynamic array of the passed type and initializes it with size 0.
 * The passed dynamic array type must be defined with A_DYNAMIC_ARRAY_DEFINITION(name, type).
 *
 * @param ArrayType The type of the dynamic array to construct.
 * @return A Pointer to the constructed Array.
 */
#define ADynArray_construct(ArrayType) \
    ((ArrayType*) private_ACUtils_ADynArray_construct(sizeof(*((ArrayType*)NULL)->buffer)))

/**
 * Constructs a dynamic array of the passed type and initializes it with size 0.
 * The passed dynamic array type must be defined with A_DYNAMIC_ARRAY_DEFINITION(name, type).
 *
 * @param ArrayType The type of the dynamic array to construct.
 * @param allocator The function to allocate memory with.
 * @param reallocator The function to reallocate memory with.
 * @param deallocator The function to free memory with.
 * @return A Pointer to the constructed Array.
 */
#define ADynArray_constructWithAllocator(ArrayType, reallocator, deallocator) \
    ((ArrayType*) private_ACUtils_ADynArray_constructWithAllocator(sizeof(*((ArrayType*)NULL)->buffer), reallocator, deallocator))

/**
 * Destructs the dynamic array and releases all held resources.
 *
 * @param dynArray The dynamic array to destruct.
 */
#define ADynArray_destruct(dynArray) \
    private_ACUtils_ADynArray_destruct(dynArray)

/**
 * @param dynArray The dynamic array to get the size from.
 * @return The size (number of elements) of the passed dynamic array.
 */
#define ADynArray_size(dynArray) \
    private_ACUtils_ADynArray_size(dynArray)

/**
 * Resize dynArray, that it can hold at least reserveSize count items without resizing.
 * If the passed reserveSize is smaller than the current size or the current capacity (number of items that the array
 * can hold without resizing) of dynArray, nothing happens.
 *
 * If reserveSize <= dynamicArrayCapacityMax, reserveSize is treated if it is dynamicArrayCapacityMax and
 * if reserveSize > dynamicArrayCapacityMax, reserveSize is treated if it is dynamicArrayCapacityMax.
 *
 * If reallocation of memory fails, the dynamic array is not modified.
 *
 * @param dynArray The dynamic array to resize the buffer if necessary.
 * @param reserveSize The number of items that dynArray should be able to hold without resizing.
 *
 * @return True if dynArray can hold at least reserveSize count elements after this operation, false if not.
 */
#define ADynArray_reserve(dynArray, reserveSize) \
    private_ACUtils_ADynArray_reserve(dynArray, reserveSize, sizeof(*(dynArray)->buffer))

/**
 * Resize dynArray to the minimum size to fit its content (dependent on the resize strategy, which means that the
 * buffer is maybe not exactly as big as the size).
 *
 * @param dynArray The dynamic array to shrink the capacity to fit its content.
 *
 * @return True if dynArray is small as possible or was successfully resized, false if not.
 */
#define ADynArray_shrinkToFit(dynArray) \
    private_ACUtils_ADynArray_shrinkToFit(dynArray, sizeof(*(dynArray)->buffer))

/**
 * Clears the content of dynArray and calls dynArrayShrinkToFit(dynArray) after that.
 *
 * @param dynArray The dynamic array to clear.
 *
 * @return True if dynArray was successfully cleared and shrinked to the minimum possible size, false if only cleared
 * but not shrinked.
 */
#define ADynArray_clear(dynArray) \
    private_ACUtils_ADynArray_clear(dynArray, sizeof(*(dynArray)->buffer))

/**
 * Inserts the value into dynArray at index. If index is bigger or equal to the size of
 * dynArray, the value gets appended.
 * Shifts all elements which were previous at or behind index, one index, to the right.
 *
 * @param dynArray The dynamic array in which the value should be inserted.
 * @param index The index at which the value should be inserted.
 * @param value The value to insert.
 *
 * @return True if the value was inserted successfully, false if not.
 */
#define ADynArray_insert(dynArray, index, value) \
    (private_ACUtils_ADynArray_prepareInsertion(dynArray, index, 1, sizeof(*(dynArray)->buffer)) \
        ? (((dynArray)->buffer[(index >= (dynArray)->size) ? (dynArray)->size - 1 : index] = (value)) ? true : true) \
        : false)

/**
 * Inserts the elements of array into dynArray at index. If index is bigger or equal to the size of
 * dynArray, the elements get appended.
 * Shifts all elements which were previous at or behind index, arraySize count indices. to the right.
 *
 * The elements gets copied from array to the buffer of dynArray with memcpy.
 *
 * @param dynArray The dynamic array in which the values should be inserted.
 * @param index The index at which the values should be inserted.
 * @param array A pointer to the array from which the elements should be copied.
 * @param arraySize The count of elements which should be copied from array to dynArray.
 *
 * @return True if the values were added successfully, false if not.
 */
#define ADynArray_insertArray(dynArray, index, array, arraySize) \
    (sizeof(*(dynArray)->buffer) != sizeof(*array) ? \
        false : private_ACUtils_ADynArray_insertArray(dynArray, index, array, arraySize, sizeof(*(dynArray)->buffer)))

/**
 * Inserts the elements of srcDynArray into destDynArray at index. If index is bigger or equal to the size of
 * destDynArray, the elements get appended.
 * Shifts all elements which were previous at or behind index, the size of srcDynArray indices, to the right.
 *
 * The elements gets copied from the buffer of srcDynArray to the buffer of destDynArray with memcpy.
 *
 * @param destDynArray The dynamic array in which the values should be inserted.
 * @param index The index at which the values should be inserted.
 * @param srcDynArray The dynamic array from which the values should be copied.
 *
 * @return True if the values were inserted successfully, false if not.
 */
#define ADynArray_insertADynArray(destDynArray, index, srcDynArray) \
    (srcDynArray == NULL ? \
        destDynArray != NULL : ADynArray_insertArray(destDynArray, index, (srcDynArray)->buffer, (srcDynArray)->size))

/**
 * Adds the value to the end of dynArray.
 *
 * @param dynArray The dynamic array to which the value should be added.
 * @param value The value to add.
 *
 * @return True if the value was added successfully, false if not.
 */
#define ADynArray_add(dynArray, value) \
    ADynArray_insert(dynArray, -1, value)
/**
 * Adds the elements of array to the end of dynArray.
 *
 * The elements gets copied from array to the buffer of dynArray with memcpy.
 *
 * @param dynArray The dynamic array to which the values should be added.
 * @param array A pointer to the array from which the elements should be copied.
 * @param arraySize The count of elements which should be copied from array to dynArray.
 *
 * @return True if the values were added successfully, false if not.
 */
#define ADynArray_addArray(dynArray, array, arraySize) \
    ADynArray_insertArray(dynArray, -1, array, arraySize)
/**
 * Adds the elements of srcDynArray to the end of destDynArray.
 *
 * The elements gets copied from the buffer of srcDynArray to the buffer of destDynArray with memcpy.
 *
 * @param destDynArray The dynamic array to which the values should be added.
 * @param srcDynArray The dynamic array from which the values should be copied.
 *
 * @return True if the values were added successfully, false if not.
 */
#define ADynArray_addADynArray(destDynArray, srcDynArray) \
    ADynArray_insertADynArray(destDynArray, ((size_t)-1), srcDynArray)

/**
 * Sets the element in dynArray at index to value. If index is bigger than the size of dynArray
 * The value is appended to dynArray as new element.
 *
 * @param dynArray The dynamic array to set the index of.
 * @param index The index at which the element should be set to value.
 * @param value The value that should be set at index.
 *
 * @return True if the value was set, false if not.
 */
#define ADynArray_set(dynArray, index, value) \
    (((dynArray) == NULL) \
        ? false \
        : ((((size_t) index) < (dynArray)->size) \
            ? (((dynArray)->buffer[(size_t) index] = value) ? true : true) \
            : ADynArray_add((dynArray), (value))))

/**
 * Removes count elements in dynArray starting at index. This operation doesn't affect the capacity.
 * The elements behind index + count are shifted to the right by count.
 *
 * If index + count is bigger or equal to the size of dynArray, all elements behind index gets removed.
 *
 * @param dynArray The dynamic array to remove elements from.
 * @param index The start index from which count elements should be removed.
 * @param count The number of elements to remove starting at index.
 */
#define ADynArray_remove(dynArray, index, count) \
    private_ACUtils_ADynArray_remove(dynArray, index, count, sizeof(*(dynArray)->buffer))

/**
 * Retrieves the element at index in dynArray.
 *
 * No bound checking is performed. The behavior is undefined if index is out of bounds!
 *
 * @param dynArray The dynamic array to get the element from.
 * @param index The index in dynArray to get the element of.
 *
 * @return The element in dynArray at index.
 */
#define ADynArray_get(dynArray, index) \
    ((dynArray)->buffer[index])

/**
 * Retrieves the buffer of the dynArray.
 *
 * @param dynArray The dynamic array to get the buffer from.
 *
 * @return The buffer of dynArray.
 */
#define ADynArray_buffer(dynArray) \
    ((dynArray)->buffer)


ACUTILS_ST_FUNC size_t private_ACUtils_ADynArray_calculateCapacityGeneric(size_t requiredSize, size_t minCapacity, size_t maxCapacity, double multiplier);
ACUTILS_ST_FUNC void* private_ACUtils_ADynArray_construct(size_t typeSize);
ACUTILS_ST_FUNC void* private_ACUtils_ADynArray_constructWithAllocator(size_t typeSize, ACUtilsReallocator reallocator, ACUtilsDeallocator deallocator);
ACUTILS_ST_FUNC void private_ACUtils_ADynArray_destruct(void *dynArray);
ACUTILS_ST_FUNC size_t private_ACUtils_ADynArray_size(void *dynArray);
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_reserve(void *dynArray, size_t reserveSize, size_t typeSize);
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_shrinkToFit(void *dynArray, size_t typeSize);
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_clear(void *dynArray, size_t typeSize);
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_prepareInsertion(void* dynArray, size_t index, size_t valueCount, size_t typeSize);
ACUTILS_ST_FUNC bool private_ACUtils_ADynArray_insertArray(void *dynArray, size_t index, const void *array, size_t arraySize, size_t typeSize);
ACUTILS_ST_FUNC void private_ACUtils_ADynArray_remove(void *dynArray, size_t index, size_t count, size_t typeSize);

#ifdef ACUTILS_ONE_SOURCE
#   include "../../src/adynarray.c"
#endif

#endif /* ACUTILS_ADYNARRAY_H */
