#ifdef A_PLUGIN_SDK_EXCLUDE_DEFINITION
# include "featuremanager.h"
#endif

namespace apl
{
    namespace detail
    {
        FeatureManager featureManagerInstance;
    }
}

apl::detail::FeatureManager::~FeatureManager()
{
    for(PluginFeatureInfo* info : feature_infos) {
        delete info;
    }
    for(PluginClassInfo* info : class_infos) {
        delete info;
    }
}

apl::PluginFeatureInfo* apl::detail::FeatureManager::registerFeature(const char *featureGroup, const char *featureName,
                                                                     const char *returnType, const char *argumentList,
                                                                     void *functionPointer)
{
    auto info = new PluginFeatureInfo();
    info->featureGroup = featureGroup;
    info->featureName = featureName;
    info->returnType = returnType;
    info->argumentList = argumentList;
    info->functionPointer = functionPointer;
    featureManagerInstance.feature_infos.push_back(info);
    return info;
}
size_t apl::detail::FeatureManager::getFeatureCount()
{
    return featureManagerInstance.feature_infos.size();
}
const apl::PluginFeatureInfo* apl::detail::FeatureManager::getFeatureInfo(size_t i)
{
    if(i < featureManagerInstance.feature_infos.size())
        return featureManagerInstance.feature_infos[i];
    return nullptr;
}
const apl::PluginFeatureInfo* const* apl::detail::FeatureManager::getFeatureInfos()
{
    return featureManagerInstance.feature_infos.data();
}

apl::PluginClassInfo* apl::detail::FeatureManager::registerClass(const char *interfaceClassName,
                                                                 const char *featureClassName,
                                                                 void* createInstance, void* deleteInstance)
{
    auto info = new PluginClassInfo();
    info->interfaceName = interfaceClassName;
    info->className = featureClassName;
    info->createInstance = createInstance;
    info->deleteInstance = deleteInstance;
    featureManagerInstance.class_infos.push_back(info);
    return info;
}
size_t apl::detail::FeatureManager::getClassCount()
{
    return featureManagerInstance.class_infos.size();
}
const apl::PluginClassInfo* apl::detail::FeatureManager::getClassInfo(size_t i)
{
    if(i < featureManagerInstance.class_infos.size())
        return featureManagerInstance.class_infos.at(i);
    return nullptr;
}
const apl::PluginClassInfo* const* apl::detail::FeatureManager::getClassInfos()
{
    return featureManagerInstance.class_infos.data();
}
