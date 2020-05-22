#ifndef APLUGINSDK_PLUGININFOS_H
#define APLUGINSDK_PLUGININFOS_H

#include "private/macros.h"
#include "private/types.h"

PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE
    struct APrivatePluginInfo;
PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

PRIVATE_APLUGINLIBRARY_OPEN_NAMESPACE
    PRIVATE_APLUGINSDK_OPEN_EXTERN_C
        struct APluginInfo;

        struct APluginFeatureInfo
        {
            const struct APluginInfo* pluginInfo;
            const char* featureGroup;
            const char* featureName;
            const char* returnType;
            const char* parameterList;
            void* functionPointer;
        };

        struct APluginClassInfo
        {
            const struct APluginInfo* pluginInfo;
            const char* interfaceName;
            const char* className;
            void* createInstance;
            void* deleteInstance;
        };

        enum APluginLanguage
        {
            CPP, C
        };

        struct APluginInfo
        {
            struct PRIVATE_APLUGINSDK_PRIVATE_NAMESPACE APrivatePluginInfo* privateInfo;

            size_t apiVersionMajor, apiVersionMinor, apiVersionPatch;

            void*(*allocateMemory)(size_t size);
            void(*freeMemory)(void*);

            enum APluginLanguage pluginLanguage;
            char* pluginName;
            size_t pluginVersionMajor, pluginVersionMinor, pluginVersionPatch;

            size_t(*getFeatureCount)();
            const struct APluginFeatureInfo*(*getFeatureInfo)(size_t index);
            const struct APluginFeatureInfo * const*(*getFeatureInfos)();

            size_t(*getClassCount)();
            const struct APluginClassInfo*(*getClassInfo)(size_t index);
            const struct APluginClassInfo* const*(*getClassInfos)();
        };
    PRIVATE_APLUGINSDK_CLOSE_EXTERN_C
PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE

#endif /* APLUGINSDK_PLUGININFOS_H */
