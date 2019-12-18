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

#ifdef A_PLUGIN_SDK_DONT_EXPORT_API
# define APLUGINSDK_API APLUGINSDK_NO_EXPORT
#else
# define APLUGINSDK_API APLUGINSDK_EXPORT
#endif

#define __A_PLUGIN_NAME_OPEN_NAMESPACE__(pluginName) \
    namespace __apl_plugin_names__ { namespace plugin_name_##pluginName {
#define __A_PLUGIN_NAME_CLOSE_NAMESPACE__ \
    }}

#define __A_PLUGIN_NAME_NAME__(pluginName) \
    APluginName_##pluginName

#define __A_PLUGIN_VERSION_OPEN_NAMESPACE__(major, minor, patch) \
    namespace __apl_plugin_versions__ { namespace major_##major { namespace minor_##minor { namespace patch_##patch {
#define __A_PLUGIN_VERSION_CLOSE_NAMESPACE__ \
    }}}}

#define __A_PLUGIN_VERSION_NAME__(major, minor, patch) \
    APluginVersion_##major##_##minor##_##patch

#define __A_PLUGIN_FEATURE_NAMESPACE__(featureGroup, featureName) \
    __apl_plugin_features__::feature_group_##featureGroup::feature_name_##featureName::
#define __A_PLUGIN_FEATURE_OPEN_NAMESPACE__(featureGroup, featureName) \
    namespace __apl_plugin_features__ { namespace feature_group_##featureGroup { namespace feature_name_##featureName {
#define __A_PLUGIN_FEATURE_CLOSE_NAMESPACE__ \
    }}}

#define __A_PLUGIN_FEATURE_NAME__(featureGroup, featureName) \
    APluginFeature_##featureGroup##_##featureName


#define __A_PLUGIN_CLASS_OPEN_NAMESPACE__(interfaceName, className) \
    namespace __apl_plugin_classes__ { namespace interface_name_##interfaceName { namespace class_name_##className {
#define __A_PLUGIN_CLASS_CLOSE_NAMESPACE__ \
    }}}

#define __A_PLUGIN_CLASS_NAME__(interfaceName, className) \
    APluginClass_##interfaceName##_##className

#endif //APLUGINSDK_MACROS_H
