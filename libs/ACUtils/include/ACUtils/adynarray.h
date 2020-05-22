#ifndef ACUTILS_ADYNARRAY_H
#define ACUTILS_ADYNARRAY_H

#include "macros.h"
#include "types.h"

ACUTILS_OPEN_EXTERN_C

/**
 * Creates the struct declaration for an dynamic array.
 */
#define A_DYNAMIC_ARRAY_DEFINITION(name, type) \
    struct name \
    { \
        const ACUtilsReallocator reallocator; \
        const ACUtilsDeallocator deallocator; \
        ACUtilsGrowStrategy growStrategy; \
        size_t size; \
        size_t capacity; \
        type* buffer; \
    }

/**
 * Constructs a dynamic array of the passed type and initializes it with size 0.
 * The passed dynamic array type must be defined with A_DYNAMIC_ARRAY_DEFINITION(name, type).
 *
 * @param ArrayType The type of the dynamic array to construct.
 * @return A Pointer to the constructed Array or null on failure.
 */
#define ADynArray_construct(ArrayType) \
    ((ArrayType*) private_ACUtils_ADynArray_construct(sizeof(*((ArrayType*) 0)->buffer)))
/**
 * Constructs a dynamic array of the passed type and initializes it with size 0.
 * The passed dynamic array type must be defined with A_DYNAMIC_ARRAY_DEFINITION(name, type) and the passed reallocator
 * and deallocator must be pointer to a valid reallocator and deallocator!
 *
 * @param ArrayType The type of the dynamic array to construct.
 * @param reallocator The function to reallocate memory with. This must be not null!
 * @param deallocator The function to free memory with. This must be not null!
 * @return A Pointer to the constructed Array or null on failure.
 */
#define ADynArray_constructWithAllocator(ArrayType, reallocator, deallocator) \
    ((ArrayType*) private_ACUtils_ADynArray_constructWithAllocator(sizeof(*((ArrayType*) 0)->buffer), reallocator, deallocator))
/**
 * Destructs the dynamic array and releases all held resources.
 *
 * @param dynArray The dynamic array to destruct.
 */
#define ADynArray_destruct(dynArray) \
    private_ACUtils_ADynArray_destruct(dynArray)

/**
 * @param dynArray The dynamic array to get the reallocator from.
 * @return The reallocator of dynArray.
 */
#define ADynArray_reallocator(dynArray) \
    (((dynArray) == nullptr) ? nullptr : (dynArray)->reallocator)
/**
 * @param dynArray The dynamic array to get the deallocatr from.
 * @return The deallocator of dynArray.
 */
#define ADynArray_deallocator(dynArray) \
    (((dynArray) == nullptr) ? nullptr : (dynArray)->deallocator)
/**
 * @param dynArray The dynamic array to get the grow strategy from.
 * @return The grow strategy of dynArray.
 */
#define ADynArray_getGrowStrategy(dynArray) \
    (((dynArray) == nullptr) ? nullptr : (dynArray)->growStrategy)
/**
 * @param dynArray The dynamic array to set the grow strategy for.
 * @param growStrategy The new grow strategy function for dynArray.
 */
#define ADynArray_setGrowStrategy(dynArray, growStrategy) \
    (private_ACUtils_ADynArray_setGrowStrategy(dynArray, growStrategy))

/**
 * @param dynArray The dynamic array to get the buffer from.
 * @return The buffer of dynArray.
 */
#define ADynArray_buffer(dynArray) \
    (((dynArray) == nullptr) ? nullptr : (dynArray)->buffer)
/**
 * @param dynArray The dynamic array to get the size from.
 * @return The size (number of elements) of the passed dynamic array.
 */
#define ADynArray_size(dynArray) \
    private_ACUtils_ADynArray_size(dynArray)
/**
 * @param dynArray The dynamic array to get the capacity from.
 * @return The capacity of dynArray (The maximum number of elements the Buffer can hold until it needs to be resized).
 */
#define ADynArray_capacity(dynArray) \
    private_ACUtils_ADynArray_capacity(dynArray)

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
 * @return True if dynArray can hold at least reserveSize count elements after this operation, false if not.
 */
#define ADynArray_reserve(dynArray, reserveSize) \
    private_ACUtils_ADynArray_reserve(dynArray, reserveSize, false, sizeof(*(dynArray)->buffer))
/**
 * Resize dynArray to the minimum size to exactly fit its content, except the buffer is smaller than the minimum size
 * that the grow strategy returns (for passed size 0).
 *
 * @param dynArray The dynamic array to shrink the capacity to fit its content.
 * @return True if dynArray is small as possible or was successfully resized, false if not.
 */
#define ADynArray_shrinkToFit(dynArray) \
    private_ACUtils_ADynArray_shrinkToFit(dynArray, sizeof(*(dynArray)->buffer))

/**
 * Clears the content of dynArray. The allocated buffer doesn't change.
 *
 * @param dynArray The dynamic array to clear.
 */
#define ADynArray_clear(dynArray) \
    private_ACUtils_ADynArray_clear(dynArray)
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
 * Inserts the value into dynArray at index. If index is bigger or equal to the size of
 * dynArray, the value gets appended.
 * Shifts all elements which were previous at or behind index, one index, to the right.
 *
 * @param dynArray The dynamic array in which the value should be inserted.
 * @param index The index at which the value should be inserted.
 * @param value The value to insert. This must be an lvalue!
 * @return True if the value was inserted successfully, false if not.
 */
#define ADynArray_insert(dynArray, index, value) \
    private_ACUtils_ADynArray_insertArray(dynArray, index, &(value), 1, sizeof(*(dynArray)->buffer))
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
 * @return True if the values were added successfully, false if not.
 */
#define ADynArray_insertArray(dynArray, index, array, arraySize) \
    private_ACUtils_ADynArray_insertArray(dynArray, index, array, arraySize, sizeof(*(dynArray)->buffer))
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
 * @return True if the values were inserted successfully, false if not.
 */
#define ADynArray_insertADynArray(destDynArray, index, srcDynArray) \
    (srcDynArray == nullptr ? \
        destDynArray != nullptr : ADynArray_insertArray(destDynArray, index, (srcDynArray)->buffer, (srcDynArray)->size))

/**
 * Adds the value to the end of dynArray.
 *
 * @param dynArray The dynamic array to which the value should be added.
 * @param value The value to add. This must be an lvalue!
 * @return True if the value was added successfully, false if not.
 */
#define ADynArray_append(dynArray, value) \
    ADynArray_insert(dynArray, -1, value)
/**
 * Adds the elements of array to the end of dynArray.
 *
 * The elements gets copied from array to the buffer of dynArray with memcpy.
 *
 * @param dynArray The dynamic array to which the values should be added.
 * @param array A pointer to the array from which the elements should be copied.
 * @param arraySize The count of elements which should be copied from array to dynArray.
 * @return True if the values were added successfully, false if not.
 */
#define ADynArray_appendArray(dynArray, array, arraySize) \
    ADynArray_insertArray(dynArray, -1, array, arraySize)
/**
 * Adds the elements of srcDynArray to the end of destDynArray.
 *
 * The elements gets copied from the buffer of srcDynArray to the buffer of destDynArray with memcpy.
 *
 * @param destDynArray The dynamic array to which the values should be added.
 * @param srcDynArray The dynamic array from which the values should be copied.
 * @return True if the values were added successfully, false if not.
 */
#define ADynArray_appendADynArray(destDynArray, srcDynArray) \
    ADynArray_insertADynArray(destDynArray, ((size_t)-1), srcDynArray)

/**
 * Sets the element in dynArray at index to value. If index is bigger than the size of dynArray
 * The value is appended to dynArray as new element.
 *
 * @param dynArray The dynamic array to set the element of.
 * @param index The index at which the element should be set to value.
 * @param value The value that should be set at index. This must be an lvalue!
 * @return True if the value was set, false if not.
 */
#define ADynArray_set(dynArray, index, value) \
    ADynArray_setRange(dynArray, index, 1, value)
/**
 * Sets count elements in dynArray starting from index to value. If index + count is bigger than the size of
 * dynArray, the values that are out of bounds gets appended to dynArray as new elements.
 *
 * @param dynArray The dynamic array to set the elements of.
 * @param index The index of the first element which should be set to value.
 * @param count The number of elements to replace, starting at index.
 * @param value The value that should be set at index. This must be an lvalue!
 * @return True if the elements were set, false if not.
 */
#define ADynArray_setRange(dynArray, index, count, value) \
    private_ACUtils_ADynArray_setRange(dynArray, index, count, &(value), sizeof(*(dynArray)->buffer))
/**
 * Retrieves the element at index in dynArray.
 *
 * No bound checking is performed. The behavior is undefined if index is out of bounds!
 *
 * @param dynArray The dynamic array to get the element from. Mustn't be null'
 * @param index The index in dynArray to get the element of.
 * @return The element in dynArray at index.
 */
#define ADynArray_get(dynArray, index) \
    (dynArray->buffer[index])

#ifdef ACUTILS_ONE_SOURCE
#   include "../../src/adynarray.c"
#else
    size_t private_ACUtils_ADynArray_growStrategyGeneric(size_t, size_t, size_t, double, size_t);
    void* private_ACUtils_ADynArray_construct(size_t);
    void* private_ACUtils_ADynArray_constructWithAllocator(size_t, ACUtilsReallocator, ACUtilsDeallocator);
    void private_ACUtils_ADynArray_destruct(void*);
    void private_ACUtils_ADynArray_setGrowStrategy(void*, ACUtilsGrowStrategy);
    size_t private_ACUtils_ADynArray_size(const void*);
    size_t private_ACUtils_ADynArray_capacity(const void*);
    bool private_ACUtils_ADynArray_reserve(void*, size_t, bool, size_t);
    bool private_ACUtils_ADynArray_shrinkToFit(void*, size_t);
    void private_ACUtils_ADynArray_clear(void*);
    void private_ACUtils_ADynArray_remove(void*, size_t, size_t, size_t);
    bool private_ACUtils_ADynArray_insertArray(void*, size_t, const void*, size_t, size_t);
    bool private_ACUtils_ADynArray_setRange(void*, size_t, size_t, void*, size_t);
#endif

ACUTILS_CLOSE_EXTERN_C

#endif /* ACUTILS_ADYNARRAY_H */
