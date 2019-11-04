#ifndef APLUGINLIBRARY_FEATUREMANAGER_H
#define APLUGINLIBRARY_FEATUREMANAGER_H

#include <vector>

#include "../plugininfos.h"
#include "macros.h"

namespace apl
{
    namespace detail
    {
        class APLUGINLIBRARY_NO_EXPORT FeatureManager final
        {
        public:
            ~FeatureManager();

            static PluginFeatureInfo *registerFeature(const char *featureGroup, const char *featureName,
                                                      const char *returnType, const char *argumentList,
                                                      void *functionPointer);
            static size_t getFeatureCount();
            static const PluginFeatureInfo *getFeatureInfo(size_t i);
            static const PluginFeatureInfo *const *getFeatureInfos();

            static PluginClassInfo *registerClass(const char *interfaceClassName, const char *featureClassName,
                                                  void *createInstance, void *deleteInstance);
            static size_t getClassCount();
            static const PluginClassInfo *getClassInfo(size_t i);
            static const PluginClassInfo *const *getClassInfos();

        private:
            std::vector<PluginFeatureInfo*> feature_infos;
            std::vector<PluginClassInfo*> class_infos;
        };
    }
}

#ifndef A_PLUGIN_SDK_EXCLUDE_DEFINITION
# include "featuremanager.cpp"
#endif

#endif //APLUGINLIBRARY_FEATUREMANAGER_H
