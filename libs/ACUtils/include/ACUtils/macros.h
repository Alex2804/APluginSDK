#ifndef ACUTILS_MACROS_H
#define ACUTILS_MACROS_H

#if defined(__STDC__)
#   define ACUTILS_C_STANDARD_89
#   if defined(__STDC_VERSION__)
#       define ACUTILS_C_STANDARD_90
#       if (__STDC_VERSION__ >= 199409L)
#           define ACUTILS_C_STANDARD_94
#       endif
#       if (__STDC_VERSION__ >= 199901L)
#           define ACUTILS_C_STANDARD_99
#       endif
#       if (__STDC_VERSION__ >=  201112L)
#           define ACUTILS_C_STANDARD_11
#       endif
#       if (__STDC_VERSION__ >= 201710L)
#           define ACUTILS_C_STANDARD_17
#           define ACUTILS_C_STANDARD_18
#       endif
#   endif
#endif

#ifdef ACUTILS_ONE_SOURCE
#   ifdef ACUTILS_C_STANDARD_99
#       define ACUTILS_HD_FUNC static inline
#   else
#       define ACUTILS_HD_FUNC static
#   endif
#else
#   define ACUTILS_HD_FUNC
#endif

#ifdef __cplusplus
#   define ACUTILS_EXTERN_C extern "C"
#   define ACUTILS_OPEN_EXTERN_C extern "C" {
#   define ACUTILS_CLOSE_EXTERN_C }
#else
#   define nullptr NULL
#   define ACUTILS_EXTERN_C
#   define ACUTILS_OPEN_EXTERN_C
#   define ACUTILS_CLOSE_EXTERN_C
#endif

#endif /* ACUTILS_MACROS_H */
