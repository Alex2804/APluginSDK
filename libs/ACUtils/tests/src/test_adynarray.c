#include "check.h"

#include <string.h>
#include <stdlib.h>

#include "ACUtils/adynarray.h"

static size_t private_ACUtilsTest_DynArray_reallocCount = 0;
static bool private_ACUtilsTest_DynArray_reallocFail = false;
static size_t private_ACUtilsTest_DynArray_freeCount = 0;

static void* private_ACUtilsTest_DynArray_realloc(void* ptr, size_t size) {
    if(!private_ACUtilsTest_DynArray_reallocFail) {
        void* tmp = realloc(ptr, size);
        if(tmp != NULL) {
            ++private_ACUtilsTest_DynArray_reallocCount;
        }
        return tmp;
    } else {
        return NULL;
    }
}
static void private_ACUtilsTest_DynArray_free(void* ptr) {
    if(ptr != NULL) {
        ++private_ACUtilsTest_DynArray_freeCount;
    }
    free(ptr);
}

static const size_t aDynArrayTestCapacityMin = 8;
static const size_t aDynArrayTestCapacityMax = 32;
static const double aDynArrayTestCapacityMul = 2;

static size_t calculateCapacityTest(size_t requiredSize) {
    return private_ACUtils_ADynArray_calculateCapacityGeneric(
            requiredSize, aDynArrayTestCapacityMin, aDynArrayTestCapacityMax, aDynArrayTestCapacityMul);
}

A_DYNAMIC_ARRAY_DEFINITION(DynStringTestArray, char);

START_TEST(test_aDynArrayConstruct_aDynArrayDestruct_valid)
{
    struct DynStringTestArray* array;
    array = ADynArray_construct(struct DynStringTestArray);
    ck_assert_uint_eq(array->size, 0);
    ck_assert_uint_eq(array->capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array->buffer);
    ck_assert_ptr_nonnull(array->calculateCapacity);
    ADynArray_destruct(array);
}
END_TEST
START_TEST(test_aDynArrayConstruct_aDynArrayDestruct_withAllocator_valid)
{
    struct DynStringTestArray* array;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = private_ACUtilsTest_DynArray_freeCount = 0;
    array = ADynArray_constructWithAllocator(struct DynStringTestArray, private_ACUtilsTest_DynArray_realloc, private_ACUtilsTest_DynArray_free);
    ck_assert_uint_eq(array->size, 0);
    ck_assert_uint_eq(array->capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array->buffer);
    ck_assert_ptr_nonnull(array->calculateCapacity);
    ADynArray_destruct(array);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, private_ACUtilsTest_DynArray_freeCount);
}
END_TEST
START_TEST(test_aDynArrayConstruct_aDynArrayDestruct_noMemoryAvailable)
{
    struct DynStringTestArray* array;
    private_ACUtilsTest_DynArray_reallocFail = true;
    private_ACUtilsTest_DynArray_reallocCount = private_ACUtilsTest_DynArray_freeCount = 0;
    array = ADynArray_constructWithAllocator(struct DynStringTestArray, private_ACUtilsTest_DynArray_realloc,
                                             private_ACUtilsTest_DynArray_free);
    ck_assert_ptr_null(array);
    ADynArray_destruct(array); /* should do nothing */
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_freeCount, 0);
}
END_TEST
START_TEST(test_aDynArrayConstruct_aDynArrayDestruct_nullptr)
{
    struct DynStringTestArray* array = NULL;
    private_ACUtilsTest_DynArray_freeCount = 0;
    ADynArray_destruct(array); /* should do nothing */
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_freeCount, 0);
}
END_TEST


START_TEST(test_aDynArraySize_valid)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.capacity = 0;
    array.buffer = NULL;
    array.calculateCapacity = NULL;
    array.size = 42;
    ck_assert_uint_eq(ADynArray_size(&array), 42);
    array.size = 13;
    ck_assert_uint_eq(ADynArray_size(&array), 13);
    array.size = 0;
    ck_assert_uint_eq(ADynArray_size(&array), 0);
}
START_TEST(test_aDynArraySize_nullptr)
{
    struct DynStringTestArray* arrayPtr = NULL;
    ck_assert_uint_eq(ADynArray_size(arrayPtr), 0);
}


START_TEST(test_aDynArrayReserve_success_enoughCapacityBufferNotNull)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 0;
    array.calculateCapacity = NULL;
    array.capacity = aDynArrayTestCapacityMin;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_reserve(&array, array.capacity), true);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_null(array.calculateCapacity);
    array.deallocator(array.buffer);
}
START_TEST(test_aDynArrayReserve_success_enoughCapacityBufferNull)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 0;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = aDynArrayTestCapacityMin;
    array.buffer = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_reserve(&array, array.capacity), true);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayReserve_success_notEnoughCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 0;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = aDynArrayTestCapacityMin;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_reserve(&array, aDynArrayTestCapacityMax), true);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMax);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayReserve_failure_biggerThanMaxCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 0;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = aDynArrayTestCapacityMin;
    array.buffer = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_reserve(&array, aDynArrayTestCapacityMax + 1), false);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_null(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST
START_TEST(test_aDynArrayReserve_failure_noMemoryAvailable)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 0;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = aDynArrayTestCapacityMin;
    array.buffer = NULL;
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_reserve(&array, aDynArrayTestCapacityMax), false);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_null(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
}
END_TEST
START_TEST(test_aDynArrayReserve_failure_calculateCapacityNull)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 0;
    array.calculateCapacity = NULL;
    array.capacity = aDynArrayTestCapacityMin;
    array.buffer = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_reserve(&array, aDynArrayTestCapacityMax), false);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_null(array.buffer);
    ck_assert_ptr_null(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST
START_TEST(test_aDynArrayReserve_failure_nullptr)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_reserve(arrayPtr, aDynArrayTestCapacityMax), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayShrinkToFit_success_hasLeastCapacityBufferNotNull)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin - 1;
    array.capacity = aDynArrayTestCapacityMin;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    array.calculateCapacity = calculateCapacityTest;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_shrinkToFit(&array), true);
    ck_assert_uint_eq(array.size, aDynArrayTestCapacityMin - 1);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
START_TEST(test_aDynArrayShrinkToFit_success_hasLeastCapacityBufferNull)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin - 1;
    array.capacity = aDynArrayTestCapacityMin;
    array.buffer = NULL;
    array.calculateCapacity = calculateCapacityTest;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_shrinkToFit(&array), true);
    ck_assert_uint_eq(array.size, aDynArrayTestCapacityMin - 1);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_null(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayShrinkToFit_success_hasNotLeastCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin - 1;
    array.capacity = calculateCapacityTest(aDynArrayTestCapacityMin + 1);
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    array.calculateCapacity = calculateCapacityTest;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_shrinkToFit(&array), true);
    ck_assert_uint_eq(array.size, aDynArrayTestCapacityMin - 1);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayShrinkToFit_failure_noMemoryAvailable)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin - 1;
    array.capacity = calculateCapacityTest(aDynArrayTestCapacityMin + 1);
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    array.calculateCapacity = calculateCapacityTest;
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_shrinkToFit(&array), false);
    ck_assert_uint_eq(array.size, aDynArrayTestCapacityMin - 1);
    ck_assert_uint_eq(array.capacity, calculateCapacityTest(aDynArrayTestCapacityMin + 1));
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayShrinkToFit_failure_calculateCapacityNull)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin - 1;
    array.capacity = calculateCapacityTest(aDynArrayTestCapacityMin + 1);
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    array.calculateCapacity = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_shrinkToFit(&array), false);
    ck_assert_uint_eq(array.size, aDynArrayTestCapacityMin - 1);
    ck_assert_uint_eq(array.capacity, calculateCapacityTest(aDynArrayTestCapacityMin + 1));
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_null(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayShrinkToFit_failure_nullptr)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_shrinkToFit(arrayPtr), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayClear_success_shrinked)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin + 1;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = (size_t) (aDynArrayTestCapacityMin * aDynArrayTestCapacityMul);
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_clear(&array), true);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, aDynArrayTestCapacityMin);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayClear_failure_notShrinked)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = aDynArrayTestCapacityMin + 1;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = (size_t) (aDynArrayTestCapacityMin * aDynArrayTestCapacityMul);
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_clear(&array), false);
    ck_assert_uint_eq(array.size, 0);
    ck_assert_uint_eq(array.capacity, (size_t) (aDynArrayTestCapacityMin * aDynArrayTestCapacityMul));
    ck_assert_ptr_nonnull(array.buffer);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayClear_failure_nullptr)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_clear(arrayPtr), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayInsert_success_zeroIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "1234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(&array, 0, '0'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[0], '0');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_success_middleIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0134", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(&array, 2, '2'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[2], '2');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_success_endIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(&array, 5, '\0'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[5], '\0');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_success_beyondEndIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(&array, 666, '\0'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[5], '\0');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_success_bufferExpanded)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0134", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(&array, 2, '2'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[2], '2');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_success_negativeIndexGetsMaxIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(&array, -1, '\0'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_failure_bufferExpansionFailed)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0134", 5);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_insert(&array, 2, '2'), false);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0134");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsert_failure_nullptr)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insert(arrayPtr, 0, '0'), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayInsertArray_success_zeroIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "3456", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 0, "012", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_middleIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 2, "234", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_endIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 5, "56\0", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_beyondEndIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 666, "56\0", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_bufferExpanded)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 2, "234", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_nullptrArray)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    char* nullptrArray = NULL;
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 2, nullptrArray, 3), true);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0156");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_zeroArraySize)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 2, "234", 0), true);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0156");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_success_negativeIndexGetsMaxIndex)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(&array, -1, "56\0", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_failure_bufferExpansionFailed)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 2, "234", 3), false);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0156");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertArray_failure_nullptrDestArray)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(arrayPtr, 0, "012", 3), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayInsertDynArray_success_zeroIndex)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "3456", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "012\0", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 0, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "012");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_middleIndex)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0156", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "234\0", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 2, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "234");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_endIndex)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "01234", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "56\0", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 5, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "56");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_beyondEndIndex)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "01234", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "56", 3);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 666, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "56");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_bufferExpanded)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0156", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "234", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 2, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "234");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_nullptrSrcArray)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray *srcArray = NULL;
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 2, srcArray), true);
    ck_assert_uint_eq(destArray.size, 5);
    ck_assert_uint_eq(destArray.capacity, 5);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0156");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_zeroSizeSrcArray)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0156", 5);
    srcArray.size = 0;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 2, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 5);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0156");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 0);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_success_negativeIndexGetsMaxIndex)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "01234", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "56", 3);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, -1, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "56");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_failure_bufferExpansionFailed)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0156", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "234", 4);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_insertADynArray(&destArray, 2, &srcArray), false);
    ck_assert_uint_eq(destArray.size, 5);
    ck_assert_uint_eq(destArray.capacity, 5);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0156");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "234");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayInsertDynArray_failure_nullptrDestArray)
{
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray *destArrayPtr = NULL;
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "012", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertADynArray(destArrayPtr, 0, &srcArray), false);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "012");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST


START_TEST(test_aDynArrayAdd_success_enoughCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_add(&array, '\0'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[array.size - 1], '\0');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAdd_success_notEnoughCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_add(&array, '\0'), true);
    ck_assert_uint_eq(array.size, 6);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_int_eq(array.buffer[array.size - 1], '\0');
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAdd_failure_bufferExpansionFailed)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "01234", 6);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_add(&array, '\0'), false);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "01234");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAdd_failure_nullptr)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_add(arrayPtr, '\0'), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayAddArray_success_enoughCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addArray(&array, "56\0", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddArray_success_notEnoughCapacity)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "01234", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addArray(&array, "56\0", 3), true);
    ck_assert_uint_eq(array.size, 8);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddArray_success_nullptrArray)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    char* nullptrArray = NULL;
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addArray(&array, nullptrArray, 3), true);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0156");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddArray_success_zeroArraySize)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0156", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addArray(&array, "234", 0), true);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0156");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddArray_failure_bufferExpansionFailed)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 5;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "0123", 5);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_insertArray(&array, 2, "45\0", 3), false);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 5);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddArray_failure_nullptrDestArray)
{
    struct DynStringTestArray* arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_insertArray(arrayPtr, 0, "012", 3), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayAddDynArray_success_enoughCapacity)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "01234", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "56", 3);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addADynArray(&destArray, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "56");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddDynArray_success_notEnoughCapacity)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "01234", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "56", 3);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addADynArray(&destArray, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 8);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123456");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "56");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddDynArray_success_nullptrSrcArray)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray *srcArray = NULL;
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0123", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addADynArray(&destArray, srcArray), true);
    ck_assert_uint_eq(destArray.size, 5);
    ck_assert_uint_eq(destArray.capacity, 5);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddDynArray_success_zeroSizeSrcArray)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 8;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0123", 5);
    srcArray.size = 0;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addADynArray(&destArray, &srcArray), true);
    ck_assert_uint_eq(destArray.size, 5);
    ck_assert_uint_eq(destArray.capacity, 8);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0123");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 0);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddDynArray_failure_bufferExpansionFailed)
{
    struct DynStringTestArray destArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    destArray.size = 5;
    destArray.calculateCapacity = calculateCapacityTest;
    destArray.capacity = 5;
    private_ACUtilsTest_DynArray_reallocFail = false;
    destArray.buffer = malloc(destArray.capacity);
    memcpy(destArray.buffer, "0156", 5);
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "234", 4);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_addADynArray(&destArray, &srcArray), false);
    ck_assert_uint_eq(destArray.size, 5);
    ck_assert_uint_eq(destArray.capacity, 5);
    ck_assert_ptr_nonnull(destArray.buffer);
    ck_assert_str_eq(destArray.buffer, "0156");
    ck_assert_ptr_nonnull(destArray.calculateCapacity);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "234");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    destArray.deallocator(destArray.buffer);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST
START_TEST(test_aDynArrayAddDynArray_failure_nullptrDestArray)
{
    struct DynStringTestArray srcArray = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    struct DynStringTestArray *destArrayPtr = NULL;
    srcArray.size = 3;
    srcArray.calculateCapacity = calculateCapacityTest;
    srcArray.capacity = 8;
    srcArray.buffer = malloc(srcArray.capacity);
    memcpy(srcArray.buffer, "012", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_addADynArray(destArrayPtr, &srcArray), false);
    ck_assert_uint_eq(srcArray.size, 3);
    ck_assert_uint_eq(srcArray.capacity, 8);
    ck_assert_ptr_nonnull(srcArray.buffer);
    ck_assert_str_eq(srcArray.buffer, "012");
    ck_assert_ptr_nonnull(srcArray.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    srcArray.deallocator(srcArray.buffer);
}
END_TEST


START_TEST(test_aDynArraySet_success_indexInBounds)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 4;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 4;
    array.buffer = malloc(array.capacity);
    memcpy(array.buffer, "012", 4);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_set(&array, 0, '2'), true);
    ck_assert_uint_eq(ADynArray_set(&array, 1, '1'), true);
    ck_assert_uint_eq(ADynArray_set(&array, 2, '0'), true);
    ck_assert_uint_eq(array.size, 4);
    ck_assert_uint_eq(array.capacity, 4);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "210");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArraySet_success_indexBeyondSize)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 3;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 4;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "012\0\0", 5);
    private_ACUtilsTest_DynArray_reallocFail = true;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_set(&array, 3, '3'), true);
    ck_assert_uint_eq(array.size, 4);
    ck_assert_uint_eq(array.capacity, 4);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArraySet_success_indexBeyondSize_bufferExpanded)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 3;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 3;
    array.buffer = malloc(array.capacity + 2);
    memcpy(array.buffer, "012\0\0", 5);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_set(&array, 3, '3'), true);
    ck_assert_uint_eq(array.size, 4);
    ck_assert_uint_eq(array.capacity, 8);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 1);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArraySet_failure_indexBeyondSize_bufferExpansionFailed)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 3;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 3;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "012", 4);
    private_ACUtilsTest_DynArray_reallocFail = true;
    ck_assert_uint_eq(ADynArray_set(&array, 3, '3'), false);
    ck_assert_uint_eq(array.size, 3);
    ck_assert_uint_eq(array.capacity, 3);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "012");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArraySet_failure_nullptr)
{
    struct DynStringTestArray *arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ck_assert_uint_eq(ADynArray_set(arrayPtr, 0, '0'), false);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST


START_TEST(test_aDynArrayRemove_indexRangeInBounds)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 11;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 16;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "0123456789", 11);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ADynArray_remove(&array, 2, 6);
    ck_assert_uint_eq(array.size, 5);
    ck_assert_uint_eq(array.capacity, 16);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0189");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayRemove_rangeBeyondBounds)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 11;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 16;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "0123456789", 11);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ADynArray_remove(&array, 2, 100);
    ck_assert_uint_eq(array.size, 2);
    ck_assert_uint_eq(array.capacity, 16);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayRemove_zeroRange)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 11;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 16;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "0123456789", 11);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ADynArray_remove(&array, 2, 0);
    ck_assert_uint_eq(array.size, 11);
    ck_assert_uint_eq(array.capacity, 16);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456789");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayRemove_indexBeyoundBounds)
{
    struct DynStringTestArray array = {.reallocator = private_ACUtilsTest_DynArray_realloc, .deallocator = private_ACUtilsTest_DynArray_free};
    array.size = 11;
    array.calculateCapacity = calculateCapacityTest;
    array.capacity = 16;
    array.buffer = malloc(array.capacity + 1);
    memcpy(array.buffer, "0123456789", 11);
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ADynArray_remove(&array, 13, 5);
    ck_assert_uint_eq(array.size, 11);
    ck_assert_uint_eq(array.capacity, 16);
    ck_assert_ptr_nonnull(array.buffer);
    ck_assert_str_eq(array.buffer, "0123456789");
    ck_assert_ptr_nonnull(array.calculateCapacity);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
    array.deallocator(array.buffer);
}
END_TEST
START_TEST(test_aDynArrayRemove_nullptr)
{
    struct DynStringTestArray *arrayPtr = NULL;
    private_ACUtilsTest_DynArray_reallocFail = false;
    private_ACUtilsTest_DynArray_reallocCount = 0;
    ADynArray_remove(arrayPtr, 5, 10);
    ck_assert_uint_eq(private_ACUtilsTest_DynArray_reallocCount, 0);
}
END_TEST



Suite* test_suite_dynarray()
{
    Suite *s;
    TCase *test_case_aDynArrayConstruct_aDynArrayDestruct, *test_case_aDynArraySize, *test_case_aDynArrayReserve,
          *test_case_aDynArrayShrinkToFit, *test_case_aDynArrayClear, *test_case_aDynArrayInsert,
          *test_case_aDynArrayInsertArray, *test_case_aDynArrayInsertDynArray, *test_case_aDynArrayAdd,
          *test_case_aDynArrayAddArray, *test_case_aDynArrayAddDynArray, *test_case_aDynArraySet,
          *test_case_aDynArrayRemove;

    s = suite_create("Dynamic Array Test Suite");

    test_case_aDynArrayConstruct_aDynArrayDestruct = tcase_create("Dynamic Array Test Case: aDynArrayConstruct / aDynArrayDestruct");
    tcase_add_test(test_case_aDynArrayConstruct_aDynArrayDestruct, test_aDynArrayConstruct_aDynArrayDestruct_valid);
    tcase_add_test(test_case_aDynArrayConstruct_aDynArrayDestruct, test_aDynArrayConstruct_aDynArrayDestruct_withAllocator_valid);
    tcase_add_test(test_case_aDynArrayConstruct_aDynArrayDestruct, test_aDynArrayConstruct_aDynArrayDestruct_noMemoryAvailable);
    tcase_add_test(test_case_aDynArrayConstruct_aDynArrayDestruct, test_aDynArrayConstruct_aDynArrayDestruct_nullptr);
    suite_add_tcase(s, test_case_aDynArrayConstruct_aDynArrayDestruct);

    test_case_aDynArraySize = tcase_create("Dynamic Array Test Case: aDynArraySize");
    tcase_add_test(test_case_aDynArraySize, test_aDynArraySize_valid);
    tcase_add_test(test_case_aDynArraySize, test_aDynArraySize_nullptr);
    suite_add_tcase(s, test_case_aDynArraySize);

    test_case_aDynArrayReserve = tcase_create("Dynamic Array Test Case: aDynArrayReserve");
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_success_enoughCapacityBufferNotNull);
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_success_enoughCapacityBufferNull);
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_success_notEnoughCapacity);
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_failure_biggerThanMaxCapacity);
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_failure_noMemoryAvailable);
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_failure_calculateCapacityNull);
    tcase_add_test(test_case_aDynArrayReserve, test_aDynArrayReserve_failure_nullptr);
    suite_add_tcase(s, test_case_aDynArrayReserve);

    test_case_aDynArrayShrinkToFit = tcase_create("Dynamic Array Test Case: aDynArrayShrinkToFit");
    tcase_add_test(test_case_aDynArrayShrinkToFit, test_aDynArrayShrinkToFit_success_hasLeastCapacityBufferNotNull);
    tcase_add_test(test_case_aDynArrayShrinkToFit, test_aDynArrayShrinkToFit_success_hasLeastCapacityBufferNull);
    tcase_add_test(test_case_aDynArrayShrinkToFit, test_aDynArrayShrinkToFit_success_hasNotLeastCapacity);
    tcase_add_test(test_case_aDynArrayShrinkToFit, test_aDynArrayShrinkToFit_failure_noMemoryAvailable);
    tcase_add_test(test_case_aDynArrayShrinkToFit, test_aDynArrayShrinkToFit_failure_calculateCapacityNull);
    tcase_add_test(test_case_aDynArrayShrinkToFit, test_aDynArrayShrinkToFit_failure_nullptr);
    suite_add_tcase(s, test_case_aDynArrayShrinkToFit);

    test_case_aDynArrayClear = tcase_create("Dynamic Array Test Case: aDynArrayClear");
    tcase_add_test(test_case_aDynArrayClear, test_aDynArrayClear_success_shrinked);
    tcase_add_test(test_case_aDynArrayClear, test_aDynArrayClear_failure_notShrinked);
    tcase_add_test(test_case_aDynArrayClear, test_aDynArrayClear_failure_nullptr);
    suite_add_tcase(s, test_case_aDynArrayClear);

    test_case_aDynArrayInsert = tcase_create("Dynamic Array Test Case: aDynArrayInsert");
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_success_zeroIndex);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_success_middleIndex);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_success_endIndex);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_success_beyondEndIndex);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_success_bufferExpanded);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_success_negativeIndexGetsMaxIndex);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_failure_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArrayInsert, test_aDynArrayInsert_failure_nullptr);
    suite_add_tcase(s, test_case_aDynArrayInsert);

    test_case_aDynArrayInsertArray = tcase_create("Dynamic Array Test Case: aDynArrayInsertArray");
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_zeroIndex);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_middleIndex);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_endIndex);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_beyondEndIndex);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_bufferExpanded);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_nullptrArray);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_zeroArraySize);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_success_negativeIndexGetsMaxIndex);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_failure_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArrayInsertArray, test_aDynArrayInsertArray_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_aDynArrayInsertArray);

    test_case_aDynArrayInsertDynArray = tcase_create("Dynamic Array Test Case: aDynArrayInsertDynArray");
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_zeroIndex);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_middleIndex);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_endIndex);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_beyondEndIndex);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_bufferExpanded);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_nullptrSrcArray);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_zeroSizeSrcArray);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_success_negativeIndexGetsMaxIndex);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_failure_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArrayInsertDynArray, test_aDynArrayInsertDynArray_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_aDynArrayInsertDynArray);

    test_case_aDynArrayAdd = tcase_create("Dynamic Array Test Case: aDynArrayAdd");
    tcase_add_test(test_case_aDynArrayAdd, test_aDynArrayAdd_success_enoughCapacity);
    tcase_add_test(test_case_aDynArrayAdd, test_aDynArrayAdd_success_notEnoughCapacity);
    tcase_add_test(test_case_aDynArrayAdd, test_aDynArrayAdd_failure_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArrayAdd, test_aDynArrayAdd_failure_nullptr);
    suite_add_tcase(s, test_case_aDynArrayAdd);

    test_case_aDynArrayAddArray = tcase_create("Dynamic Array Test Case: aDynArrayAddArray");
    tcase_add_test(test_case_aDynArrayAddArray, test_aDynArrayAddArray_success_enoughCapacity);
    tcase_add_test(test_case_aDynArrayAddArray, test_aDynArrayAddArray_success_notEnoughCapacity);
    tcase_add_test(test_case_aDynArrayAddArray, test_aDynArrayAddArray_success_nullptrArray);
    tcase_add_test(test_case_aDynArrayAddArray, test_aDynArrayAddArray_success_zeroArraySize);
    tcase_add_test(test_case_aDynArrayAddArray, test_aDynArrayAddArray_failure_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArrayAddArray, test_aDynArrayAddArray_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_aDynArrayAddArray);

    test_case_aDynArrayAddDynArray = tcase_create("Dynamic Array Test Case: aDynArrayAddDynArray");
    tcase_add_test(test_case_aDynArrayAddDynArray, test_aDynArrayAddDynArray_success_enoughCapacity);
    tcase_add_test(test_case_aDynArrayAddDynArray, test_aDynArrayAddDynArray_success_notEnoughCapacity);
    tcase_add_test(test_case_aDynArrayAddDynArray, test_aDynArrayAddDynArray_success_nullptrSrcArray);
    tcase_add_test(test_case_aDynArrayAddDynArray, test_aDynArrayAddDynArray_success_zeroSizeSrcArray);
    tcase_add_test(test_case_aDynArrayAddDynArray, test_aDynArrayAddDynArray_failure_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArrayAddDynArray, test_aDynArrayAddDynArray_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_aDynArrayAddDynArray);

    test_case_aDynArraySet = tcase_create("Dynamic Array Test Case: aDynArraySet");
    tcase_add_test(test_case_aDynArraySet, test_aDynArraySet_success_indexInBounds);
    tcase_add_test(test_case_aDynArraySet, test_aDynArraySet_success_indexBeyondSize);
    tcase_add_test(test_case_aDynArraySet, test_aDynArraySet_success_indexBeyondSize_bufferExpanded);
    tcase_add_test(test_case_aDynArraySet, test_aDynArraySet_failure_indexBeyondSize_bufferExpansionFailed);
    tcase_add_test(test_case_aDynArraySet, test_aDynArraySet_failure_nullptr);
    suite_add_tcase(s, test_case_aDynArraySet);

    test_case_aDynArrayRemove = tcase_create("Dynamic Array Test Case: aDynArrayRemove");
    tcase_add_test(test_case_aDynArrayRemove, test_aDynArrayRemove_indexRangeInBounds);
    tcase_add_test(test_case_aDynArrayRemove, test_aDynArrayRemove_rangeBeyondBounds);
    tcase_add_test(test_case_aDynArrayRemove, test_aDynArrayRemove_zeroRange);
    tcase_add_test(test_case_aDynArrayRemove, test_aDynArrayRemove_indexBeyoundBounds);
    tcase_add_test(test_case_aDynArrayRemove, test_aDynArrayRemove_nullptr);
    suite_add_tcase(s, test_case_aDynArrayRemove);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = test_suite_dynarray();
    sr = srunner_create(s);

    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}