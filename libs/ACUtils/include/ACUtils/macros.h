#ifndef ACUTILS_MACROS_H
#define ACUTILS_MACROS_H

#ifdef ACUTILS_ONE_SOURCE
#   define ACUTILS_ST_FUNC static
#else
#   define ACUTILS_ST_FUNC
#endif

#define ACUTILS_ST_DATA ACUTILS_ST_FUNC

#endif /* ACUTILS_MACROS_H */
