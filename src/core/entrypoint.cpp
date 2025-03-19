#include "entrypoint.h"

#include <core/configuration/setup.h>
#include <core/console/console.h>

#include <sdk/game.h>

//////////////////////////////////////////////////////////////
/////////////////  Core Variables & Functions  //////////////
////////////////////////////////////////////////////////////
SwiftlyS2 g_Plugin;

//////////////////////////////////////////////////////////////
/////////////////      Internal Variables      //////////////
////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
/////////////////          Core Class          //////////////
////////////////////////////////////////////////////////////

PLUGIN_EXPOSE(SwiftlyS2, g_Plugin);
bool SwiftlyS2::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    if(GetGameName() == "unknown") {
        ismm->Format(error, maxlen, "Unknown game detected.");
        return false;
    }

    PLUGIN_SAVEVARS();
    g_SMAPI->AddListener(this, this);

    SetupConsoleColors();

    return true;
}

bool SwiftlyS2::Unload(char *error, size_t maxlen)
{
    return true;
}

void SwiftlyS2::AllPluginsLoaded()
{

}

bool SwiftlyS2::Pause(char* error, size_t maxlen)
{
    return true;
}

bool SwiftlyS2::Unpause(char* error, size_t maxlen)
{
    return true;
}

const char* SwiftlyS2::GetLicense()
{
    return "Gnu GPL 3 License";
}

const char* SwiftlyS2::GetVersion()
{
#ifndef SWIFTLY_VERSION
    return "Local";
#else
    return (std::string("v") + std::string(SWIFTLY_VERSION)).c_str();
#endif
}

const char* SwiftlyS2::GetDate()
{
    return __DATE__;
}

const char* SwiftlyS2::GetLogTag()
{
    return "SWIFTLY";
}

const char* SwiftlyS2::GetAuthor()
{
    return "Swiftly Development Team";
}

const char* SwiftlyS2::GetDescription()
{
    return "SwiftlyS2 - Framework";
}

const char* SwiftlyS2::GetName()
{
    return "SwiftlyS2";
}

const char* SwiftlyS2::GetURL()
{
    return "https://github.com/swiftly-solution/swiftly";
}