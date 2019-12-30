# APluginSDK
This repository is part of [APluginLibrary](https://github.com/Alex2804/APluginLibrary) and contains all components
to write your own plugins, which can then be loaded by [APluginLibrary](https://github.com/Alex2804/APluginLibrary).

### <a name="Plugin">Plugin</a>
Plugins are provided as shared libraries and can be loaded into the program at runtime.

A plugin can consist of [features](#Feature) (functions) and [classes](#Class).

Even though C++ types can be passed between plugin and main application, you should limit yourself to C types to avoid
ABI incompatibility.

---
### <a name="pluginapi.h">pluginapi.h</a>
pluginapi.h must be included to develop plugins.

If a plugin consists of multiple source files, either A_PLUGIN_SDK_EXCLUDE_DEFINITIONS must always except once be
defined before pluginapi.h is included to prevent redefinitions of functions and classes, or pluginapi.cpp is compiled as
source file and A_PLUGIN_SDK_EXCLUDE_DEFINITIONS is defined always before pluginapi.h is included (or you define it
for the whole project).

---
### <a name="names_and_versions">Name and Version</a>
Your can set the name of your plugin with the ```A_PLUGIN_SET_NAME(name)``` macro and the version with the
```A_PLUGIN_SET_VERSION(major, minor, patch)``` macro.

---
### <a name="Feature">Feature</a>
A feature is a function that has a feature name and belongs to a feature group. A feature also has a return type
and an argument list.  
For each registered feature there is one PluginFeatureInfo object in the plugin.

Features can be registered with the following macro:

    A_PLUGIN_REGISTER_FEATURE(returnType, featureGroup, featureName, [parameterList]...)
    {
        // function body
    }

for example:

    #include "APluginLibrary/pluginapi.h" // this must be included
    
    A_PLUGIN_REGISTER_FEATURE(int, group1, feature1, int x1, double y, const char* x2)
    {
        std::cout << x2 << std::end;
        return x1 + y;
    }
---
### <a name="Class">Class</a>
A class is a normal C++ class which is derived from an interface and implements its virtual methods.  
For each registered class there is one PluginClassInfo object in the plugin.

There is no guarantee that there are no plugins with different interfaces but the same interface names.

Classes can be registered with the following macro:

    A_PLUGIN_REGISTER_CLASS(interfaceName, className);
    
for example:
    
    // interface.h (there should be include guards)
    
    class Interface
    {
    public:
        Interface() = default;
        virtual ~Interface() = default;
    
        virtual int function1(int, int) = 0;
        virtual int function2(int) = 0;
    };
<!-- tsk -->
    // whatever_plugin.cpp
    #include "APluginSDK/pluginapi.h" // this must be included
    #include "path/to/interface.h"
    
    class Implementation : public Interface
    {
    public:
        int function1(int x1, int x2) override;
        int function2(int x) override;
    };
    
    A_PLUGIN_REGISTER_CLASS(Interface, Implementation);
    
    int Implementation::function1(int x1, int x2) {
        return x1 * x2;
    }
    int Implementation::function2(int x) {
        return x * x;
    }
---
