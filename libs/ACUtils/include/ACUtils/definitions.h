#ifndef ACUTILSTEST_DEFINITIONS_H
#define ACUTILSTEST_DEFINITIONS_H

#if A_C_UTILS_ONE_SOURCE
#   define ST_FUNC static
#else
#   define ST_FUNC
#endif

#define ST_DATA ST_FUNC

#ifndef __cplusplus
#   ifndef __STDC_VERSION__
#       define __STDC_VERSION__ 0L
#   endif
#   if __STDC_VERSION__ >= 199901L
#       include <stdbool.h>
#   else
typedef enum { false, true } bool;
#   endif
#endif

#endif /* ACUTILSTEST_DEFINITIONS_H */
