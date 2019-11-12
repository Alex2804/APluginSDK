#ifndef APLUGINSDK_MACROS_H
#define APLUGINSDK_MACROS_H

#ifndef APLUGINSDK_EXPORT
# if defined(APLUGINLIBRARY_EXPORT)
#  define APLUGINSDK_EXPORT APLUGINLIBRARY_EXPORT
# elif defined(_WIN32)
#  define APLUGINSDK_EXPORT __declspec( dllexport )
# else
#  define APLUGINSDK_EXPORT
# endif
#endif

#ifndef APLUGINSDK_NO_EXPORT
# if defined(APLUGINLIBRARY_NO_EXPORT)
#  define APLUGINSDK_NO_EXPORT APLUGINLIBRARY_NO_EXPORT
# elif defined(__unix__) || defined(__APPLE__)
#  define APLUGINSDK_NO_EXPORT __attribute__((visibility("hidden")))
# else
#  define APLUGINSDK_NO_EXPORT
# endif
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
