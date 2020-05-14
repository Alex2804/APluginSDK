#ifndef APLUGINSDK_C_MACROS_H
#define APLUGINSDK_C_MACROS_H

#define PRIVATE_APLUGINSDK_OPEN_EXTERN_C
#define PRIVATE_APLUGINSDK_CLOSE_EXTERN_C

#define APLUGINLIBRARY_NAMESPACE
#define PRIVATE_APLUGINLIBRARY_OPEN_NAMESPACE
#define PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE

#define PRIVATE_APLUGINSDK_PLUGIN_LANGUAGE C

#define PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE
#define PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE
#define PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

#define PRIVATE_APLUGINSDK_API_NAMESPACE
#define PRIVATE_APLUGINSDK_OPEN_API_NAMESPACE
#define PRIVATE_APLUGINSDK_CLOSE_API_NAMESPACE


/* private plugin name macro */
#define PRIVATE_APLUGINSDK_SET_NAME(pluginName) private_APluginSDK_setPluginName(#pluginName)

/* private plugin version macro */
#define PRIVATE_APLUGINSDK_SET_VERSION(major, minor, patch) private_APluginSDK_setPluginVersion(major, minor, patch)

/* private plugin feature macro */
#define PRIVATE_APLUGINSDK_REGISTER_FEATURE(returnType, featureGroup, featureName, ...)                                                           \
    APLUGINSDK_NO_EXPORT const char* private_APluginSDK_plugin_implemenation_feature_##featureGroup##_##featureName##_returnType = #returnType;  \
    APLUGINSDK_NO_EXPORT const char* private_APluginSDK_plugin_implemenation_feature_##featureGroup##_##featureName##_parameters = #__VA_ARGS__; \
    APLUGINSDK_NO_EXPORT returnType private_APluginSDK_plugin_implemenation_feature_##featureGroup##_##featureName##_function(__VA_ARGS__)

#define PRIVATE_APLUGINSDK_RECORD_FEATURE(featureGroup, featureName)                                                   \
    private_APluginSDK_registerFeature(#featureGroup, #featureName,                                                   \
        private_APluginSDK_plugin_implemenation_feature_##featureGroup##_##featureName##_returnType,                  \
        private_APluginSDK_plugin_implemenation_feature_##featureGroup##_##featureName##_parameters,                  \
        (void*) private_APluginSDK_plugin_implemenation_feature_##featureGroup##_##featureName##_function)

#endif /* APLUGINSDK_C_MACROS_H */
