#ifndef _core_extensions_ext_h
#define _core_extensions_ext_h

#include <string>
#include <swiftly-ext/extension.h>
#include <metamod_oslink.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

class Extension
{
private:
    std::string m_name;
    std::string m_error;
    bool m_errored = false;
    HINSTANCE m_hModule;
    SwiftlyExt* m_api;
    bool loaded = false;
public:
    Extension(std::string name);
    ~Extension();

    std::string GetName();
    SwiftlyExt* GetAPI();
    bool IsLoaded();

    std::string GetError();
    bool HasError();

    bool LoadExtension(bool late);
    bool UnloadExtension();
};

#endif