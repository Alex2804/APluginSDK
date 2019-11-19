#ifdef A_PLUGIN_SDK_EXCLUDE_DEFINITIONS
#include "infomanager.h"
# include "../pluginapi.h"
#endif

#include <string>
#include <cstdlib>

apl::detail::InfoManager::InfoManager()
    : pluginInfo(new PluginInfo())
{
    pluginInfo->pluginName = "";

    pluginInfo->apiVersionMajor = A_PLUGIN_API_VERSION_MAJOR;
    pluginInfo->apiVersionMinor = A_PLUGIN_API_VERSION_MINOR;
    pluginInfo->apiVersionPatch = A_PLUGIN_API_VERSION_PATCH;

    pluginInfo->pluginVersionMajor = 0;
    pluginInfo->pluginVersionMinor = 0;
    pluginInfo->pluginVersionPatch = 0;

    pluginInfo->allocateMemory = apl::detail::allocateMemoryNotExported;
    pluginInfo->freeMemory = apl::detail::freeMemoryNotExported;

    pluginInfo->getPluginFeatureCount = apl::detail::getPluginFeatureCountNotExported;
    pluginInfo->getPluginFeatureInfo = apl::detail::getPluginFeatureInfoNotExported;
    pluginInfo->getPluginFeatureInfos = apl::detail::getPluginFeatureInfosNotExported;

    pluginInfo->getPluginClassCount = apl::detail::getPluginClassCountNotExported;
    pluginInfo->getPluginClassInfo = apl::detail::getPluginClassInfoNotExported;
    pluginInfo->getPluginClassInfos = apl::detail::getPluginClassInfosNotExported;
}
apl::detail::InfoManager::~InfoManager()
{
    for(PluginFeatureInfo* info : feature_infos) {
        delete[] info->parameterTypes;
        delete[] info->parameterNames;
        delete info;
    }
    for(PluginClassInfo* info : class_infos) {
        delete info;
    }
    delete pluginInfo;
}

apl::detail::InfoManager* apl::detail::InfoManager::instance()
{
    static InfoManager instance;
    return &instance;
}

const apl::PluginInfo* apl::detail::InfoManager::getPluginInfo() const
{
    return pluginInfo;
}
const char* apl::detail::InfoManager::setPluginName(const char* name)
{
    instance()->pluginInfo->pluginName = name;
    return name;
}
size_t apl::detail::InfoManager::setPluginVersion(size_t major, size_t minor, size_t patch)
{
    instance()->pluginInfo->pluginVersionMajor = major;
    instance()->pluginInfo->pluginVersionMinor = minor;
    instance()->pluginInfo->pluginVersionPatch = patch;
    return major;
}

apl::PluginFeatureInfo* apl::detail::InfoManager::registerFeature(const char *featureGroup, const char *featureName,
                                                                  const char *returnType, const char *parameterList,
                                                                  void *functionPointer)
{
    auto info = new PluginFeatureInfo();
    info->pluginInfo = instance()->pluginInfo;
    info->featureGroup = featureGroup;
    info->featureName = featureName;
    info->returnType = returnType;
    info->parameterList = parameterList;
    std::pair<char*, char*> pair = splitParameterList(parameterList);
    info->parameterTypes = pair.first;
    info->parameterNames = pair.second;
    info->functionPointer = functionPointer;
    instance()->feature_infos.push_back(info);
    return info;
}
size_t apl::detail::InfoManager::getFeatureCount() const
{
    return feature_infos.size();
}
const apl::PluginFeatureInfo* apl::detail::InfoManager::getFeatureInfo(size_t i) const
{
    if(i < feature_infos.size())
        return feature_infos[i];
    return nullptr;
}
const apl::PluginFeatureInfo* const* apl::detail::InfoManager::getFeatureInfos() const
{
    return feature_infos.data();
}

apl::PluginClassInfo* apl::detail::InfoManager::registerClass(const char *interfaceClassName,
                                                              const char *featureClassName,
                                                              void* createInstance, void* deleteInstance)
{
    auto info = new PluginClassInfo();
    info->pluginInfo = instance()->pluginInfo;
    info->interfaceName = interfaceClassName;
    info->className = featureClassName;
    info->createInstance = createInstance;
    info->deleteInstance = deleteInstance;
    instance()->class_infos.push_back(info);
    return info;
}
size_t apl::detail::InfoManager::getClassCount() const
{
    return class_infos.size();
}
const apl::PluginClassInfo* apl::detail::InfoManager::getClassInfo(size_t i) const
{
    if(i < class_infos.size())
        return class_infos.at(i);
    return nullptr;
}
const apl::PluginClassInfo* const* apl::detail::InfoManager::getClassInfos() const
{
    return class_infos.data();
}

// =============================================== splitParameterList =============================================== //

std::pair<char*, char*> apl::detail::splitParameterList(const char* parameterList)
{
    std::string tmpParameterList(parameterList), typeString, tmpString, tmpTypesString, tmpNamesString;
    bool typeFront = true, firstRun = true;
    char last = ' ';
    tmpParameterList.push_back(','); // terminate with ',' to flush last type and name
    for(char current : tmpParameterList) {
        if(typeFront) {
            if(current != ',' && (std::isalnum(current) || std::ispunct(current))) {
                typeString.push_back(current);
            } else if(std::isspace(current) && typeString == "const") {
                typeString.push_back(' ');
            } else if(current == '*' || current == '&') {
                typeString.push_back(current);
                typeFront = false;
            } else if(std::isspace(current) && !std::isspace(last)) {
                typeFront = false;
            }
        } else {
            if(current != ',' && (std::isalnum(current) || std::ispunct(current))) {
                tmpString.push_back(current);
            } else if(current == '*' || current == '&') {
                if(!tmpString.empty() && typeString.back() != '*' && typeString.back() != '&') {
                    typeString.append(" ");
                }
                typeString.append(tmpString).push_back(current);
                tmpString.clear();
            } else if((std::isspace(current) || current == ',') && tmpString == "const") {
                typeString.append(tmpString);
                tmpString.clear();
            }
            if(current == ',') {
                tmpTypesString.append(firstRun ? "" : ", ").append(typeString);
                tmpNamesString.append(firstRun ? "" : ", ").append(tmpString);
                typeString.clear();
                tmpString.clear();
                typeFront = true;
                firstRun = false;
                current = ' '; // 'last' should be ' '
            }
        }
        last = current;
    }
    char* types = new char[tmpTypesString.size() + 1];
    char* names = new char[tmpNamesString.size() + 1];
    types[tmpTypesString.size()] = '\0';
    names[tmpNamesString.size()] = '\0';
    tmpTypesString.copy(types, std::string::npos);
    tmpNamesString.copy(names, std::string::npos);
    return {types, names};
}

// ============================================= not exported functions ============================================= //

void* apl::detail::allocateMemoryNotExported(size_t size)
{
    return std::malloc(size);
}
void apl::detail::freeMemoryNotExported(void *ptr)
{
    std::free(ptr);
}

size_t apl::detail::getPluginFeatureCountNotExported()
{
    return InfoManager::instance()->getFeatureCount();
}
const apl::PluginFeatureInfo* apl::detail::getPluginFeatureInfoNotExported(size_t index)
{
    return InfoManager::instance()->getFeatureInfo(index);
}
const apl::PluginFeatureInfo * const* apl::detail::getPluginFeatureInfosNotExported()
{
    return InfoManager::instance()->getFeatureInfos();
}

size_t apl::detail::getPluginClassCountNotExported()
{
    return InfoManager::instance()->getClassCount();
}
const apl::PluginClassInfo* apl::detail::getPluginClassInfoNotExported(size_t index)
{
    return InfoManager::instance()->getClassInfo(index);
}
const apl::PluginClassInfo* const* apl::detail::getPluginClassInfosNotExported()
{
    return InfoManager::instance()->getClassInfos();
}
