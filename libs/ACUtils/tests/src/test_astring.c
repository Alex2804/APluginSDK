#include "../include/ACUtilsTest/acheck.h"

#include <stdlib.h>

#include "ACUtils/astring.h"

static size_t private_ACUtilsTest_AString_reallocCount = 0;
static size_t private_ACUtilsTest_AString_reallocFailCounter = 0;
static bool private_ACUtilsTest_AString_reallocFail = false;
static size_t private_ACUtilsTest_AString_freeCount = 0;

static void* private_ACUtilsTest_AString_realloc(void* ptr, size_t size) {
    if(!private_ACUtilsTest_AString_reallocFail || private_ACUtilsTest_AString_reallocFailCounter > 0) {
        if(private_ACUtilsTest_AString_reallocFail)
            --private_ACUtilsTest_AString_reallocFailCounter;
        void* tmp = realloc(ptr, size);
        if(tmp != nullptr)
            ++private_ACUtilsTest_AString_reallocCount;
        return tmp;
    }
    return nullptr;
}
static void private_ACUtilsTest_AString_free(void* ptr) {
    if(ptr != nullptr)
        ++private_ACUtilsTest_AString_freeCount;
    free(ptr);
}

static void private_ACUtilsTest_AString_setReallocFail(bool reallocFail, size_t failCounter)
{
    if(reallocFail) {
        private_ACUtilsTest_AString_reallocFailCounter = failCounter;
        private_ACUtilsTest_AString_reallocFail = true;
    } else {
        private_ACUtilsTest_AString_reallocFail = false;
    }
}

#ifndef ACUTILS_ONE_SOURCE
struct AString
{
    const ACUtilsReallocator reallocator;
    const ACUtilsDeallocator deallocator;
    size_t size;
    size_t capacity;
    char *buffer;
};
#endif

static struct AString private_ACUtilsTest_AString_constructTestString(const char *initBuffer, size_t capacity)
{
    bool tmp = private_ACUtilsTest_AString_reallocFail;
    struct AString string = {private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free};
    string.size = strlen(initBuffer);
    string.capacity = capacity;
    private_ACUtilsTest_AString_reallocFail = false;
    string.buffer = (char*) string.reallocator(nullptr, string.capacity + 1);
    private_ACUtilsTest_AString_reallocFail = tmp;
    private_ACUtilsTest_AString_reallocCount = 0;
    private_ACUtilsTest_AString_freeCount = 0;
    memcpy(string.buffer, initBuffer, string.size + 1); /* +1 for '\0' */
    return string;
}
static void private_ACUtilsTest_AString_destructTestString(struct AString string)
{
    string.deallocator(string.buffer);
}
#define ACUTILSTEST_ASTRING_CHECK_ASTRING(string_, buffer_, capacity_) do \
    { \
        ACUTILS_ASSERT_UINT_EQ((string_).capacity, (capacity_)); \
        ACUTILS_ASSERT_PTR_NONNULL((string_).buffer); \
        ACUTILS_ASSERT_STR_EQ((string_).buffer, (buffer_)); \
        ACUTILS_ASSERT_UINT_EQ((string_).size, strlen(buffer_)); \
    } while(0)
#define ACUTILSTEST_ASTRING_CHECK_REALLOC(reallocCount_) do \
    { \
        ACUTILS_ASSERT_UINT_EQ(private_ACUtilsTest_AString_reallocCount, (reallocCount_)); \
    } while(0)

START_TEST(test_AString_construct_destruct_valid)
{
    struct AString *string = AString_construct();
    ACUTILS_ASSERT_PTR_NONNULL(string);
    ACUTILS_ASSERT_PTR_NONNULL(string->reallocator);
    ACUTILS_ASSERT_PTR_NONNULL(string->deallocator);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*string, "", 8);
    AString_destruct(string);
}
END_TEST
START_TEST(test_AString_construct_destruct_withAllocator_valid)
{
    struct AString *string;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    string = AString_constructWithAllocator(private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NONNULL(string);
    ACUTILS_ASSERT_PTR_EQ(string->reallocator, private_ACUtilsTest_AString_realloc);
    ACUTILS_ASSERT_PTR_EQ(string->deallocator, private_ACUtilsTest_AString_free);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*string, "", 8);
    AString_destruct(string);
    ACUTILS_ASSERT_UINT_EQ(private_ACUtilsTest_AString_reallocCount, private_ACUtilsTest_AString_freeCount);
}
END_TEST
START_TEST(test_AString_construct_destruct_withCapacityAndAllocator_valid)
{
    struct AString *string;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    string = AString_constructWithCapacityAndAllocator(666, private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NONNULL(string);
    ACUTILS_ASSERT_PTR_EQ(string->reallocator, private_ACUtilsTest_AString_realloc);
    ACUTILS_ASSERT_PTR_EQ(string->deallocator, private_ACUtilsTest_AString_free);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*string, "", 666);
    AString_destruct(string);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
}
END_TEST
START_TEST(test_AString_construct_destruct_withAllocator_invalid)
{
    struct AString *string;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    string = AString_constructWithAllocator(nullptr, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NULL(string);
    string = AString_constructWithAllocator(private_ACUtilsTest_AString_realloc, nullptr);
    ACUTILS_ASSERT_PTR_NULL(string);
}
END_TEST
START_TEST(test_AString_construct_destruct_withCapacityAndAllocator_invalid)
{
    struct AString *string;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    string = AString_constructWithCapacityAndAllocator(666, nullptr, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NULL(string);
    string = AString_constructWithCapacityAndAllocator(666, private_ACUtilsTest_AString_realloc, nullptr);
    ACUTILS_ASSERT_PTR_NULL(string);
}
END_TEST
START_TEST(test_AString_construct_destruct_withAllocator_noMemoryAvailable)
{
    struct AString *string;
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    string = AString_constructWithAllocator(private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NULL(string);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_setReallocFail(true, 1);
    string = AString_constructWithAllocator(private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NULL(string);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
}
END_TEST
START_TEST(test_AString_construct_destruct_withCapacityAndAllocator_noMemoryAvailable)
{
    struct AString *string;
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    string = AString_constructWithCapacityAndAllocator(666, private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NULL(string);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_setReallocFail(true, 1);
    string = AString_constructWithCapacityAndAllocator(666, private_ACUtilsTest_AString_realloc, private_ACUtilsTest_AString_free);
    ACUTILS_ASSERT_PTR_NULL(string);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
}
END_TEST
START_TEST(test_AString_construct_destruct_nullptr)
{
    private_ACUtilsTest_AString_reallocCount = private_ACUtilsTest_AString_freeCount = 0;
    AString_destruct(nullptr);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
}
END_TEST


START_TEST(test_AString_reallocator_valid)
{
    struct AString string0 = {nullptr, nullptr};
    ACUTILS_ASSERT_PTR_NULL(AString_reallocator(&string0));
    struct AString string1 = {realloc, nullptr};
    ACUTILS_ASSERT_PTR_EQ(AString_reallocator(&string1), realloc);
    struct AString string2 = {private_ACUtilsTest_AString_realloc, nullptr};
    ACUTILS_ASSERT_PTR_EQ(AString_reallocator(&string2), private_ACUtilsTest_AString_realloc);
}
START_TEST(test_AString_reallocator_nullptr)
{
    ACUTILS_ASSERT_PTR_NULL(AString_reallocator(nullptr));
}


START_TEST(test_AString_deallocator_valid)
{
    struct AString string0 = {nullptr, nullptr};
    ACUTILS_ASSERT_PTR_NULL(AString_deallocator(&string0));
    struct AString string1 = {nullptr, free};
    ACUTILS_ASSERT_PTR_EQ(AString_deallocator(&string1), free);
    struct AString string2 = {nullptr, private_ACUtilsTest_AString_free};
    ACUTILS_ASSERT_PTR_EQ(AString_deallocator(&string2), private_ACUtilsTest_AString_free);
}
START_TEST(test_AString_deallocator_nullptr)
{
    ACUTILS_ASSERT_PTR_NULL(AString_deallocator(nullptr));
}


START_TEST(test_AString_size_valid)
{
    struct AString string = {nullptr, nullptr};
    string.size = 42;
    ACUTILS_ASSERT_UINT_EQ(AString_size(&string), 42);
    string.size = 13;
    ACUTILS_ASSERT_UINT_EQ(AString_size(&string), 13);
    string.size = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_size(&string), 0);
}
START_TEST(test_AString_size_nullptr)
{
    ACUTILS_ASSERT_UINT_EQ(AString_size(nullptr), 0);
}


START_TEST(test_AString_capacity_valid)
{
    struct AString string = {nullptr, nullptr};
    string.capacity = 42;
    ACUTILS_ASSERT_UINT_EQ(AString_capacity(&string), 42);
    string.capacity = 13;
    ACUTILS_ASSERT_UINT_EQ(AString_capacity(&string), 13);
    string.capacity = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_capacity(&string), 0);
}
START_TEST(test_AString_capacity_nullptr)
{
    ACUTILS_ASSERT_UINT_EQ(AString_capacity(nullptr), 0);
}


START_TEST(test_AString_buffer_valid)
{
    struct AString string = {nullptr, nullptr};
    string.buffer = (char*) 42;
    ACUTILS_ASSERT_PTR_EQ(AString_buffer(&string), (char*) 42);
    string.buffer = (char*) 13;
    ACUTILS_ASSERT_PTR_EQ(AString_buffer(&string), (char*) 13);
    string.buffer = nullptr;
    ACUTILS_ASSERT_PTR_NULL(AString_buffer(&string));
}
START_TEST(test_AString_buffer_nullptr)
{
    ACUTILS_ASSERT_PTR_NULL(AString_buffer(nullptr));
}


START_TEST(test_AString_reserve_success_enoughCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(&string, 0), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(&string, 1), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(&string, 16), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
START_TEST(test_AString_reserve_success_notEnoughCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(&string, 9), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_reserve_success_notEnoughCapacity_overMaxAlloc)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(&string, 8193), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 9217);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_reserve_failure_noMemoryAvailable)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(&string, 9), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_reserve_failure_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_reserve(nullptr, 42), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_shrinkToFit_success_hasLeastCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456789", 10);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_shrinkToFit(&string), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456789", 10);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
START_TEST(test_AString_shrinkToFit_success_smallerThanMinSize)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_shrinkToFit(&string), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    ACUTILS_ASSERT_UINT_EQ(AString_shrinkToFit(&string), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
START_TEST(test_AString_shrinkToFit_success_hasNotLeastCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456789", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_shrinkToFit(&string), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456789", 10);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_shrinkToFit_failure_noMemoryAvailable)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456789", 16);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_shrinkToFit(&string), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456789", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_shrinkToFit_failure_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_shrinkToFit(nullptr), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_clear)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456789", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_clear(&string);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_clear_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    AString_clear(nullptr);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_remove_indexRangeInBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456789", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_remove(&string, 4, 5);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01239", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_remove_rangeBeyondBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456789", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_remove(&string, 5, 100);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    AString_remove(&string, 2, -1);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_remove_zeroRange)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_remove(&string, 2, 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    AString_remove(&string, 0, 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    AString_remove(&string, 8, 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    AString_remove(&string, 666, 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_remove_indexBeyoundBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    AString_remove(&string, 8, 1);
    AString_remove(&string, 666, 42);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_remove_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    AString_remove(nullptr, 5, 10);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_trim_trimmed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trim(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trim_frontTrimming)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("       0", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trim(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trim_backTrimming)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0       ", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trim(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trim_frontAndBackTrimming)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("   01   ", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trim(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trim_completeString)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("        ", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trim(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trim_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    AString_trim(nullptr, ' ');
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_trimFront_trimmed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trimFront(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trimFront_trimming)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("       0", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trimFront(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trimFront_completeString)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("        ", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trimFront(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trimFront_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    AString_trimFront(nullptr, ' ');
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_trimBack_trimmed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trimBack(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trimBack_trimming)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0       ", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trimBack(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trimBack_completeString)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("        ", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    AString_trimBack(&string, ' ');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_trimBack_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    AString_trimBack(nullptr, ' ');
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_insert_success_zeroIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("1234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, 0, '0'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insert_success_middleIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0134567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, 2, '2'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insert_success_beyondEndIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012345", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, string.size, '6'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, 666, '7'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insert_success_nullTerminator)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("1234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, 3, '\0'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "123", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insert_success_bufferExpanded)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01345678", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, 2, '2'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012345678", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insert_failure_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01345678", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insert(&string, 2, '2'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insert_failure_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_insert(nullptr, 0, '0'), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_insertCString_success_zeroIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("34567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 0, "012", 3), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertCString_success_middleIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 2, "234", 3), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertCString_success_endIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 5, "567", 3), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertCString_success_beyondEndIndex)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 666, "567", 3), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertArray_success_bufferExpanded)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012789", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 3, "3456", 4), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456789", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertArray_success_nullptrArray)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 2, nullptr, 3), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertArray_success_zeroArraySize)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 2, "xyz", 0), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertArray_failure_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("015678", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(&string, 2, "234", 3), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "015678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_insertArray_failure_nullptrDestArray)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(nullptr, 0, "012", 3), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_insertAString_success_zeroIndex)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("34567", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 0, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_success_middleIndex)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01567", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 2, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_success_endIndex)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 5, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_success_beyondEndIndex)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 666, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_success_bufferExpanded)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01678", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("2345", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 2, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "012345678", 16);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "2345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_success_nullptrSrcArray)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 2, nullptr), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
}
END_TEST
START_TEST(test_AString_insertAString_success_zeroSizeSrcArray)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01567", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 2, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_failure_bufferExpansionFailed)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01678", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("2345", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(&destString, 2, &srcString), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01678", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "2345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_insertAString_failure_nullptrDestArray)
{
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_insertAString(nullptr, 0, &srcString), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST


START_TEST(test_AString_append_success_enoughCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_append(&string, '5'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_append_success_nullTerminator)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_append(&string, '\0'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_append_success_notEnoughCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_append(&string, '8'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012345678", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_append_failure_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_append(&string, '8'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_append_failure_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_append(nullptr, '0'), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_appendCString_success_enoughCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendCString(&string, "567", 3), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_appendCString_success_notEnoughCapacity)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendCString(&string, "5678", 4), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012345678", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_appendCString_success_nullptrArray)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendCString(&string, nullptr, 4), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_appendCString_success_zeroArraySize)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendCString(&string, "567", 0), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_appendCString_failure_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendCString(&string, "5678", 4), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_appendCString_failure_nullptrDestArray)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_insertCString(nullptr, 0, "012", 3), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_appendAString_success_enoughCapacity)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendAString(&destString, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_appendAString_success_notEnoughCapacity)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("5678", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendAString(&destString, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "012345678", 16);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "5678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_appendAString_success_nullptrSrcArray)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendAString(&destString, nullptr), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
}
END_TEST
START_TEST(test_AString_appendAString_success_zeroSizeSrcArray)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendAString(&destString, &srcString), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_appendAString_failure_bufferExpansionFailed)
{
    struct AString destString = private_ACUtilsTest_AString_constructTestString("01234", 8);
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("5678", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendAString(&destString, &srcString), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(destString, "01234", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "5678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(destString);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST
START_TEST(test_AString_appendAString_failure_nullptrDestArray)
{
    struct AString srcString = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_appendAString(nullptr, &srcString), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(srcString, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(srcString);
}
END_TEST


START_TEST(test_AString_get_indexInBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("12345678", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 0), '1');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 1), '2');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 2), '3');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 3), '4');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 4), '5');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 5), '6');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 6), '7');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 7), '8');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "12345678", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_get_indexOutOfBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 16);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 8), '\0');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 15), '\0');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_get(&string, 666), '\0');
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_get_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_get(nullptr, 0), '\0');
    ACUTILS_ASSERT_UINT_EQ(AString_get(nullptr, 666), '\0');
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_set_success_indexInBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_set(&string, 0, 'x'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "x1234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_set(&string, 1, 'y'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xy234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_set(&string, 2, 'z'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xyz34567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_set_success_indexBeyondSize)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_set(&string, 666, '7'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_set_success_indexBeyondSize_bufferExpanded)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_set(&string, 666, '8'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012345678", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_set_failure_indexBeyondSize_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_set(&string, 666, '8'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_set_failure_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_set(nullptr, 0, '0'), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_setRange_success_indexAndRangeInBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 0, 2, 'x'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xx234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 1, 2, 'y'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xyy34567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 2, 3, 'z'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xyzzz567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_success_indexInBoundsRangeBeyondSize)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 2, 2, 'x'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01xx", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 2, 4, 'y'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01yyyy", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_success_indexInBoundsRangeBeyondSize_bufferExpanded)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 2, 7, 'x'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01xxxxxxx", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 6, 13, 'y'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01xxxxyyyyyyyyyyyyy", 32);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(2);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_success_indexAndRangeBeyondSize)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 3, 2, 'x'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012xx", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, -1, 3, 'y'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012xxyyy", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_success_indexAndRangeBeyondSize_bufferExpanded)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 3, 6, 'x'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012xxxxxx", 16);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(1);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 666, 8, 'y'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012xxxxxxyyyyyyyy", 32);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(2);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, -1, 16, 'z'), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012xxxxxxyyyyyyyyzzzzzzzzzzzzzzzz", 64);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(3);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_failure_indexInBoundsRangeBeyondSize_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 2, 7, 'x'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 6, 12, 'y'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_failure_indexAndRangeBeyondSize_bufferExpansionFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("012", 8);
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 3, 6, 'x'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(&string, 666, 8, 'y'), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "012", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_setRange_failure_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_UINT_EQ(AString_setRange(nullptr, 0, 0, '0'), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_equals_valid)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("012345", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("012345", 8);
    struct AString string3 = private_ACUtilsTest_AString_constructTestString("543210", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(&string1, &string2), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(&string2, &string1), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(&string1, &string3), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "543210", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(&string2, &string3), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "543210", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
    private_ACUtilsTest_AString_destructTestString(string3);
}
END_TEST
START_TEST(test_AString_equals_nullptr)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(&string, nullptr), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(nullptr, &string), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equals(nullptr, nullptr), true);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST


START_TEST(test_AString_equalsCString_valid)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("012345", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("543210", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(&string1, "012345"), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(&string1, "543210"), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(&string2, "543210"), true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "543210", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(&string2, "012345"), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "543210", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
}
END_TEST
START_TEST(test_AString_equalsCString_nullptr)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(&string, nullptr), false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(nullptr, ""), false);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_UINT_EQ(AString_equalsCString(nullptr, nullptr), true);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST


START_TEST(test_AString_compare_equals)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("012345", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("012345", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_EQ(AString_compare(&string1, &string2), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
}
END_TEST
START_TEST(test_AString_compare_firstLessThanSecond)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("a", 8);
    struct AString string3 = private_ACUtilsTest_AString_constructTestString("b", 8);
    struct AString string4 = private_ACUtilsTest_AString_constructTestString("ba", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_LT(AString_compare(&string1, &string2), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compare(&string2, &string3), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "b", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compare(&string2, &string4), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string4, "ba", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compare(&string3, &string4), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "b", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string4, "ba", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
    private_ACUtilsTest_AString_destructTestString(string3);
    private_ACUtilsTest_AString_destructTestString(string4);
}
END_TEST
START_TEST(test_AString_compare_firstGreaterThanSecond)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("a", 8);
    struct AString string3 = private_ACUtilsTest_AString_constructTestString("b", 8);
    struct AString string4 = private_ACUtilsTest_AString_constructTestString("ba", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_GT(AString_compare(&string2, &string1), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_GT(AString_compare(&string3, &string2), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "b", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_GT(AString_compare(&string4, &string2), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string4, "ba", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_GT(AString_compare(&string4, &string3), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "b", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string4, "ba", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
    private_ACUtilsTest_AString_destructTestString(string3);
    private_ACUtilsTest_AString_destructTestString(string4);
}
END_TEST
START_TEST(test_AString_compare_nullptr)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_GT(AString_compare(&string, nullptr), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compare(nullptr, &string), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_EQ(AString_compare(nullptr, nullptr), 0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST


START_TEST(test_AString_compareCString_equals)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("012345", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_EQ(AString_compareCString(&string1, "012345"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "012345", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
}
END_TEST
START_TEST(test_AString_compareCString_firstLessThanSecond)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("a", 8);
    struct AString string3 = private_ACUtilsTest_AString_constructTestString("b", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_LT(AString_compareCString(&string1, "a"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compareCString(&string2, "b"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compareCString(&string2, "ba"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compareCString(&string3, "ba"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "b", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
    private_ACUtilsTest_AString_destructTestString(string3);
}
END_TEST
START_TEST(test_AString_compareCString_firstGreaterThanSecond)
{
    struct AString string1 = private_ACUtilsTest_AString_constructTestString("a", 8);
    struct AString string2 = private_ACUtilsTest_AString_constructTestString("b", 8);
    struct AString string3 = private_ACUtilsTest_AString_constructTestString("ba", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_GT(AString_compareCString(&string1, ""), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string1, "a", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_GT(AString_compareCString(&string2, "a"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string2, "b", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_GT(AString_compareCString(&string3, "a"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "ba", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_GT(AString_compareCString(&string3, "b"), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string3, "ba", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    private_ACUtilsTest_AString_destructTestString(string1);
    private_ACUtilsTest_AString_destructTestString(string2);
    private_ACUtilsTest_AString_destructTestString(string3);
}
END_TEST
START_TEST(test_AString_compareCString_nullptr)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("", 8);
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    ACUTILS_ASSERT_INT_GT(AString_compareCString(&string, nullptr), 0);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_LT(AString_compareCString(nullptr, ""), 0);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
    ACUTILS_ASSERT_INT_EQ(AString_compareCString(nullptr, nullptr), 0);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST


START_TEST(test_AString_clone_valid)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 666);
    struct AString *cloned;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    cloned = AString_clone(&string);
    ACUTILS_ASSERT_PTR_NONNULL(cloned);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*cloned, "01234567", 666);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 666);
    AString_destruct(cloned);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_clone_noMemoryAvailable)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 666);
    struct AString *cloned;
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    cloned = AString_clone(&string);
    ACUTILS_ASSERT_PTR_NULL(cloned);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 666);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_clone_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_PTR_NULL(AString_clone(nullptr));
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_substring_indexRangeInBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("abcd01234567wxyz", 16);
    struct AString *substring;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    substring = AString_substring(&string, 0, 16);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "abcd01234567wxyz", 16);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, 0, 8);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "abcd0123", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, 4, 8);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, 8, 8);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "4567wxyz", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_substring_rangeBeyondBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("abcd01234567wxyz", 16);
    struct AString *substring;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    substring = AString_substring(&string, 0, 17);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "abcd01234567wxyz", 16);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, 0, -1);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "abcd01234567wxyz", 16);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, 8, -1);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "4567wxyz", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, 15, 666);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "z", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "abcd01234567wxyz", 16);
    AString_destruct(substring);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_substring_indexBeyondBounds)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("xxxx01234567xxxx", 16);
    struct AString *substring;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    substring = AString_substring(&string, 16, -1);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xxxx01234567xxxx", 16);
    AString_destruct(substring);
    substring = AString_substring(&string, -1, 4);
    ACUTILS_ASSERT_PTR_NONNULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*substring, "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xxxx01234567xxxx", 16);
    AString_destruct(substring);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_substring_reallocationFailed)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("xxxx01234567xxxx", 16);
    struct AString *substring;
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    substring = AString_substring(&string, 0, -1);
    ACUTILS_ASSERT_PTR_NULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xxxx01234567xxxx", 16);
    private_ACUtilsTest_AString_setReallocFail(true, 1);
    substring = AString_substring(&string, 0, -1);
    ACUTILS_ASSERT_PTR_NULL(substring);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "xxxx01234567xxxx", 16);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_substring_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_PTR_NULL(AString_substring(nullptr, 0, -1));
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST


START_TEST(test_AString_split_emptyString)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILS_ASSERT_PTR_NONNULL(splitted);
    ACUTILS_ASSERT_UINT_EQ(ADynArray_size(splitted), 1);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 0), "", 8);
    AString_freeSplitted(splitted);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_noDelimiter)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("01234567", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILS_ASSERT_PTR_NONNULL(splitted);
    ACUTILS_ASSERT_UINT_EQ(ADynArray_size(splitted), 1);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 0), "01234567", 8);
    AString_freeSplitted(splitted);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "01234567", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_firstCharDelimiter)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString(";0123456", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";0123456", 8);
    ACUTILS_ASSERT_PTR_NONNULL(splitted);
    ACUTILS_ASSERT_UINT_EQ(ADynArray_size(splitted), 2);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 0), "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 1), "0123456", 8);
    AString_freeSplitted(splitted);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";0123456", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_lastCharDelimiter)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString("0123456;", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456;", 8);
    ACUTILS_ASSERT_PTR_NONNULL(splitted);
    ACUTILS_ASSERT_UINT_EQ(ADynArray_size(splitted), 2);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 0), "0123456", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 1), "", 8);
    AString_freeSplitted(splitted);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, "0123456;", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_multipleDelimiters)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString(";01;;23;", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";01;;23;", 8);
    ACUTILS_ASSERT_PTR_NONNULL(splitted);
    ACUTILS_ASSERT_UINT_EQ(ADynArray_size(splitted), 5);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 0), "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 1), "01", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 2), "", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 3), "23", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 4), "", 8);
    AString_freeSplitted(splitted);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";01;;23;", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_multipleDelimiters_discardEmpty)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString(";01;;23;", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    splitted = AString_split(&string, ';', true);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";01;;23;", 8);
    ACUTILS_ASSERT_PTR_NONNULL(splitted);
    ACUTILS_ASSERT_UINT_EQ(ADynArray_size(splitted), 2);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 0), "01", 8);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(*ADynArray_get(splitted, 1), "23", 8);
    AString_freeSplitted(splitted);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";01;;23;", 8);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_noMemoryAvailable)
{
    struct AString string = private_ACUtilsTest_AString_constructTestString(";01;;23;", 8);
    struct ASplittedString *splitted;
    private_ACUtilsTest_AString_setReallocFail(true, 0);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";01;;23;", 8);
    ACUTILS_ASSERT_PTR_NULL(splitted);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_setReallocFail(true, 2);
    splitted = AString_split(&string, ';', false);
    ACUTILSTEST_ASTRING_CHECK_ASTRING(string, ";01;;23;", 8);
    ACUTILS_ASSERT_PTR_NULL(splitted);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(private_ACUtilsTest_AString_freeCount);
    private_ACUtilsTest_AString_destructTestString(string);
}
END_TEST
START_TEST(test_AString_split_nullptr)
{
    private_ACUtilsTest_AString_setReallocFail(false, 0);
    private_ACUtilsTest_AString_reallocCount = 0;
    ACUTILS_ASSERT_PTR_NULL(AString_split(nullptr, ' ', false));
    AString_freeSplitted(nullptr);
    ACUTILSTEST_ASTRING_CHECK_REALLOC(0);
}
END_TEST



ACUTILS_EXTERN_C Suite* private_ACUtilsTest_AString_getTestSuite(void)
{
    Suite *s;
    TCase *test_case_AString_construct_destruct, *test_case_AString_reallocator, *test_case_AString_deallocator,
          *test_case_AString_size, *test_case_AString_capacity, *test_case_AString_buffer, *test_case_AString_reserve,
          *test_case_AString_shrinkToFit, *test_case_AString_clear, *test_case_AString_remove, *test_case_AString_trim,
          *test_case_AString_trimFront, *test_case_AString_trimBack, *test_case_AString_insert,
          *test_case_AString_insertCString, *test_case_AString_insertAString, *test_case_AString_append,
          *test_case_AString_appendCString, *test_case_AString_appendAString, *test_case_AString_get,
          *test_case_AString_set, *test_case_AString_setRange, *test_case_AString_equals,
          *test_case_AString_equalsCString, *test_case_AString_compare, *test_case_AString_compareCString,
          *test_case_AString_clone, *test_case_AString_substring, *test_case_AString_split;

    s = suite_create("AString Test Suite");

    test_case_AString_construct_destruct = tcase_create("AString Test Case: AString_construct / AString_destruct");
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_valid);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_withAllocator_valid);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_withCapacityAndAllocator_valid);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_withAllocator_invalid);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_withCapacityAndAllocator_invalid);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_withAllocator_noMemoryAvailable);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_withCapacityAndAllocator_noMemoryAvailable);
    tcase_add_test(test_case_AString_construct_destruct, test_AString_construct_destruct_nullptr);
    suite_add_tcase(s, test_case_AString_construct_destruct);

    test_case_AString_reallocator = tcase_create("AString Test Case: AString_reallocator");
    tcase_add_test(test_case_AString_reallocator, test_AString_reallocator_valid);
    tcase_add_test(test_case_AString_reallocator, test_AString_reallocator_nullptr);
    suite_add_tcase(s, test_case_AString_reallocator);

    test_case_AString_deallocator = tcase_create("AString Test Case: AString_deallocator");
    tcase_add_test(test_case_AString_deallocator, test_AString_deallocator_valid);
    tcase_add_test(test_case_AString_deallocator, test_AString_deallocator_nullptr);
    suite_add_tcase(s, test_case_AString_deallocator);

    test_case_AString_size = tcase_create("AString Test Case: AString_size");
    tcase_add_test(test_case_AString_size, test_AString_size_valid);
    tcase_add_test(test_case_AString_size, test_AString_size_nullptr);
    suite_add_tcase(s, test_case_AString_size);

    test_case_AString_capacity = tcase_create("AString Test Case: AString_capacity");
    tcase_add_test(test_case_AString_capacity, test_AString_capacity_valid);
    tcase_add_test(test_case_AString_capacity, test_AString_capacity_nullptr);
    suite_add_tcase(s, test_case_AString_capacity);

    test_case_AString_buffer = tcase_create("AString Test Case: AString_buffer");
    tcase_add_test(test_case_AString_buffer, test_AString_buffer_valid);
    tcase_add_test(test_case_AString_buffer, test_AString_buffer_nullptr);
    suite_add_tcase(s, test_case_AString_buffer);

    test_case_AString_reserve = tcase_create("AString Test Case: AString_reserve");
    tcase_add_test(test_case_AString_reserve, test_AString_reserve_success_enoughCapacity);
    tcase_add_test(test_case_AString_reserve, test_AString_reserve_success_notEnoughCapacity);
    tcase_add_test(test_case_AString_reserve, test_AString_reserve_success_notEnoughCapacity_overMaxAlloc);
    tcase_add_test(test_case_AString_reserve, test_AString_reserve_failure_noMemoryAvailable);
    tcase_add_test(test_case_AString_reserve, test_AString_reserve_failure_nullptr);
    suite_add_tcase(s, test_case_AString_reserve);

    test_case_AString_shrinkToFit = tcase_create("AString Test Case: AString_shrinkToFit");
    tcase_add_test(test_case_AString_shrinkToFit, test_AString_shrinkToFit_success_hasLeastCapacity);
    tcase_add_test(test_case_AString_shrinkToFit, test_AString_shrinkToFit_success_smallerThanMinSize);
    tcase_add_test(test_case_AString_shrinkToFit, test_AString_shrinkToFit_success_hasNotLeastCapacity);
    tcase_add_test(test_case_AString_shrinkToFit, test_AString_shrinkToFit_failure_noMemoryAvailable);
    tcase_add_test(test_case_AString_shrinkToFit, test_AString_shrinkToFit_failure_nullptr);
    suite_add_tcase(s, test_case_AString_shrinkToFit);

    test_case_AString_clear = tcase_create("AString Test Case: AString_clear");
    tcase_add_test(test_case_AString_clear, test_AString_clear);
    tcase_add_test(test_case_AString_clear, test_AString_clear_nullptr);
    suite_add_tcase(s, test_case_AString_clear);

    test_case_AString_remove = tcase_create("AString Test Case: AString_remove");
    tcase_add_test(test_case_AString_remove, test_AString_remove_indexRangeInBounds);
    tcase_add_test(test_case_AString_remove, test_AString_remove_rangeBeyondBounds);
    tcase_add_test(test_case_AString_remove, test_AString_remove_zeroRange);
    tcase_add_test(test_case_AString_remove, test_AString_remove_indexBeyoundBounds);
    tcase_add_test(test_case_AString_remove, test_AString_remove_nullptr);
    suite_add_tcase(s, test_case_AString_remove);

    test_case_AString_trim = tcase_create("AString Test Case: AString_trim");
    tcase_add_test(test_case_AString_trim, test_AString_trim_trimmed);
    tcase_add_test(test_case_AString_trim, test_AString_trim_frontTrimming);
    tcase_add_test(test_case_AString_trim, test_AString_trim_backTrimming);
    tcase_add_test(test_case_AString_trim, test_AString_trim_frontAndBackTrimming);
    tcase_add_test(test_case_AString_trim, test_AString_trim_completeString);
    tcase_add_test(test_case_AString_trim, test_AString_trim_nullptr);
    suite_add_tcase(s, test_case_AString_trim);

    test_case_AString_trimFront = tcase_create("AString Test Case: AString_trimFront");
    tcase_add_test(test_case_AString_trimFront, test_AString_trimFront_trimmed);
    tcase_add_test(test_case_AString_trimFront, test_AString_trimFront_trimming);
    tcase_add_test(test_case_AString_trimFront, test_AString_trimFront_completeString);
    tcase_add_test(test_case_AString_trimFront, test_AString_trimFront_nullptr);
    suite_add_tcase(s, test_case_AString_trimFront);

    test_case_AString_trimBack = tcase_create("AString Test Case: AString_trimBack");
    tcase_add_test(test_case_AString_trimBack, test_AString_trimBack_trimmed);
    tcase_add_test(test_case_AString_trimBack, test_AString_trimBack_trimming);
    tcase_add_test(test_case_AString_trimBack, test_AString_trimBack_completeString);
    tcase_add_test(test_case_AString_trimBack, test_AString_trimBack_nullptr);
    suite_add_tcase(s, test_case_AString_trimBack);

    test_case_AString_insert = tcase_create("AString Test Case: AString_insert");
    tcase_add_test(test_case_AString_insert, test_AString_insert_success_zeroIndex);
    tcase_add_test(test_case_AString_insert, test_AString_insert_success_middleIndex);
    tcase_add_test(test_case_AString_insert, test_AString_insert_success_beyondEndIndex);
    tcase_add_test(test_case_AString_insert, test_AString_insert_success_nullTerminator);
    tcase_add_test(test_case_AString_insert, test_AString_insert_success_bufferExpanded);
    tcase_add_test(test_case_AString_insert, test_AString_insert_failure_bufferExpansionFailed);
    tcase_add_test(test_case_AString_insert, test_AString_insert_failure_nullptr);
    suite_add_tcase(s, test_case_AString_insert);

    test_case_AString_insertCString = tcase_create("AString Test Case: AString_insertCString");
    tcase_add_test(test_case_AString_insertCString, test_AString_insertCString_success_zeroIndex);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertCString_success_middleIndex);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertCString_success_endIndex);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertCString_success_beyondEndIndex);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertArray_success_bufferExpanded);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertArray_success_nullptrArray);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertArray_success_zeroArraySize);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertArray_failure_bufferExpansionFailed);
    tcase_add_test(test_case_AString_insertCString, test_AString_insertArray_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_AString_insertCString);

    test_case_AString_insertAString = tcase_create("AString Test Case: AString_insertAString");
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_zeroIndex);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_middleIndex);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_endIndex);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_beyondEndIndex);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_bufferExpanded);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_nullptrSrcArray);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_success_zeroSizeSrcArray);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_failure_bufferExpansionFailed);
    tcase_add_test(test_case_AString_insertAString, test_AString_insertAString_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_AString_insertAString);

    test_case_AString_append = tcase_create("AString Test Case: AString_append");
    tcase_add_test(test_case_AString_append, test_AString_append_success_enoughCapacity);
    tcase_add_test(test_case_AString_append, test_AString_append_success_nullTerminator);
    tcase_add_test(test_case_AString_append, test_AString_append_success_notEnoughCapacity);
    tcase_add_test(test_case_AString_append, test_AString_append_failure_bufferExpansionFailed);
    tcase_add_test(test_case_AString_append, test_AString_append_failure_nullptr);
    suite_add_tcase(s, test_case_AString_append);

    test_case_AString_appendCString = tcase_create("AString Test Case: AString_appendCString");
    tcase_add_test(test_case_AString_appendCString, test_AString_appendCString_success_enoughCapacity);
    tcase_add_test(test_case_AString_appendCString, test_AString_appendCString_success_notEnoughCapacity);
    tcase_add_test(test_case_AString_appendCString, test_AString_appendCString_success_nullptrArray);
    tcase_add_test(test_case_AString_appendCString, test_AString_appendCString_success_zeroArraySize);
    tcase_add_test(test_case_AString_appendCString, test_AString_appendCString_failure_bufferExpansionFailed);
    tcase_add_test(test_case_AString_appendCString, test_AString_appendCString_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_AString_appendCString);

    test_case_AString_appendAString = tcase_create("AString Test Case: AString_appendAString");
    tcase_add_test(test_case_AString_appendAString, test_AString_appendAString_success_enoughCapacity);
    tcase_add_test(test_case_AString_appendAString, test_AString_appendAString_success_notEnoughCapacity);
    tcase_add_test(test_case_AString_appendAString, test_AString_appendAString_success_nullptrSrcArray);
    tcase_add_test(test_case_AString_appendAString, test_AString_appendAString_success_zeroSizeSrcArray);
    tcase_add_test(test_case_AString_appendAString, test_AString_appendAString_failure_bufferExpansionFailed);
    tcase_add_test(test_case_AString_appendAString, test_AString_appendAString_failure_nullptrDestArray);
    suite_add_tcase(s, test_case_AString_appendAString);

    test_case_AString_get = tcase_create("AString Test Case: AString_get");
    tcase_add_test(test_case_AString_get, test_AString_get_indexInBounds);
    tcase_add_test(test_case_AString_get, test_AString_get_indexOutOfBounds);
    tcase_add_test(test_case_AString_get, test_AString_get_nullptr);
    suite_add_tcase(s, test_case_AString_get);

    test_case_AString_set = tcase_create("AString Test Case: AString_set");
    tcase_add_test(test_case_AString_set, test_AString_set_success_indexInBounds);
    tcase_add_test(test_case_AString_set, test_AString_set_success_indexBeyondSize);
    tcase_add_test(test_case_AString_set, test_AString_set_success_indexBeyondSize_bufferExpanded);
    tcase_add_test(test_case_AString_set, test_AString_set_failure_indexBeyondSize_bufferExpansionFailed);
    tcase_add_test(test_case_AString_set, test_AString_set_failure_nullptr);
    suite_add_tcase(s, test_case_AString_set);

    test_case_AString_setRange = tcase_create("AString Test Case: AString_setRange");
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_success_indexAndRangeInBounds);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_success_indexInBoundsRangeBeyondSize);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_success_indexInBoundsRangeBeyondSize_bufferExpanded);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_success_indexAndRangeBeyondSize);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_success_indexAndRangeBeyondSize_bufferExpanded);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_failure_indexInBoundsRangeBeyondSize_bufferExpansionFailed);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_failure_indexAndRangeBeyondSize_bufferExpansionFailed);
    tcase_add_test(test_case_AString_setRange, test_AString_setRange_failure_nullptr);
    suite_add_tcase(s, test_case_AString_setRange);

    test_case_AString_equals = tcase_create("AString Test Case: AString_equals");
    tcase_add_test(test_case_AString_equals, test_AString_equals_valid);
    tcase_add_test(test_case_AString_equals, test_AString_equals_nullptr);
    suite_add_tcase(s, test_case_AString_equals);

    test_case_AString_equalsCString = tcase_create("AString Test Case: AString_equalsCString");
    tcase_add_test(test_case_AString_equalsCString, test_AString_equalsCString_valid);
    tcase_add_test(test_case_AString_equalsCString, test_AString_equalsCString_nullptr);
    suite_add_tcase(s, test_case_AString_equalsCString);

    test_case_AString_compare = tcase_create("AString Test Case: AString_compare");
    tcase_add_test(test_case_AString_compare, test_AString_compare_equals);
    tcase_add_test(test_case_AString_compare, test_AString_compare_firstLessThanSecond);
    tcase_add_test(test_case_AString_compare, test_AString_compare_firstGreaterThanSecond);
    tcase_add_test(test_case_AString_compare, test_AString_compare_nullptr);
    suite_add_tcase(s, test_case_AString_compare);

    test_case_AString_compareCString = tcase_create("AString Test Case: AString_compareCString");
    tcase_add_test(test_case_AString_compareCString, test_AString_compareCString_equals);
    tcase_add_test(test_case_AString_compareCString, test_AString_compareCString_firstLessThanSecond);
    tcase_add_test(test_case_AString_compareCString, test_AString_compareCString_firstGreaterThanSecond);
    tcase_add_test(test_case_AString_compareCString, test_AString_compareCString_nullptr);
    suite_add_tcase(s, test_case_AString_compareCString);

    test_case_AString_substring = tcase_create("AString Test Case: AString_substring");
    tcase_add_test(test_case_AString_substring, test_AString_substring_indexRangeInBounds);
    tcase_add_test(test_case_AString_substring, test_AString_substring_rangeBeyondBounds);
    tcase_add_test(test_case_AString_substring, test_AString_substring_indexBeyondBounds);
    tcase_add_test(test_case_AString_substring, test_AString_substring_reallocationFailed);
    tcase_add_test(test_case_AString_substring, test_AString_substring_nullptr);
    suite_add_tcase(s, test_case_AString_substring);

    test_case_AString_clone = tcase_create("AString Test Case: AString_clone");
    tcase_add_test(test_case_AString_clone, test_AString_clone_valid);
    tcase_add_test(test_case_AString_clone, test_AString_clone_noMemoryAvailable);
    tcase_add_test(test_case_AString_clone, test_AString_clone_nullptr);
    suite_add_tcase(s, test_case_AString_clone);

    test_case_AString_substring = tcase_create("AString Test Case: AString_substring");
    tcase_add_test(test_case_AString_substring, test_AString_substring_indexRangeInBounds);
    tcase_add_test(test_case_AString_substring, test_AString_substring_rangeBeyondBounds);
    tcase_add_test(test_case_AString_substring, test_AString_substring_indexBeyondBounds);
    tcase_add_test(test_case_AString_substring, test_AString_substring_reallocationFailed);
    tcase_add_test(test_case_AString_substring, test_AString_substring_nullptr);
    suite_add_tcase(s, test_case_AString_substring);

    test_case_AString_split = tcase_create("AString Test Case: AString_split");
    tcase_add_test(test_case_AString_split, test_AString_split_emptyString);
    tcase_add_test(test_case_AString_split, test_AString_split_noDelimiter);
    tcase_add_test(test_case_AString_split, test_AString_split_firstCharDelimiter);
    tcase_add_test(test_case_AString_split, test_AString_split_lastCharDelimiter);
    tcase_add_test(test_case_AString_split, test_AString_split_multipleDelimiters);
    tcase_add_test(test_case_AString_split, test_AString_split_multipleDelimiters_discardEmpty);
    tcase_add_test(test_case_AString_split, test_AString_split_noMemoryAvailable);
    tcase_add_test(test_case_AString_split, test_AString_split_nullptr);
    suite_add_tcase(s, test_case_AString_split);
    
    return s;
}
