#include "entrypoint.h"

#include <utils/common.h>

#include <core/configuration/setup.h>
#include <core/console/console.h>

#include <server/configuration/configuration.h>
#include <server/translations/translations.h>

#include <filesystem/logs/logger.h>

#include <memory/gamedata/gamedata.h>

#include <sdk/access.h>
#include <sdk/game.h>

#include <schemasystem/schemasystem.h>

//////////////////////////////////////////////////////////////
/////////////////  Core Variables & Functions  //////////////
////////////////////////////////////////////////////////////

SwiftlyS2 g_Plugin;
IVEngineServer2* engine = nullptr;
ISource2Server* server = nullptr;
CSchemaSystem* g_pSchemaSystem2 = nullptr;

//////////////////////////////////////////////////////////////
/////////////////      Internal Variables      //////////////
////////////////////////////////////////////////////////////

Configuration g_Config;
Logger g_Logger;
Translations g_translations;
GameData g_GameData;
SDKAccess g_sdk;

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

    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pSchemaSystem2, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);

    HandleConfigExamples();

    if(g_Config.LoadConfiguration())
        PRINT("The configurations has been succesfully loaded.\n");
    else
        PRINTRET("Failed to load configurations. The framework will not work.\n", false);

    g_Logger.AddLogger("core", false);

    g_sdk.LoadSDKData();
    g_Config.LoadPluginConfigurations();
    g_GameData.LoadGameData();
    g_GameData.PerformPatches();

    g_translations.LoadTranslations();

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