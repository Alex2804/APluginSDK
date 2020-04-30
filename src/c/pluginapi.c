#if APLUGINSDK_EXCLUDE_IMPLEMENTATION
# include "../../pluginapi.h"
#endif

/* when pluginapi.cpp is compiled, all other source files should be compiled too */
#include "../../private/src/infomanager.c"

const struct APLUGINLIBRARY_NAMESPACE APluginInfo* PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_getPluginInfo()
{
    return PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE _private_APluginSDK_getPluginInfo();
}
