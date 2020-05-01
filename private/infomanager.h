#ifndef APLUGINSDK_INFOMANAGER_H
#define APLUGINSDK_INFOMANAGER_H

#include "../plugininfos.h"

PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE

    APLUGINSDK_NO_EXPORT const struct APLUGINLIBRARY_NAMESPACE APluginInfo* _private_APluginSDK_getPluginInfo(void);

    APLUGINSDK_NO_EXPORT bool _private_APluginSDK_setPluginName(const char *name);
    APLUGINSDK_NO_EXPORT bool _private_APluginSDK_setPluginVersion(size_t major, size_t minor, size_t patch);

    APLUGINSDK_NO_EXPORT bool _private_APluginSDK_registerFeature(const char *featureGroup, const char *featureName,
                                                                  const char *returnType, const char *parameterList,
                                                                  void* functionPtr);
    APLUGINSDK_NO_EXPORT bool _private_APluginSDK_registerClass(const char *interfaceClassName,
                                                                const char *featureClassName,
                                                                void *createInstance,
                                                                void *deleteInstance);

#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
    APLUGINSDK_NO_EXPORT bool _private_APluginSDK_registerInitAPluginFunction(void *functionPtr);
    APLUGINSDK_NO_EXPORT bool _private_APluginSDK_registerFiniAPluginFunction(void *functionPtr);
#endif

PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

#endif /* APLUGINSDK_INFOMANAGER_H */
