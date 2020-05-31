#ifndef APLUGINSDK_CPP_MACROS_H
#define APLUGINSDK_CPP_MACROS_H

#define APLUGINLIBRARY_NAMESPACE apl::
#define PRIVATE_APLUGINLIBRARY_OPEN_NAMESPACE namespace apl {
#define PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE }

#define PRIVATE_APLUGINSDK_PLUGIN_LANGUAGE APLUGINLIBRARY_NAMESPACE CPP

#define PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE APLUGINLIBRARY_NAMESPACE sdk::detail::
#define PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE PRIVATE_APLUGINLIBRARY_OPEN_NAMESPACE namespace sdk { namespace detail {
#define PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE }} PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE

#define PRIVATE_APLUGINSDK_API_NAMESPACE PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE api::
#define PRIVATE_APLUGINSDK_OPEN_API_NAMESPACE PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE namespace api {
#define PRIVATE_APLUGINSDK_CLOSE_API_NAMESPACE } PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE


/* private plugin initialization function macro */
#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
#   define PRIVATE_APLUGINSDK_INIT_FUNCTION                                                                            \
        PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                      \
            namespace implementation { namespace init {                                                                \
                APLUGINSDK_NO_EXPORT bool initAPluginFunctionRegistered =                                              \
                    private_APluginSDK_registerInitAPluginFunction(                                                   \
                        reinterpret_cast<void*>(PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_initPlugin));              \
            }}                                                                                                         \
        PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                     \
        void PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_initPlugin(void)
#endif

/* private plugin finalization function macro */
#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
#   define PRIVATE_APLUGINSDK_FINI_FUNCTION                                                                            \
        PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                      \
            namespace implementation { namespace fini {                                                                \
                APLUGINSDK_NO_EXPORT bool finiAPluginFunctionRegistered =                                              \
                    private_APluginSDK_registerFiniAPluginFunction(                                                   \
                        reinterpret_cast<void*>(PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_finiPlugin));              \
            }}                                                                                                         \
        PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                     \
        void PRIVATE_APLUGINSDK_API_NAMESPACE APluginSDK_finiPlugin(void)
#endif

/* private plugin name macro */
#define PRIVATE_APLUGINSDK_SET_NAME(pluginName)                                                                        \
    static bool private_APluginSDK_implementation_name_pluginNameSet =                                                \
        PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE private_APluginSDK_setPluginName(#pluginName)

/* private plugin version macro */
#define PRIVATE_APLUGINSDK_SET_VERSION(major, minor, patch)                                                            \
    static bool private_APluginSDK_implementation_version_pluginVersionSet =                                          \
        PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE private_APluginSDK_setPluginVersion(major, minor, patch)

/* private plugin feature macro */
#define PRIVATE_APLUGINSDK_REGISTER_FEATURE(returnType, featureGroup, featureName, ...)                                \
    PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                          \
        namespace implementation { namespace features { namespace featureGroup { namespace featureName {               \
            APLUGINSDK_NO_EXPORT returnType featureFunction(__VA_ARGS__);                                              \
            APLUGINSDK_NO_EXPORT bool pluginFeatureRegistered = private_APluginSDK_registerFeature(#featureGroup,     \
                #featureName, #returnType, "" #__VA_ARGS__,  reinterpret_cast<void*>(featureFunction));                \
        }}}}                                                                                                           \
    PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                         \
    returnType PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE implementation::features::featureGroup::featureName::featureFunction(__VA_ARGS__)

#define PRIVATE_APLUGINSDK_RECORD_FEATURE(featureGroup, featureName) \
    { ((void)PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE implementation::features::featureGroup::featureName::pluginFeatureRegistered); } ((void)0)

/* private plugin class macros */
#define PRIVATE_APLUGINSDK_REGISTER_CLASS(interfaceName, className)                                                    \
    PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE                                                                          \
        namespace implementation { namespace classes {                                                                 \
            namespace interface_##interfaceName { namespace class_##className {                                        \
                APLUGINSDK_NO_EXPORT className* createInstance()                                                       \
                {                                                                                                      \
                    return new className();                                                                            \
                }                                                                                                      \
                APLUGINSDK_NO_EXPORT void deleteInstance(className* ptr)                                               \
                {                                                                                                      \
                    delete ptr;                                                                                        \
                }                                                                                                      \
            }}                                                                                                         \
        }}                                                                                                             \
    PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE                                                                         \
    APLUGINSDK_NO_EXPORT bool private_APluginSDK_implementation_classes_##interfaceName##_##className =               \
        :: PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE private_APluginSDK_registerClass(#interfaceName, #className,          \
            reinterpret_cast<void*>(PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE implementation::classes::                     \
                interface_##interfaceName::class_##className::createInstance),                                         \
            reinterpret_cast<void*>(PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE implementation::classes::                     \
                interface_##interfaceName::class_##className::deleteInstance))

#endif /* APLUGINSDK_CPP_MACROS_H */
