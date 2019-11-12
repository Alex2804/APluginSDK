#ifdef A_PLUGIN_SDK_EXCLUDE_DEFINITION
# include "../pluginapi.h"
#endif

/**
 * @param size The amount of memory to allocate in bytes
 * @return The allocated memory or nullptr on failure
 */
void* apl::detail::allocatePluginMemory(size_t size)
{
    return malloc(size);
}
/**
 * @param ptr The pointer with the memory to free
 */
void apl::detail::freePluginMemory(void *ptr)
{
    free(ptr);
}

size_t apl::detail::getPluginFeatureCount()
{
    return apl::detail::FeatureManager::getFeatureCount();
}
const apl::PluginFeatureInfo* apl::detail::getPluginFeatureInfo(size_t index)
{
    return apl::detail::FeatureManager::getFeatureInfo(index);
}
const apl::PluginFeatureInfo * const* apl::detail::getPluginFeatureInfos()
{
    return apl::detail::FeatureManager::getFeatureInfos();
}

size_t apl::detail::getPluginClassCount()
{
    return apl::detail::FeatureManager::getClassCount();
}
const apl::PluginClassInfo* apl::detail::getPluginClassInfo(size_t index)
{
    return apl::detail::FeatureManager::getClassInfo(index);
}
const apl::PluginClassInfo* const* apl::detail::getPluginClassInfos()
{
    return apl::detail::FeatureManager::getClassInfos();
}
