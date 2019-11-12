#ifndef APLUGINSDK_PLUGININFOS_H
#define APLUGINSDK_PLUGININFOS_H

#include "implementation/macros.h"

namespace apl {
    extern "C" {
        struct APLUGINSDK_EXPORT PluginFeatureInfo
        {
            const char* featureGroup;
            const char* featureName;
            const char* returnType;
            const char* parameterList;
            char* parameterTypes;
            char* parameterNames;
            void* functionPointer;
        };

        struct APLUGINSDK_EXPORT PluginClassInfo
        {
            const char* interfaceName;
            const char* className;
            void* createInstance;
            void* deleteInstance;
        };
    }
}

#endif //APLUGINSDK_PLUGININFOS_H
