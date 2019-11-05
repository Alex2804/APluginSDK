#ifdef A_PLUGIN_SDK_EXCLUDE_DEFINITION
# include "featuremanager.h"
#endif

#include <string>

namespace apl
{
    namespace detail
    {
        FeatureManager featureManagerInstance;

        std::pair<char*, char*> splitParameterList(const char* parameterList);
    }
}

std::pair<char*, char*> apl::detail::splitParameterList(const char* parameterList)
{
    std::string tmpParameterList(parameterList), typeString, tmpString, tmpTypesString, tmpNamesString;
    bool typeFront = true, firstRun = true;
    char last = ' ';
    tmpParameterList.push_back(','); // terminate with ',' to flush last type and name
    for(char current : tmpParameterList) {
        if(typeFront) {
            if(isalpha(current) || isdigit(current)) {
                typeString.push_back(current);
            } else if(current == ' ' && typeString == "const") {
                typeString.push_back(' ');
            } else if(current == '*' || current == '&') {
                typeString.push_back(current);
                typeFront = false;
            } else if(current == ' ' && last != ' ') {
                typeFront = false;
            }
        } else {
            if(isalpha(current) || isdigit(current)) {
                tmpString.push_back(current);
            } else if(current == '*' || current == '&') {
                if(!tmpString.empty() && typeString.back() != '*' && typeString.back() != '&') {
                    typeString.append(" ");
                }
                typeString.append(tmpString).push_back(current);
                tmpString.clear();
            } else if((current == ' ' || current == ',') && tmpString == "const") {
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


apl::detail::FeatureManager::~FeatureManager()
{
    for(PluginFeatureInfo* info : feature_infos) {
        delete[] info->parameterTypes;
        delete[] info->parameterNames;
        delete info;
    }
    for(PluginClassInfo* info : class_infos) {
        delete info;
    }
}

apl::PluginFeatureInfo* apl::detail::FeatureManager::registerFeature(const char *featureGroup, const char *featureName,
                                                                     const char *returnType, const char *parameterList,
                                                                     void *functionPointer)
{
    auto info = new PluginFeatureInfo();
    info->featureGroup = featureGroup;
    info->featureName = featureName;
    info->returnType = returnType;
    info->parameterList = parameterList;
    std::pair<char*, char*> pair = splitParameterList(parameterList);
    info->parameterTypes = pair.first;
    info->parameterNames = pair.second;
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
