#ifndef ACUTILS_TYPES_H
#define ACUTILS_TYPES_H

#include <stddef.h>

#include "macros.h"

#ifndef __cplusplus
#   ifdef ACUTILS_C_STANDARD_99
#       include <stdbool.h>
#   else
        typedef enum { false, true } bool;
#   endif
#endif

typedef void*(*ACUtilsReallocator)(void* ptr, size_t size);
typedef void(*ACUtilsDeallocator)(void* ptr);
typedef size_t(*ACUtilsGrowStrategy)(size_t requiredSize, size_t typeSize);

#endif /* ACUTILS_TYPES_H */
