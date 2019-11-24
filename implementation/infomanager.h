#ifndef APLUGINSDK_INFOMANAGER_H
#define APLUGINSDK_INFOMANAGER_H

#include <vector>
#include <string>
#include <cstdlib>

#include "../plugininfos.h"
#include "macros.h"

namespace apl
{
    namespace detail
    {
        class APLUGINSDK_NO_EXPORT InfoManager final
        {
        public:
            static InfoManager* instance();

            const PluginInfo* getPluginInfo() const;
            static const char* setPluginName(const char* name);
            static size_t setPluginVersion(size_t major, size_t minor, size_t patch);

            static PluginFeatureInfo *registerFeature(const char *featureGroup, const char *featureName,
                                                      const char *returnType, const char *parameterList,
                                                      void *functionPointer);
            size_t getFeatureCount() const;
            const PluginFeatureInfo *getFeatureInfo(size_t i) const;
            const PluginFeatureInfo *const *getFeatureInfos() const;

            static PluginClassInfo *registerClass(const char *interfaceClassName, const char *featureClassName,
                                                  void *createInstance, void *deleteInstance);
            size_t getClassCount() const;
            const PluginClassInfo *getClassInfo(size_t i) const;
            const PluginClassInfo *const *getClassInfos() const;

        private:
            InfoManager();
            ~InfoManager();

            std::vector<PluginFeatureInfo*> feature_infos;
            std::vector<PluginClassInfo*> class_infos;
            PluginInfo* pluginInfo;
        };

        APLUGINSDK_NO_EXPORT inline std::pair<char*, char*> splitParameterList(const char* parameterList);

        APLUGINSDK_NO_EXPORT inline void *allocateMemoryNotExported(size_t size);
        APLUGINSDK_NO_EXPORT inline void freeMemoryNotExported(void *ptr);

        APLUGINSDK_NO_EXPORT inline size_t getPluginFeatureCountNotExported();
        APLUGINSDK_NO_EXPORT inline const PluginFeatureInfo *getPluginFeatureInfoNotExported(size_t index);
        APLUGINSDK_NO_EXPORT inline const PluginFeatureInfo *const *getPluginFeatureInfosNotExported();

        APLUGINSDK_NO_EXPORT inline size_t getPluginClassCountNotExported();
        APLUGINSDK_NO_EXPORT inline const PluginClassInfo *getPluginClassInfoNotExported(size_t index);
        APLUGINSDK_NO_EXPORT inline const PluginClassInfo *const *getPluginClassInfosNotExported();
    }
}

#include "infomanager.ipp"

#endif //APLUGINSDK_INFOMANAGER_H
