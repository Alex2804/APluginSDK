#ifndef APLUGINSDK_PLUGINAPI_H
#define APLUGINSDK_PLUGINAPI_H

#include "implementation/infomanager.h"
#include "implementation/macros.h"

namespace apl
{
    constexpr size_t A_PLUGIN_API_VERSION_MAJOR = 1;
    constexpr size_t A_PLUGIN_API_VERSION_MINOR = 0;
    constexpr size_t A_PLUGIN_API_VERSION_PATCH = 0;

    extern "C"
    {
        APLUGINSDK_API void* allocateMemory(size_t size);
        APLUGINSDK_API void freeMemory(void* ptr);
    }

    namespace detail
    {
        extern "C"
        {
            APLUGINSDK_API const PluginInfo* getPluginInfo();

            APLUGINSDK_API size_t getPluginFeatureCount();
            APLUGINSDK_API const PluginFeatureInfo* getPluginFeatureInfo(size_t index);
            APLUGINSDK_API const PluginFeatureInfo* const* getPluginFeatureInfos();

            APLUGINSDK_API size_t getPluginClassCount();
            APLUGINSDK_API const PluginClassInfo* getPluginClassInfo(size_t index);
            APLUGINSDK_API const PluginClassInfo* const* getPluginClassInfos();
        }
    }
}

#ifdef A_PLUGIN_SDK_DEBUG
namespace apl
{
    namespace debug
    {
        extern "C"
        {
            extern size_t allocationCount;
            extern size_t freeCount;
        }
    }
}
#endif //A_PLUGIN_SDK_DEBUG

#ifndef A_PLUGIN_SDK_EXCLUDE_DEFINITIONS
# include "implementation/pluginapi.cpp"
#endif

#define A_PLUGIN_SET_NAME(pluginName)                                                                                  \
    __A_PLUGIN_NAME_OPEN_NAMESPACE__(pluginName)                                                                       \
        class __A_PLUGIN_NAME_NAME__(pluginName)                                                                       \
        {                                                                                                              \
        public:                                                                                                        \
            static const char* plugin_name;                                                                            \
        };                                                                                                             \
        const char* __A_PLUGIN_NAME_NAME__(pluginName)::plugin_name =                                                  \
                                           apl::detail::InfoManager::setPluginName(#pluginName);                       \
    __A_PLUGIN_NAME_CLOSE_NAMESPACE__

#define A_PLUGIN_SET_VERSION(versionMajor, versionMinor, versionPatch)                                                 \
    __A_PLUGIN_VERSION_OPEN_NAMESPACE__(versionMajor, versionMinor, versionPatch)                                      \
        class __A_PLUGIN_VERSION_NAME__(versionMajor, versionMinor, versionPatch)                                      \
        {                                                                                                              \
        public:                                                                                                        \
            static size_t plugin_version_major;                                                                        \
        };                                                                                                             \
        size_t __A_PLUGIN_VERSION_NAME__(versionMajor, versionMinor, versionPatch)::plugin_version_major =             \
                apl::detail::InfoManager::setPluginVersion(versionMajor, versionMinor, versionPatch);                  \
    __A_PLUGIN_VERSION_CLOSE_NAMESPACE__

#define A_PLUGIN_REGISTER_FEATURE(returnType, featureGroup, featureName, ...)                                          \
    __A_PLUGIN_FEATURE_OPEN_NAMESPACE__(featureGroup, featureName)                                                     \
        class __A_PLUGIN_FEATURE_NAME__(featureGroup, featureName)                                                     \
        {                                                                                                              \
        public:                                                                                                        \
            static apl::PluginFeatureInfo* feature_info;                                                               \
            static returnType featureBody(__VA_ARGS__);                                                                \
        };                                                                                                             \
                                                                                                                       \
        apl::PluginFeatureInfo* __A_PLUGIN_FEATURE_NAME__(featureGroup, featureName)::feature_info =                   \
            apl::detail::InfoManager::registerFeature(#featureGroup, #featureName, #returnType, #__VA_ARGS__,          \
                reinterpret_cast<void*>(__A_PLUGIN_FEATURE_NAME__(featureGroup, featureName)::featureBody));           \
    __A_PLUGIN_FEATURE_CLOSE_NAMESPACE__                                                                               \
                                                                                                                       \
    returnType __A_PLUGIN_FEATURE_NAMESPACE__(featureGroup, featureName) \
                                __A_PLUGIN_FEATURE_NAME__(featureGroup, featureName)::featureBody(__VA_ARGS__)


#define A_PLUGIN_REGISTER_CLASS(interfaceName, className)                                                              \
    __A_PLUGIN_CLASS_OPEN_NAMESPACE__(interfaceName, className)                                                        \
        class __A_PLUGIN_CLASS_NAME__(interfaceName, className)                                                        \
        {                                                                                                              \
        public:                                                                                                        \
            static apl::PluginClassInfo* info;                                                                         \
            static className* createInstance();                                                                        \
            static void deleteInstance(className*);                                                                    \
        };                                                                                                             \
                                                                                                                       \
        apl::PluginClassInfo* __A_PLUGIN_CLASS_NAME__(interfaceName, className)::info =                                \
            apl::detail::InfoManager::registerClass(#interfaceName, #className,                                        \
                reinterpret_cast<void*>(__A_PLUGIN_CLASS_NAME__(interfaceName, className)::createInstance),            \
                reinterpret_cast<void*>(__A_PLUGIN_CLASS_NAME__(interfaceName, className)::deleteInstance));           \
                                                                                                                       \
        className* __A_PLUGIN_CLASS_NAME__(interfaceName, className)::createInstance()                                 \
        {                                                                                                              \
            return new className();                                                                                    \
        }                                                                                                              \
        void __A_PLUGIN_CLASS_NAME__(interfaceName, className)::deleteInstance(className* ptr)                         \
        {                                                                                                              \
            delete ptr;                                                                                                \
        }                                                                                                              \
    __A_PLUGIN_CLASS_CLOSE_NAMESPACE__

#ifdef A_PLUGIN_NAME
A_PLUGIN_SET_NAME(A_PLUGIN_NAME)
#endif

#if defined(A_PLUGIN_VERSION_MAJOR) || defined(A_PLUGIN_VERSION_MINOR) || defined(A_PLUGIN_VERSION_PATCH)
# ifndef A_PLUGIN_VERSION_MAJOR
#  define A_PLUGIN_VERSION_MAJOR 0
# endif
# ifndef A_PLUGIN_VERSION_MINOR
#  define A_PLUGIN_VERSION_MINOR 0
# endif
# ifndef A_PLUGIN_VERSION_PATCH
#  define A_PLUGIN_VERSION_PATCH 0
# endif
A_PLUGIN_SET_VERSION(A_PLUGIN_VERSION_MAJOR, A_PLUGIN_VERSION_MINOR, A_PLUGIN_VERSION_PATCH)
#endif

#endif //APLUGINSDK_PLUGINAPI_H
