#if APLUGINSDK_EXCLUDE_IMPLEMENTATION
# include "../../pluginapi.h"
#endif

#include <stdlib.h>

/* when pluginapi.c is compiled, all other source files should be compiled too */
#include "../../private/src/infomanager.c"

APLUGINSDK_NO_EXPORT void* APLUGINLIBRARY_NAMESPACE APluginSDK_malloc(size_t size)
{
    return malloc(size);
}
APLUGINSDK_NO_EXPORT void* APLUGINLIBRARY_NAMESPACE APluginSDK_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}
APLUGINSDK_NO_EXPORT void APLUGINLIBRARY_NAMESPACE APluginSDK_free(void* ptr)
{
    free(ptr);
}

const struct APLUGINLIBRARY_NAMESPACE APluginInfo* PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_getPluginInfo(void)
{
    return PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE private_APluginSDK_getPluginInfo();
}
