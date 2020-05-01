# APluginSDK
This repository is part of [APluginLibrary](https://github.com/Alex2804/APluginLibrary) and contains all components
to write your own plugins, which can then be loaded by [APluginLibrary](https://github.com/Alex2804/APluginLibrary).

The SDK provides an api for C and C++. The C api is compatible with C++ but not vice versa.


## <a name="Plugin">Plugin</a>
Plugins are provided as shared libraries and can be loaded into the program at runtime. Since they are normal shared
libraries, the same rules apply as usual regarding ABI compatibility.

## <a name="pluginapi.h">pluginapi.h</a>
pluginapi.h provides the API and macros to write plugins.

By default, pluginapi.h includes all required source files, unless APLUGINSDK_EXCLUDE_IMPLEMENTATION is defined before
pluginapi.h is included (note that the sources have to be included once or they have to be compiled manually by
compiling src/c/pluginapi.c or src/cpp/pluginapi.cpp (depending on the language)).

*If pluginapi.h is only included in one compilation unit, you don't have to do anything.*


## <a name="C-API">C API</a>
The C API is C90 compliant.

#### <a name="C-API-init-fini">Init and Fini function</a>
A plugin can have an initialization and a finalization method, which are called after which are called after loading/
before unloading of the shared library (not every time the plugin is loaded). The macros can only be used once per
plugin and must be used in the global scope. The initialization method must exist in the plugin, the finalization method
is optional.

The initialization method can be registered with the following macro:

    A_PLUGIN_INIT
    {
        // initialization code
    }

The finalization method can be registered with the following macro:

    A_PLUGIN_FiINI
    {
        // finalization code
    }

#### <a name="C-API-name-version">Name And Version</a>
You can set the name of your plugin with macro ```A_PLUGIN_SET_NAME(name);``` and the version with macro
```A_PLUGIN_SET_VERSION(major, minor, patch);```. These macros must be used in a function. With multiple use of these
macros the ordering is undefined. The function containing one or multiple of these macros must be executed at least once
to apply the name/version (only after execution the name/version appears in APluginInfo; the init function is guaranteed
to be executed on loading, it is recommended to set the name/version in this function).  
```A_PLUGIN_SET_NAME(name);``` is the same as ```A_PLUGIN_SET_NAME("name");```

#### <a name="C-API-feature">Feature</a>
A feature is a function that has a feature name and belongs to a feature group. A feature also has a return type
and an argument list.  
For each registered feature there is one APluginFeatureInfo object in the plugin which can be queried when the plugin
is loaded.

Features can be registered with the following macro:

    A_PLUGIN_REGISTER_FEATURE(returnType, featureGroup, featureName, [parameterList]...)
    {
        // function body
    }
    
And must be added to the internal feature manager with the following macro (must be in an function and is applied every
function call. It is recommended to use it in the initialization function):

    A_PLUGIN_RECORD_FEATURE(featureGroup, featureName);

for example:

    #include "path/to/APluginSDK/pluginapi.h" /* this must be included */
    #include <stdio.h>
    #include <string.h>
    
    static char* globalC;
    
    A_PLUGIN_REGISTER_FEATURE(int, group1, feature1, int x1, int x2)
    {
        return x1 * x2;
    }
    
    A_PLUGIN_REGISTER_FEATURE(double, group1, feature2, double x, int y, const char *c)
    {
        printf("c = %s\nglobal c = %s", c, globalC);
        return x + y;
    }
    
    A_PLUGIN_INIT
    {
        A_PLUGIN_SET_NAME("good_plugin_name"); /* same as A_PLUGIN_SET_NAME(good_plugin_name) */
        A_PLUGIN_SET_VERSION(9, 87, 789);
        A_PLUGIN_RECORD_FEATURE(group1, feature1);
        A_PLUGIN_RECORD_FEATURE(group1, feature2);
        globalC = (char*) malloc(sizeof(char) * 13);
        globalC[0] = '\0';
        strcat(globalC, "Hello World!");
        printf("good plugin initialized!\n");
    }
    
    A_PLUGIN_FINI
    {
        free(globalC);
        printf("good plugin finalized!\n");
    }

---

## <a name="CPP-API">C++ API</a>
The [C API](#C-API) is compatible with C++ but not vice versa. This means that plugins that use the C API can also be easily
compiled with C++. The C++ API has a few requirements less and some extensions. The C++ API is C++98 compliant.

#### <a name="CPP-API-init-fini">Init and Fini function</a>
The initialization and finalization functions are the same as the [C API](#C-API-init-fini), but both the initialization
function and the finalization function are optional. The macros must be used in global namespace!

#### <a name="CPP-API-name-version">Name And Version</a>
The name and version macros are the same as the [C API](#C-API-name-version) and can additionally be used outside of
functions in any namespace. The name/version selected from multiple definitions is undefined.

#### <a name="CPP-API-feature">Feature</a>
Features are registered in the same way as in the [C API](#C-API-feature), but you don't have to enter them manually
in the internal feature manager, as this is already done during registration (```A_PLUGIN_RECORD_FEATURE``` does
nothing). Features must not be in namespaces.

The following is sufficient to register the feature and add it to the internal feature manager:

    #include "path/to/APluginSDK/pluginapi.h" // this must be included
    #include <iostream>
    
    A_PLUGIN_REGISTER_FEATURE(int, group1, feature1, int x1, double y, const char* x2)
    {
        std::cout << x2 << std::end;
        return x1 + y;
    }

#### <a name="CPP-API-class">Class</a>
A class is a normal C++ class which is derived from an interface and implements its virtual methods.  
For each registered class there is one APluginClassInfo object in the plugin which can be queried when the plugin is
loaded.  
A class must not be registered multiple times and the passed class name and interface name must not contain any
namespaces.

There is no guarantee that there are no plugins with different interfaces but the same interface names.  
Classes should be marked with **APLUGINSDK_NO_EXPORT**!

Classes can be registered with the following macro (in global or namespace scope):

    A_PLUGIN_REGISTER_CLASS(interfaceName, className);
    
for example:
    
    // interface.h

    namespace plugin {
        namespace ifc {
            class Interface
            {
            public:
                Interface() = default;
                virtual ~Interface() = default; // should be virtual because of polymorphism
    
                // (pure) virtual methods which get implemented by the implementation
                virtual int function1(int, int) = 0;
                virtual int function2(int) = 0;
            };
        }
    }
<!-- tsk -->
    // whatever_plugin.cpp
    #include "path/to/APluginSDK/pluginapi.h" // this must be included
    #include "path/to/interface.h" // include the interface
    
    namespace plugin {
        namespace impl {
            class APLUGINSDK_NO_EXPORT Implementation : public plugin::ifc::Interface
            {
            public:
                int function1(int x1, int x2) override;
                int function2(int x) override;
            };
    
            // implement the virtual methods
            int Implementation::function1(int x1, int x2) {
                return x1 * x2;
            }
            int Implementation::function2(int x) {
                return x * x;
            }
    
            typedef ifc::Interface Interface; // must use typedef to eliminate namespace
            A_PLUGIN_REGISTER_CLASS(Interface, Implementation);
        }
    }

---
