#ifndef APLUGINSDK_MACROS_H
#define APLUGINSDK_MACROS_H

#if !defined(APLUGINLIBRARY_EXPORT) && defined(_WIN32)
# define APLUGINLIBRARY_EXPORT  __declspec( dllexport )
#elif !defined(APLUGINLIBRARY_EXPORT)
# define APLUGINLIBRARY_EXPORT
#endif

#if !defined(APLUGINLIBRARY_NO_EXPORT) && (defined(__unix__) || defined(__APPLE__))
# define APLUGINLIBRARY_NO_EXPORT __attribute__((visibility("hidden")))
#elif !defined(APLUGINLIBRARY_NO_EXPORT)
# define APLUGINLIBRARY_NO_EXPORT
#endif


#define A_PLUGIN_FEATURE_NAMESPACE(featureGroup, featureName) \
    featureGroup##_feature_group::featureName##_feature_name::
#define A_PLUGIN_FEATURE_OPEN_NAMESPACE(featureGroup, featureName) \
    namespace featureGroup##_feature_group { namespace featureName##_feature_name {
#define A_PLUGIN_FEATURE_CLOSE_NAMESPACE \
    }}

#define A_PLUGIN_FEATURE_NAME(featureGroup, featureName) \
    APluginFeature_##featureGroup##_##featureName


#define A_PLUGIN_CLASS_OPEN_NAMESPACE(interfaceName, className) \
    namespace interfaceName##_interface_name { namespace className##_class_name {
#define A_PLUGIN_CLASS_CLOSE_NAMESPACE \
    }}

#define A_PLUGIN_CLASS_NAME(interfaceName, className) \
    APluginClass_##interfaceName##_##className

#endif //APLUGINSDK_MACROS_H
