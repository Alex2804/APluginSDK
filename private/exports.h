#ifndef APLUGINSDK_EXPORTS_H
#define APLUGINSDK_EXPORTS_H

#ifndef APLUGINSDK_EXPORT
#   if defined(APLUGINLIBRARY_EXPORT)
#       define APLUGINSDK_EXPORT APLUGINLIBRARY_EXPORT
#   elif defined(_WIN32)
#       define APLUGINSDK_EXPORT __declspec( dllexport )
#   else
#       define APLUGINSDK_EXPORT
#   endif
#endif

#ifndef APLUGINSDK_NO_EXPORT
#   if defined(APLUGINLIBRARY_NO_EXPORT)
#       define APLUGINSDK_NO_EXPORT APLUGINLIBRARY_NO_EXPORT
#   elif defined(__unix__) || defined(__APPLE__)
#       define APLUGINSDK_NO_EXPORT __attribute__((visibility("hidden")))
#   else
#       define APLUGINSDK_NO_EXPORT
#   endif
#endif

#ifdef PRIVATE_APLUGINSDK_DONT_EXPORT_API
#   define APLUGINSDK_API APLUGINSDK_NO_EXPORT
#else
#   define APLUGINSDK_API APLUGINSDK_EXPORT
#endif

#endif /* APLUGINSDK_EXPORTS_H */
