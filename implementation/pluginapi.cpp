#ifdef A_PLUGIN_SDK_EXCLUDE_IMPLEMENTATION
# include "../pluginapi.h"
#endif

// when pluginapi.cpp is compiled, all other source files should be compiled too
#include "infomanager.cpp"

#ifdef A_PLUGIN_SDK_DEBUG
namespace apl
{
    namespace debug
    {
        extern "C"
        {
            size_t allocationCount = 0;
            size_t freeCount = 0;
        }
    }
}
#endif //A_PLUGIN_SDK_DEBUG

/**
 * Allocates memory inside the plugin which can be freed by apl::freeMemory.
 * @param size The number of bytes to allocate
 * @return The allocated memory.
 */
void* apl::allocateMemory(size_t size)
{
#ifdef A_PLUGIN_SDK_DEBUG
    if(size > 0)
        debug::allocationCount += 1;
#endif //A_PLUGIN_SDK_DEBUG
    return detail::allocateMemoryNotExported(size);
}
/**
 * Frees memory which was allocated by this plugin.
 * @param ptr A pointer to the memory to free.
 */
void apl::freeMemory(void *ptr)
{
#ifdef A_PLUGIN_SDK_DEBUG
    if(ptr != nullptr)
        debug::freeCount += 1;
#endif //A_PLUGIN_SDK_DEBUG
    detail::freeMemoryNotExported(ptr);
}

const apl::PluginInfo* apl::detail::getPluginInfo()
{
    return InfoManager::instance()->getPluginInfo();
}
