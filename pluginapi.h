#ifndef APLUGINSDK_PLUGINAPI_H
#define APLUGINSDK_PLUGINAPI_H

#include "implementation/featuremanager.h"
#include "implementation/macros.h"

namespace apl
{
    namespace detail
    {
        extern "C"
        {
            APLUGINLIBRARY_EXPORT size_t getPluginFeatureCount();
            APLUGINLIBRARY_EXPORT const apl::PluginFeatureInfo* getPluginFeatureInfo(size_t index);
            APLUGINLIBRARY_EXPORT const apl::PluginFeatureInfo * const* getPluginFeatureInfos();

            APLUGINLIBRARY_EXPORT size_t getPluginClassCount();
            APLUGINLIBRARY_EXPORT const apl::PluginClassInfo* getPluginClassInfo(size_t index);
            APLUGINLIBRARY_EXPORT const apl::PluginClassInfo* const* getPluginClassInfos();
        }
    }
}


#define A_PLUGIN_REGISTER_FEATURE(returnType, featureGroup, featureName, ...)                                          \
    A_PLUGIN_FEATURE_OPEN_NAMESPACE(featureGroup, featureName)                                                         \
        class A_PLUGIN_FEATURE_NAME(featureGroup, featureName)                                                         \
        {                                                                                                              \
        public:                                                                                                        \
            static apl::PluginFeatureInfo* feature_info;                                                               \
            static returnType featureBody(__VA_ARGS__);                                                                \
        };                                                                                                             \
                                                                                                                       \
        apl::PluginFeatureInfo* A_PLUGIN_FEATURE_NAME(featureGroup, featureName)::feature_info =                       \
            apl::detail::FeatureManager::registerFeature(#featureGroup, #featureName, #returnType, #__VA_ARGS__,       \
                reinterpret_cast<void*>(A_PLUGIN_FEATURE_NAME(featureGroup, featureName)::featureBody));               \
    A_PLUGIN_FEATURE_CLOSE_NAMESPACE                                                                                   \
                                                                                                                       \
    returnType A_PLUGIN_FEATURE_NAMESPACE(featureGroup, featureName)A_PLUGIN_FEATURE_NAME(featureGroup, featureName)   \
                                                                                          ::featureBody(__VA_ARGS__)


#define A_PLUGIN_REGISTER_CLASS(interfaceName, className)                                                              \
    A_PLUGIN_CLASS_OPEN_NAMESPACE(interfaceName, className)                                                            \
        class A_PLUGIN_CLASS_NAME(interfaceName, className)                                                            \
        {                                                                                                              \
        public:                                                                                                        \
            static apl::PluginClassInfo* info;                                                                         \
            static className* createInstance();                                                                        \
            static void deleteInstance(className*);                                                                    \
        };                                                                                                             \
                                                                                                                       \
        apl::PluginClassInfo* A_PLUGIN_CLASS_NAME(interfaceName, className)::info =                                    \
            apl::detail::FeatureManager::registerClass(#interfaceName, #className,                                     \
                reinterpret_cast<void*>(A_PLUGIN_CLASS_NAME(interfaceName, className)::createInstance),                \
                reinterpret_cast<void*>(A_PLUGIN_CLASS_NAME(interfaceName, className)::deleteInstance));               \
                                                                                                                       \
        className* A_PLUGIN_CLASS_NAME(interfaceName, className)::createInstance()                                     \
        {                                                                                                              \
            return new className();                                                                                    \
        }                                                                                                              \
        void A_PLUGIN_CLASS_NAME(interfaceName, className)::deleteInstance(className* ptr)                             \
        {                                                                                                              \
            delete ptr;                                                                                                \
        }                                                                                                              \
    A_PLUGIN_CLASS_CLOSE_NAMESPACE

#ifndef A_PLUGIN_SDK_EXCLUDE_DEFINITION
# include "implementation/pluginapi.cpp"
#endif

#endif //APLUGINSDK_PLUGINAPI_H
