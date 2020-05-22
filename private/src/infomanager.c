#include "../infomanager.h"

#if APLUGINSDK_EXCLUDE_IMPLEMENTATION
#   include "../../pluginapi.h"
#endif

#include <stdlib.h>

#ifndef ACUTILS_ONE_SOURCE
#   define ACUTILS_ONE_SOURCE
#endif
#include "../../libs/ACUtils/include/ACUtils/adynarray.h"

#include "../privateplugininfos.h"

#ifdef __cplusplus
#   define PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT APLUGINSDK_NO_EXPORT
#else
#   define PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT
#endif

PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE

    A_DYNAMIC_ARRAY_DEFINITION(PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT private_APluginSDK_APluginFeatureInfo_DynArray, struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo*);
    A_DYNAMIC_ARRAY_DEFINITION(PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT private_APluginSDK_APluginClassInfo_DynArray, struct APLUGINLIBRARY_NAMESPACE APluginClassInfo*);

    struct private_APluginSDK_InfoManager;
    static void private_APluginSDK_releaseInfoManager(struct private_APluginSDK_InfoManager*);
    static struct private_APluginSDK_InfoManager* private_APluginSDK_initInfoManager(struct private_APluginSDK_InfoManager*);
    static struct APLUGINLIBRARY_NAMESPACE APluginInfo* private_APluginSDK_constructPluginInfo(void);
    static void private_APluginSDK_destructPluginInfo(struct APLUGINLIBRARY_NAMESPACE APluginInfo*);

    struct PRIVATE_APLUGINSDK_STRUCT_NO_EXPORT private_APluginSDK_InfoManager {
        struct APLUGINLIBRARY_NAMESPACE APluginInfo *pluginInfo;
        struct private_APluginSDK_APluginFeatureInfo_DynArray *featureInfos;
        struct private_APluginSDK_APluginClassInfo_DynArray *classInfos;

#ifdef __cplusplus
        private_APluginSDK_InfoManager() {
            private_APluginSDK_initInfoManager(this);
        }
        ~private_APluginSDK_InfoManager() {
            private_APluginSDK_releaseInfoManager(this);
        }
#endif
    };

    static void private_APluginSDK_releaseInfoManager(struct private_APluginSDK_InfoManager *infoManager)
    {
        size_t i;
        if(infoManager == NULL)
            return;
        private_APluginSDK_destructPluginInfo(infoManager->pluginInfo);
        for(i = 0; i < ADynArray_size(infoManager->featureInfos); ++i)
            free(ADynArray_get(infoManager->featureInfos, i));
        ADynArray_destruct(infoManager->featureInfos);
        for(i = 0; i < ADynArray_size(infoManager->classInfos); ++i)
            free(ADynArray_get(infoManager->classInfos, i));
        ADynArray_destruct(infoManager->classInfos);
    }
    static void private_APluginSDK_destructInfoManager(struct private_APluginSDK_InfoManager *infoManager)
    {
        private_APluginSDK_releaseInfoManager(infoManager);
        free(infoManager);
    }
    static struct private_APluginSDK_InfoManager* private_APluginSDK_initInfoManager(
            struct private_APluginSDK_InfoManager *infoManager)
    {
        if(infoManager == NULL)
            infoManager = (struct private_APluginSDK_InfoManager*) malloc(sizeof(struct private_APluginSDK_InfoManager));
        if(infoManager != NULL) {
            infoManager->pluginInfo = private_APluginSDK_constructPluginInfo();
            infoManager->featureInfos = ADynArray_construct(struct private_APluginSDK_APluginFeatureInfo_DynArray);
            infoManager->classInfos = ADynArray_construct(struct private_APluginSDK_APluginClassInfo_DynArray);
            if(infoManager->pluginInfo == NULL
               || infoManager->featureInfos == NULL
               || infoManager->classInfos == NULL)
            {
                private_APluginSDK_destructInfoManager(infoManager);
                return NULL;
            }
        }
        return infoManager;
    }
    static struct private_APluginSDK_InfoManager* private_APluginSDK_constructInfoManager(void)
    {
        return private_APluginSDK_initInfoManager(NULL);
    }

#ifndef __cplusplus
    static struct private_APluginSDK_InfoManager *private_APluginSDK_infoManagerInstance = NULL;
#endif
    static size_t private_APluginSDK_pluginRefCount = 0;

    static size_t private_APluginSDK_constructPluginInternals(void(*initPlugin)(void))
    {
        if(private_APluginSDK_pluginRefCount++ == 0 && initPlugin != NULL)
            initPlugin();
        return private_APluginSDK_pluginRefCount;
    }
    static size_t private_APluginSDK_destructPluginInternals(void(*finiPlugin)(void))
    {
        if(--private_APluginSDK_pluginRefCount == 0) {
            if(finiPlugin != NULL)
                finiPlugin();
#ifndef __cplusplus
            private_APluginSDK_destructInfoManager(private_APluginSDK_infoManagerInstance);
            private_APluginSDK_infoManagerInstance = NULL;
#endif
        }
        return private_APluginSDK_pluginRefCount;
    }

    static struct private_APluginSDK_InfoManager* private_APluginSDK_getInfoManagerInstance(void)
    {
#ifdef __cplusplus
        static struct private_APluginSDK_InfoManager infoManager;
        return &infoManager;
#else
        if(private_APluginSDK_infoManagerInstance == NULL)
            private_APluginSDK_infoManagerInstance = private_APluginSDK_constructInfoManager();
        return private_APluginSDK_infoManagerInstance;
#endif
    }

#if PRIVATE_APLUGINSDK_INTEGRATED_PLUGIN
    void* private_APluginSDK_initAPluginFunctionPtr = NULL;
    bool private_APluginSDK_registerInitAPluginFunction(void *functionPtr)
    {
        private_APluginSDK_initAPluginFunctionPtr = functionPtr;
        return true;
    }

    void* private_APluginSDK_finiAPluginFunctionPtr = NULL;
    bool private_APluginSDK_registerFiniAPluginFunction(void *functionPtr)
    {
        private_APluginSDK_finiAPluginFunctionPtr = functionPtr;
        return true;
    }
#endif

    const struct APLUGINLIBRARY_NAMESPACE APluginInfo* private_APluginSDK_getPluginInfo(void)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return NULL;
        return infoManager->pluginInfo;
    }

    bool private_APluginSDK_setPluginName(const char *name)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
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
    bool private_APluginSDK_setPluginVersion(size_t major, size_t minor, size_t patch)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return false;
        infoManager->pluginInfo->pluginVersionMajor = major;
        infoManager->pluginInfo->pluginVersionMinor = minor;
        infoManager->pluginInfo->pluginVersionPatch = patch;
        return true;
    }

    bool private_APluginSDK_registerFeature(const char* featureGroup, const char* featureName, const char* returnType,
                                            const char* parameterList, void* functionPtr)
    {
        struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* info;
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return false;
        info = (struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo*) malloc(sizeof(struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo));
        if(info != NULL) {
            info->pluginInfo = infoManager->pluginInfo;
            info->featureGroup = featureGroup;
            info->featureName = featureName;
            info->returnType = returnType;
            info->parameterList = parameterList;
            info->functionPointer = functionPtr;
            ADynArray_append(infoManager->featureInfos, info);
        }
        return info != NULL;
    }
    bool private_APluginSDK_registerClass(const char* interfaceClassName, const char* featureClassName,
                                          void* createInstance, void* deleteInstance)
    {
        struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* info;
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
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
        ADynArray_append(infoManager->classInfos, info);
        return true;
    }

    static size_t private_APluginSDK_getFeatureCount(void)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return 0;
        return ADynArray_size(infoManager->featureInfos);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* private_APluginSDK_getFeatureInfo(size_t index)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || index >= ADynArray_size(infoManager->featureInfos))
            return NULL;
        return ADynArray_get(infoManager->featureInfos, index);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* const* private_APluginSDK_getFeatureInfos(void)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || infoManager->featureInfos == NULL)
            return NULL;
        return (const struct APLUGINLIBRARY_NAMESPACE APluginFeatureInfo* const*) infoManager->featureInfos->buffer;
    }

    static size_t private_APluginSDK_getClassCount(void)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL)
            return 0;
        return ADynArray_size(infoManager->classInfos);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* private_APluginSDK_getClassInfo(size_t index)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || index >= ADynArray_size(infoManager->classInfos))
            return NULL;
        return ADynArray_get(infoManager->classInfos, index);
    }
    static const struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* const* private_APluginSDK_getClassInfos(void)
    {
        struct private_APluginSDK_InfoManager* infoManager = private_APluginSDK_getInfoManagerInstance();
        if(infoManager == NULL || infoManager->classInfos == NULL)
            return NULL;
        return (const struct APLUGINLIBRARY_NAMESPACE APluginClassInfo* const*) infoManager->classInfos->buffer;
    }

    static struct APrivatePluginInfo* private_APluginSDK_constructPrivatePluginInfo(void)
    {
        struct APrivatePluginInfo *info = (struct APrivatePluginInfo*) malloc(sizeof(struct APrivatePluginInfo));
        info->constructPluginInternals = private_APluginSDK_constructPluginInternals;
        info->destructPluginInternals = private_APluginSDK_destructPluginInternals;
        return info;
    }
    static void private_APluginSDK_destructPrivatePluginInfo(struct APrivatePluginInfo *info)
    {
        free(info);
    }

static struct APLUGINLIBRARY_NAMESPACE APluginInfo* private_APluginSDK_constructPluginInfo(void)
    {
        struct APLUGINLIBRARY_NAMESPACE APluginInfo* info = (struct APLUGINLIBRARY_NAMESPACE APluginInfo*) malloc(sizeof(struct APLUGINLIBRARY_NAMESPACE APluginInfo));
        info->privateInfo = private_APluginSDK_constructPrivatePluginInfo();
        info->apiVersionMajor = APLUGINSDK_API_VERSION_MAJOR;
        info->apiVersionMinor = APLUGINSDK_API_VERSION_MINOR;
        info->apiVersionPatch = APLUGINSDK_API_VERSION_PATCH;
        info->allocateMemory = APLUGINLIBRARY_NAMESPACE APluginSDK_malloc;
        info->freeMemory = APLUGINLIBRARY_NAMESPACE APluginSDK_free;
        info->pluginLanguage = PRIVATE_APLUGINSDK_PLUGIN_LANGUAGE;
        info->pluginName = (char*) malloc(sizeof(char));
        info->pluginName[0] = '\0';
        info->pluginVersionMajor = info->pluginVersionMinor = info->pluginVersionPatch = 0;
        info->getFeatureCount = private_APluginSDK_getFeatureCount;
        info->getFeatureInfo = private_APluginSDK_getFeatureInfo;
        info->getFeatureInfos = private_APluginSDK_getFeatureInfos;
        info->getClassCount = private_APluginSDK_getClassCount;
        info->getClassInfo = private_APluginSDK_getClassInfo;
        info->getClassInfos = private_APluginSDK_getClassInfos;
        return info;
    }
    static void private_APluginSDK_destructPluginInfo(struct APLUGINLIBRARY_NAMESPACE APluginInfo* info)
    {
        private_APluginSDK_destructPrivatePluginInfo(info->privateInfo);
        free(info->pluginName);
        free(info);
    }

PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE
