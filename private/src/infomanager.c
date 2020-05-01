#include "../infomanager.h"

#if APLUGINSDK_EXCLUDE_IMPLEMENTATION
#   include "../../pluginapi.h"
#endif

#include <ctype.h>
#include <stdlib.h>

#include "../privateplugininfos.h"

#undef ACUTILS_SYMBOL_ATTRIBUTES
#define ACUTILS_SYMBOL_ATTRIBUTES APLUGINSDK_NO_EXPORT
#include "../../libs/ACUtils/src/dynarray.c"

#ifdef __cplusplus
#   define PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT APLUGINSDK_NO_EXPORT
#else
#   define PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT
#endif

PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE

    A_DYNAMIC_ARRAY_DEFINITION(PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT APluginSDKFeatureInfoDynArray, struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo*);
    A_DYNAMIC_ARRAY_DEFINITION(PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT APluginSDKClassInfoDynArray, struct APLUGINLIBRARY_NAMESPACE APluginClassInfo*);
    A_DYNAMIC_ARRAY_DEFINITION(PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT APluginSDKCString, char);

    struct _private_APluginSDK_InfoManager;
    static void _private_APluginSDK_releaseInfoManager(struct _private_APluginSDK_InfoManager*);
    static struct _private_APluginSDK_InfoManager* _private_APluginSDK_initInfoManager(struct _private_APluginSDK_InfoManager*);
    static struct APLUGINLIBRARY_NAMESPACE APluginInfo* _private_APluginSDK_constructPluginInfo(void);
    static void _private_APluginSDK_destructPluginInfo(struct APLUGINLIBRARY_NAMESPACE APluginInfo*);

    static char** _private_APluginSDK_splitParameterList(const char *parameterList);

    struct PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT _private_APluginSDK_InfoManager {
        struct APLUGINLIBRARY_NAMESPACE APluginInfo *pluginInfo;
        struct APluginSDKFeatureInfoDynArray *featureInfos;
        struct APluginSDKClassInfoDynArray *classInfos;

#ifdef __cplusplus
        _private_APluginSDK_InfoManager() {
            _private_APluginSDK_initInfoManager(this);
        }
        ~_private_APluginSDK_InfoManager() {
            _private_APluginSDK_releaseInfoManager(this);
        }
#endif
    };

    static void _private_APluginSDK_releaseInfoManager(struct _private_APluginSDK_InfoManager *infoManager)
    {
        size_t i;
        if(infoManager == NULL)
            return;
        _private_APluginSDK_destructPluginInfo(infoManager->pluginInfo);
        for(i = 0; i < aDynArraySize(infoManager->featureInfos); ++i) {
            struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* featureInfo = aDynArrayGet(infoManager->featureInfos, i);
            free(featureInfo->parameterTypes);
            free(featureInfo->parameterNames);
            free(featureInfo);
        }
        aDynArrayDestruct(infoManager->featureInfos);
        for(i = 0; i < aDynArraySize(infoManager->classInfos); ++i)
            free(aDynArrayGet(infoManager->classInfos, i));
        aDynArrayDestruct(infoManager->classInfos);
    }
    static void _private_APluginSDK_destructInfoManager(struct _private_APluginSDK_InfoManager *infoManager)
    {
        _private_APluginSDK_releaseInfoManager(infoManager);
        free(infoManager);
    }
    static struct _private_APluginSDK_InfoManager* _private_APluginSDK_initInfoManager(
            struct _private_APluginSDK_InfoManager *infoManager)
    {
        if(infoManager == NULL)
            infoManager = (struct _private_APluginSDK_InfoManager*) malloc(sizeof(struct _private_APluginSDK_InfoManager));
        if(infoManager != NULL) {
            infoManager->pluginInfo = _private_APluginSDK_constructPluginInfo();
            infoManager->featureInfos = aDynArrayConstruct(struct APluginSDKFeatureInfoDynArray);
            infoManager->classInfos = aDynArrayConstruct(struct APluginSDKClassInfoDynArray);
            if(infoManager->pluginInfo == NULL
               || infoManager->featureInfos == NULL
               || infoManager->classInfos == NULL)
            {
                _private_APluginSDK_destructInfoManager(infoManager);
                return NULL;
            }
        }
        return infoManager;
    }
    static struct _private_APluginSDK_InfoManager* _private_APluginSDK_constructInfoManager(void)
    {
        return _private_APluginSDK_initInfoManager(NULL);
    }

#ifndef __cplusplus
    static struct _private_APluginSDK_InfoManager *_private_APluginSDK_infoManagerInstance = NULL;
#endif
    static size_t _private_APluginSDK_pluginRefCount = 0;

    static size_t _private_APluginSDK_constructPluginInternals(void(*initPlugin)(void))
    {
        if(_private_APluginSDK_pluginRefCount++ == 0 && initPlugin != NULL)
            initPlugin();
        return _private_APluginSDK_pluginRefCount;
    }
    static size_t _private_APluginSDK_destructPluginInternals(void(*finiPlugin)(void))
    {
        if(--_private_APluginSDK_pluginRefCount == 0) {
            if(finiPlugin != NULL)
                finiPlugin();
#ifndef __cplusplus
            _private_APluginSDK_destructInfoManager(_private_APluginSDK_infoManagerInstance);
            _private_APluginSDK_infoManagerInstance = NULL;
#endif
        }
        return _private_APluginSDK_pluginRefCount;
    }

    static struct _private_APluginSDK_InfoManager* _private_APluginSDK_getInfoManagerInstance(void)
    {
#ifdef __cplusplus
        static struct _private_APluginSDK_InfoManager infoManager;
        return &infoManager;
#else
        if(_private_APluginSDK_infoManagerInstance == NULL)
            _private_APluginSDK_infoManagerInstance = _private_APluginSDK_constructInfoManager();
        return _private_APluginSDK_infoManagerInstance;
#endif
    }

#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
    void* _private_APluginSDK_initAPluginFunctionPtr = NULL;
    bool _private_APluginSDK_registerInitAPluginFunction(void *functionPtr)
    {
        _private_APluginSDK_initAPluginFunctionPtr = functionPtr;
        return true;
    }

    void* _private_APluginSDK_finiAPluginFunctionPtr = NULL;
    bool _private_APluginSDK_registerFiniAPluginFunction(void *functionPtr)
    {
        _private_APluginSDK_finiAPluginFunctionPtr = functionPtr;
        return true;
    }
#endif

    const struct APLUGINLIBRARY_NAMESPACE APluginInfo* _private_APluginSDK_getPluginInfo(void)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return NULL;
        return infoManager->pluginInfo;
    }

    bool _private_APluginSDK_setPluginName(const char *name)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        size_t nameLength = strlen(name);
        if(infoManager == NULL)
            return false;
        free(infoManager->pluginInfo->pluginName);
        if(nameLength >= 2 && name[0] == '"' && name[nameLength - 1] == '"') {
            infoManager->pluginInfo->pluginName = (char*) malloc(sizeof(char) * (nameLength - 1));
            memcpy(infoManager->pluginInfo->pluginName, name + 1, sizeof(char) * (nameLength - 2));
            infoManager->pluginInfo->pluginName[nameLength - 2] = '\0';
        } else {
            infoManager->pluginInfo->pluginName = (char*) malloc(sizeof(char) * nameLength + 1);
            memcpy(infoManager->pluginInfo->pluginName, name, sizeof(char) * nameLength);
            infoManager->pluginInfo->pluginName[nameLength] = '\0';
        }
        return true;
    }
    bool _private_APluginSDK_setPluginVersion(size_t major, size_t minor, size_t patch)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return false;
        infoManager->pluginInfo->pluginVersionMajor = major;
        infoManager->pluginInfo->pluginVersionMinor = minor;
        infoManager->pluginInfo->pluginVersionPatch = patch;
        return true;
    }

    bool _private_APluginSDK_registerFeature(const char* featureGroup, const char* featureName, const char* returnType,
                                             const char* parameterList, void* functionPtr)
    {
        struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* info;
        char **splittedParameterList;
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return false;
        info = (struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo*) malloc(sizeof(struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo));
        if(info != NULL) {
            info->pluginInfo = infoManager->pluginInfo;
            info->featureGroup = featureGroup;
            info->featureName = featureName;
            info->returnType = returnType;
            info->parameterList = parameterList;
            splittedParameterList = _private_APluginSDK_splitParameterList(parameterList);
            info->parameterTypes = splittedParameterList[0];
            info->parameterNames = splittedParameterList[1];
            free(splittedParameterList);
            info->functionPointer = functionPtr;
            aDynArrayAdd(infoManager->featureInfos, info);
        }
        return info != NULL;
    }
    bool _private_APluginSDK_registerClass(const char* interfaceClassName, const char* featureClassName,
                                           void* createInstance, void* deleteInstance)
    {
        struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* info;
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return false;
        info = (struct APLUGINLIBRARY_NAMESPACE APluginClassInfo*) malloc(sizeof(struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo));
        if(info == NULL)
            return false;
        info->pluginInfo = infoManager->pluginInfo;
        info->interfaceName = interfaceClassName;
        info->className = featureClassName;
        info->createInstance = createInstance;
        info->deleteInstance = deleteInstance;
        aDynArrayAdd(infoManager->classInfos, info);
        return true;
    }

    static size_t _private_APluginSDK_getFeatureCount(void)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return 0;
        return aDynArraySize(infoManager->featureInfos);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* _private_APluginSDK_getFeatureInfo(size_t index)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || index >= aDynArraySize(infoManager->featureInfos))
            return NULL;
        return aDynArrayGet(infoManager->featureInfos, index);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* const* _private_APluginSDK_getFeatureInfos(void)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || infoManager->featureInfos == NULL)
            return NULL;
        return (const struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* const*) infoManager->featureInfos->buffer;
    }

    static size_t _private_APluginSDK_getClassCount(void)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return 0;
        return aDynArraySize(infoManager->classInfos);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* _private_APluginSDK_getClassInfo(size_t index)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || index >= aDynArraySize(infoManager->classInfos))
            return NULL;
        return aDynArrayGet(infoManager->classInfos, index);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* const* _private_APluginSDK_getClassInfos(void)
    {
        struct _private_APluginSDK_InfoManager* infoManager = _private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || infoManager->classInfos == NULL)
            return NULL;
        return (const struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* const*) infoManager->classInfos->buffer;
    }

    static struct APrivatePluginInfo* _private_APluginSDK_constructPrivatePluginInfo(void)
    {
        struct APrivatePluginInfo *info = (struct APrivatePluginInfo*) malloc(sizeof(struct APrivatePluginInfo));
        info->constructPluginInternals = _private_APluginSDK_constructPluginInternals;
        info->destructPluginInternals = _private_APluginSDK_destructPluginInternals;
        return info;
    }
    static void _private_APluginSDK_destructPrivatePluginInfo(struct APrivatePluginInfo *info)
    {
        free(info);
    }

    static struct APLUGINLIBRARY_NAMESPACE APluginInfo* _private_APluginSDK_constructPluginInfo(void)
    {
        struct APLUGINLIBRARY_NAMESPACE APluginInfo* info = (struct APLUGINLIBRARY_NAMESPACE APluginInfo*) malloc(sizeof(struct APLUGINLIBRARY_NAMESPACE APluginInfo));
        info->privateInfo = _private_APluginSDK_constructPrivatePluginInfo();
        info->apiVersionMajor = APLUGINSDK_API_VERSION_MAJOR;
        info->apiVersionMinor = APLUGINSDK_API_VERSION_MINOR;
        info->apiVersionPatch = APLUGINSDK_API_VERSION_PATCH;
        info->allocateMemory = APLUGINLIBRARY_NAMESPACE APluginSDK_malloc;
        info->freeMemory = APLUGINLIBRARY_NAMESPACE APluginSDK_free;
        info->pluginLanguage = PRIVATE_APLUGINSDK_PLUGIN_LANGUAGE;
        info->pluginName = (char*) malloc(sizeof(char));
        info->pluginName[0] = '\0';
        info->pluginVersionMajor = info->pluginVersionMinor = info->pluginVersionPatch = 0;
        info->getFeatureCount = _private_APluginSDK_getFeatureCount;
        info->getFeatureInfo = _private_APluginSDK_getFeatureInfo;
        info->getFeatureInfos = _private_APluginSDK_getFeatureInfos;
        info->getClassCount = _private_APluginSDK_getClassCount;
        info->getClassInfo = _private_APluginSDK_getClassInfo;
        info->getClassInfos = _private_APluginSDK_getClassInfos;
        return info;
    }
    static void _private_APluginSDK_destructPluginInfo(struct APLUGINLIBRARY_NAMESPACE APluginInfo* info)
    {
        _private_APluginSDK_destructPrivatePluginInfo(info->privateInfo);
        free(info->pluginName);
        free(info);
    }

/* ===================================== _private_APluginSDK_splitParameterList ===================================== */

    static bool _private_streq(struct APluginSDKCString *dynArray, const char *str)
    {
        size_t dynArraySize;
        bool ret;
        if(str == NULL || dynArray == NULL)
            return false;
        dynArraySize = aDynArraySize(dynArray);
        if(dynArraySize == 0 || aDynArrayGet(dynArray, dynArraySize - 1) != '\0') {
            if(!aDynArrayAdd(dynArray, '\0'))
                return false;
        }
        ret = (strcmp(aDynArrayBuffer(dynArray), str) == 0);
        aDynArrayRemove(dynArray, dynArraySize, 1);
        return ret;
    }

    static char** _private_APluginSDK_splitParameterList(const char* parameterList)
    {
        struct APluginSDKCString *tmpParameterList, *typeString, *tmpString, *tmpTypesString, *tmpNamesString;
        size_t tmp, parameterListLength = strlen(parameterList);
        char** returnArray;
        tmpParameterList = aDynArrayConstruct(struct APluginSDKCString);
        typeString = aDynArrayConstruct(struct APluginSDKCString);
        tmpString = aDynArrayConstruct(struct APluginSDKCString);
        tmpTypesString = aDynArrayConstruct(struct APluginSDKCString);
        tmpNamesString = aDynArrayConstruct(struct APluginSDKCString);
        aDynArrayAddArray(tmpParameterList, parameterList, parameterListLength);
        bool typeFront = true, firstRun = true;
        char last = ' ';
        aDynArrayAdd(tmpParameterList, ','); /* terminate with ',' to flush last type and name */
        for(tmp = 0; tmp < parameterListLength + 1; ++tmp) {
            char current = aDynArrayGet(tmpParameterList, tmp);
            if(typeFront) {
                if(current != ',' && (isalnum(current) || ispunct(current))) {
                    aDynArrayAdd(typeString, current);
                } else if(isspace(current) && _private_streq(typeString, "const")) {
                    aDynArrayAdd(typeString, ' ');
                } else if(current == '*' || current == '&') {
                    aDynArrayAdd(typeString, current);
                    typeFront = false;
                } else if(isspace(current) && !isspace(last) && aDynArraySize(typeString) != 0) {
                    typeFront = false;
                }
            } else {
                if(current != ',' && (isalnum(current) || ispunct(current))) {
                    aDynArrayAdd(tmpString, current);
                } else if(current == '*' || current == '&') {
                    char typeStringLastChar = aDynArrayGet(typeString, aDynArraySize(typeString) - 1);
                    if(aDynArraySize(tmpString) != 0 && typeStringLastChar != '*' && typeStringLastChar != '&') {
                        aDynArrayAdd(typeString, ' ');
                    }
                    aDynArrayAddDynArray(typeString, tmpString);
                    aDynArrayAdd(typeString, current);
                    aDynArrayClear(tmpString);
                } else if((isspace(current) || current == ',') && _private_streq(tmpString, "const")) {
                    aDynArrayAddDynArray(typeString, tmpString);
                    aDynArrayClear(tmpString);
                }
                if(current == ',') {
                    if(!firstRun) {
                        aDynArrayAddArray(tmpTypesString, ", ", 2);
                        aDynArrayAddArray(tmpNamesString, ", ", 2);
                    }
                    aDynArrayAddDynArray(tmpTypesString, typeString);
                    aDynArrayAddDynArray(tmpNamesString, tmpString);
                    aDynArrayClear(typeString);
                    aDynArrayClear(tmpString);
                    typeFront = true;
                    firstRun = false;
                    current = ' '; /* 'last' should be ' ' */
                }
            }
            last = current;
        }
        tmp = aDynArraySize(tmpTypesString);
        char* types = (char*) malloc(sizeof(char) * (tmp + 1));
        memcpy(types, tmpTypesString->buffer, sizeof(char) * tmp);
        types[tmp] = '\0';

        tmp = aDynArraySize(tmpNamesString);
        char* names = (char*) malloc(sizeof(char) * (tmp + 1));
        memcpy(names, tmpNamesString->buffer, sizeof(char) * tmp);
        names[tmp] = '\0';

        aDynArrayDestruct(tmpParameterList);
        aDynArrayDestruct(typeString);
        aDynArrayDestruct(tmpString);
        aDynArrayDestruct(tmpTypesString);
        aDynArrayDestruct(tmpNamesString);

        returnArray = (char**) malloc(sizeof(char*) * 2);
        returnArray[0] = types;
        returnArray[1] = names;
        return returnArray;
    }

PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE
