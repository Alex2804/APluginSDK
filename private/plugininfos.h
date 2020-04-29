#ifndef APLUGINSDK_PLUGININFOS_H
#define APLUGINSDK_PLUGININFOS_H

#include "macros.h"
#include "types.h"

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
            char* parameterTypes;
            char* parameterNames;
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
            size_t apiVersionMajor, apiVersionMinor, apiVersionPatch;
            enum APluginLanguage pluginLanguage;

            void(*releasePlugin)();

            void*(*allocateMemory)(size_t size);
            void(*freeMemory)(void*);

            size_t(*getFeatureCount)();
            const struct APluginFeatureInfo*(*getFeatureInfo)(size_t index);
            const struct APluginFeatureInfo * const*(*getFeatureInfos)();

            size_t(*getClassCount)();
            const struct APluginClassInfo*(*getClassInfo)(size_t index);
            const struct APluginClassInfo* const*(*getClassInfos)();

            char* pluginName;
            size_t pluginVersionMajor, pluginVersionMinor, pluginVersionPatch;
        };
    PRIVATE_APLUGINSDK_CLOSE_EXTERN_C
PRIVATE_APLUGINLIBRARY_CLOSE_NAMESPACE

#endif /* APLUGINSDK_PLUGININFOS_H */
