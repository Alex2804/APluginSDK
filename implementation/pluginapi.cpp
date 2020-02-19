#ifdef A_PLUGIN_SDK_EXCLUDE_DEFINITIONS
# include "../pluginapi.h"
#endif

// when pluginapi.cpp is compiled, all other source files should be compiled too
#include "infomanager.cpp"

void* apl::allocateMemory(size_t size)
{
#ifdef A_PLUGIN_SDK_DEBUG
    debug::allocationCount += 1;
#endif //A_PLUGIN_SDK_DEBUG
    return detail::allocateMemoryNotExported(size);
}
void apl::freeMemory(void *ptr)
{
#ifdef A_PLUGIN_SDK_DEBUG
    debug::freeCount += 1;
#endif //A_PLUGIN_SDK_DEBUG
    detail::freeMemoryNotExported(ptr);
}

const apl::PluginInfo* apl::detail::getPluginInfo()
{
    return InfoManager::instance()->getPluginInfo();
}

size_t apl::detail::getPluginFeatureCount()
{
    return getPluginFeatureCountNotExported();
}
const apl::PluginFeatureInfo* apl::detail::getPluginFeatureInfo(size_t index)
{
    return getPluginFeatureInfoNotExported(index);
}
const apl::PluginFeatureInfo * const* apl::detail::getPluginFeatureInfos()
{
    return getPluginFeatureInfosNotExported();
}

size_t apl::detail::getPluginClassCount()
{
    return getPluginClassCountNotExported();
}
const apl::PluginClassInfo* apl::detail::getPluginClassInfo(size_t index)
{
    return getPluginClassInfoNotExported(index);
}
const apl::PluginClassInfo* const* apl::detail::getPluginClassInfos()
{
    return getPluginClassInfosNotExported();
}
