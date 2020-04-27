#ifndef ACUTILSTEST_MACROS_H
#define ACUTILSTEST_MACROS_H

#if ACUTILS_ONE_SOURCE
#   define PRIVATE_ACUTILS_ST_FUNC static
#else
#   define PRIVATE_ACUTILS_ST_FUNC
#endif

#define PRIVATE_ACUTILS_ST_DATA PRIVATE_ACUTILS_ST_FUNC

#ifndef ACUTILS_SYMBOL_ATTRIBUTES
#   define ACUTILS_SYMBOL_ATTRIBUTES
#endif

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

#endif /* ACUTILSTEST_MACROS_H */
