#ifndef APLUGINSDK_MACROS_H
#define APLUGINSDK_MACROS_H

#include "../libs/ACUtils/include/ACUtils/macros.h"

#include "exports.h"

#ifdef __cplusplus
#   include "cpp/macros.h"
#else
#   include "c/macros.h"
#endif

#define PRIVATE_APLUGINSDK_OPEN_EXTERN_C ACUTILS_OPEN_EXTERN_C
#define PRIVATE_APLUGINSDK_CLOSE_EXTERN_C ACUTILS_CLOSE_EXTERN_C

/* private plugin initialization function macro */
#ifndef PRIVATE_APLUGINSDK_INIT_FUNCTION
#   define PRIVATE_APLUGINSDK_INIT_FUNCTION void PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_initPlugin(void)
#endif

/* private plugin finalization function macro*/
#ifndef PRIVATE_APLUGINSDK_FINI_FUNCTION
#   define PRIVATE_APLUGINSDK_FINI_FUNCTION void PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_finiPlugin(void)
#endif

#endif /* APLUGINSDK_MACROS_H */
