#ifndef APLUGINLIBRARY_PLUGININFOS_H
#define APLUGINLIBRARY_PLUGININFOS_H

#include "implementation/macros.h"

namespace apl {
    extern "C" {
        struct APLUGINLIBRARY_EXPORT PluginFeatureInfo
        {
            const char *featureGroup;
            const char *featureName;
            const char *returnType;
            const char *argumentList;
            void *functionPointer;
        };

        struct APLUGINLIBRARY_EXPORT PluginClassInfo
        {
            const char *interfaceName;
            const char *className;
            void *createInstance;
            void *deleteInstance;
        };
    }
}

#endif //APLUGINLIBRARY_PLUGININFOS_H
