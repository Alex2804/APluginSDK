#ifndef APLUGINSDK_PRIVATEPLUGININFOS_H
#define APLUGINSDK_PRIVATEPLUGININFOS_H

#include "macros.h"

PRIVATE_APLUGINSDK_OPEN_PRIVATE_NAMESPACE
    struct APrivatePluginInfo
    {
        size_t(*constructPluginInternals)(void(*)(void));
        size_t(*destructPluginInternals)(void(*)(void));
    };
PRIVATE_APLUGINSDK_CLOSE_PRIVATE_NAMESPACE

#endif /* APLUGINSDK_PRIVATEPLUGININFOS_H */
