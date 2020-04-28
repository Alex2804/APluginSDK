#if APLUGINSDK_EXCLUDE_IMPLEMENTATION
# include "../../pluginapi.h"
#endif

/* when pluginapi.cpp is compiled, all other source files should be compiled too */
#include "../../private/src/infomanager.c"

void* APLUGINLIBRARY_NAMESPACE allocateMemory(size_t size)
{
    return PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE _private_APluginSDK_allocateMemory(size);
}

void APLUGINLIBRARY_NAMESPACE freeMemory(void *ptr)
{
    PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE _private_APluginSDK_freeMemory(ptr);
}

const struct APLUGINLIBRARY_NAMESPACE APluginInfo* PRIVATE_APLUGINSDK_API_NAMESPACE getAPluginInfo()
{
    return PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE _private_APluginSDK_getPluginInfo();
}
