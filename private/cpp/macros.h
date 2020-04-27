#ifndef APLUGINSDK_CPP_MACROS_H
#define APLUGINSDK_CPP_MACROS_H

#define PRIVATE_APLUGINSDK_OPEN_EXTERN_C extern "C" {
#define PRIVATE_APLUGINSDK_CLOSE_EXTERN_C }

#define APLUGINLIBRARY_NAMESPACE apl::
#define PRIVATE_APLUGINLIBRARY_OPEN_NAMESPACE namespace apl {
#define PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE }

#define PRIVATE_APLUGINSDK_PLUGIN_LANGUAGE APLUGINLIBRARY_NAMESPACE APluginLanguage::CPP

#define PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE APLUGINLIBRARY_NAMESPACE sdk::detail::
#define PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE PRIVATE_APLUGINLIBRARY_OPEN_NAMESPACE namespace sdk { namespace detail {
#define PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE }} PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE

#define PRIVATE_APLUGINSDK_API_NAMESPACE PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE api::
#define PRIVATE_APLUGINSDK_OPEN_API_NAMESPACE PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE namespace api {
#define PRIVATE_APLUGINSDK_CLOSE_API_NAMESPACE } PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

/* private plugin initialization function macro */
#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
    #define PRIVATE_APLUGINSDK_INIT_FUNCTION                                                                           \
        PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                      \
            namespace implementation { namespace init {                                                                \
                APLUGINSDK_NO_EXPORT bool initAPluginFunctionRegistered =                                              \
                    private_APluginSDK_registerInitAPluginFunction(                                                    \
                        reinterpret_cast<void*>(PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE api::initAPlugin));               \
            }}                                                                                                         \
        PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                     \
        void PRIVATE_APLUGINSDK_API_NAMESPACE initAPlugin()
#endif

/* private plugin finalization function macro */
#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
    #define PRIVATE_APLUGINSDK_FINI_FUNCTION                                                                           \
        PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                      \
            namespace implementation { namespace fini {                                                                \
                APLUGINSDK_NO_EXPORT bool finiAPluginFunctionRegistered =                                              \
                    private_APluginSDK_registerFiniAPluginFunction(                                                    \
                        reinterpret_cast<void*>(PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE api::finiAPlugin));               \
            }}                                                                                                         \
        PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                     \
        void PRIVATE_APLUGINSDK_API_NAMESPACE finiAPlugin()
#endif

/* private plugin name macro */
#define PRIVATE_APLUGINSDK_SET_NAME(pluginName)                                                                        \
    PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                          \
        namespace implementation { namespace name {                                                                    \
            APLUGINSDK_NO_EXPORT bool pluginNameSet = private_APluginSDK_setPluginName(#pluginName);                   \
        }}                                                                                                             \
    PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

/* private plugin version macro */
#define PRIVATE_APLUGINSDK_SET_VERSION(major, minor, patch)                                                            \
    PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                          \
        namespace implementation { namespace name {                                                                    \
            APLUGINSDK_NO_EXPORT bool pluginVersionSet = private_APluginSDK_setPluginVersion(major, minor, patch);     \
        }}                                                                                                             \
    PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

/* private plugin feature macro */
#define PRIVATE_APLUGINSDK_REGISTER_FEATURE(returnType, featureGroup, featureName, ...)                                \
    PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                          \
        namespace implementation { namespace features {                                                                \
            namespace feature_group_##featureGroup { namespace feature_name_##featureName {                            \
                APLUGINSDK_NO_EXPORT returnType featureFunction(__VA_ARGS__);                                          \
                APLUGINSDK_NO_EXPORT bool pluginFeatureRegistered = private_APluginSDK_registerFeature(#featureGroup,  \
                    #featureName, #returnType, "" #__VA_ARGS__,  reinterpret_cast<void*>(featureFunction));            \
            }}                                                                                                         \
        }}                                                                                                             \
    PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                         \
    returnType PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE implementation::features::feature_group_##featureGroup::           \
        feature_name_##featureName::featureFunction(__VA_ARGS__)

#define PRIVATE_APLUGINSDK_RECORD_FEATURE(featureGroup, featureName) ((void)0) /* no-op but force ';' behind macro */

/* private plugin class macros */
#define PRIVATE_APLUGINSDK_REGISTER_CLASS(interfaceName, className)                                                    \
    PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                          \
        namespace implementation { namespace features {                                                                \
            namespace interface_name_##interfaceName { namespace class_name_##className {                              \
                APLUGINSDK_NO_EXPORT className* createInstance()                                                       \
                {                                                                                                      \
                    return new className();                                                                            \
                }                                                                                                      \
                APLUGINSDK_NO_EXPORT void deleteInstance(className* ptr)                                               \
                {                                                                                                      \
                    delete ptr;                                                                                        \
                }                                                                                                      \
                APLUGINSDK_NO_EXPORT bool pluginClassRegistered = private_APluginSDK_registerClass(#interfaceName,     \
                    #className, reinterpret_cast<void*>(createInstance), reinterpret_cast<void*>(deleteInstance));     \
            }}                                                                                                         \
        }}                                                                                                             \
    PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

#endif /* APLUGINSDK_CPP_MACROS_H */
