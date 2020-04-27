#ifndef ACUTILS_TYPES_H
#define ACUTILS_TYPES_H

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

#endif /* ACUTILS_TYPES_H */
