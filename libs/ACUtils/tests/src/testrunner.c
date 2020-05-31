#include "check.h"

#include <stdlib.h>

#include "ACUtils/macros.h"

#ifndef __cplusplus
#   undef ACUTILS_EXTERN_C
#   define ACUTILS_EXTERN_C extern
#endif

ACUTILS_EXTERN_C Suite* private_ACUtilsTest_ADynArray_getTestSuite(void);
ACUTILS_EXTERN_C Suite* private_ACUtilsTest_AString_getTestSuite(void);

int main(void)
{
    int numberFailed = 0;
    SRunner *runner;

    runner = srunner_create(private_ACUtilsTest_ADynArray_getTestSuite());
    srunner_set_fork_status(runner, CK_NOFORK);
    srunner_run_all(runner, CK_NORMAL);
    numberFailed += srunner_ntests_failed(runner);
    srunner_free(runner);

    runner = srunner_create(private_ACUtilsTest_AString_getTestSuite());
    srunner_set_fork_status(runner, CK_NOFORK);
    srunner_run_all(runner, CK_NORMAL);
    numberFailed += srunner_ntests_failed(runner);
    srunner_free(runner);

    return (numberFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
