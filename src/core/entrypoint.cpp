#include "entrypoint.h"

#include <tier1/convar.h>
#include <interfaces/interfaces.h>
#include <metamod_oslink.h>

#include <utils/common.h>

#include <core/configuration/setup.h>
#include <core/console/console.h>

#include <server/configuration/configuration.h>
#include <server/translations/translations.h>

#include <filesystem/logs/logger.h>

#include <memory/gamedata/gamedata.h>

#include <plugins/manager.h>

#include <sdk/access.h>
#include <sdk/game.h>

#include <schemasystem/schemasystem.h>

//////////////////////////////////////////////////////////////
/////////////////       SourceHook Hooks       //////////////
////////////////////////////////////////////////////////////

class GameSessionConfiguration_t
{
};

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);

//////////////////////////////////////////////////////////////
/////////////////  Core Variables & Functions  //////////////
////////////////////////////////////////////////////////////

SwiftlyS2 g_Plugin;
IVEngineServer2* engine = nullptr;
ISource2Server* server = nullptr;
CSchemaSystem* g_pSchemaSystem2 = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
IGameResourceService* g_pGameResourceService = nullptr;
CEntitySystem* g_pEntitySystem = nullptr;

//////////////////////////////////////////////////////////////
/////////////////      Internal Variables      //////////////
////////////////////////////////////////////////////////////

Configuration g_Config;
Logger g_Logger;
Translations g_translations;
GameData g_GameData;
SDKAccess g_sdk;
PluginsManager g_pluginManager;

//////////////////////////////////////////////////////////////
/////////////////          Core Class          //////////////
////////////////////////////////////////////////////////////

PLUGIN_EXPOSE(SwiftlyS2, g_Plugin);
bool SwiftlyS2::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();
    g_SMAPI->AddListener(this, this);

    if(GetGameName() == "unknown") {
        ismm->Format(error, maxlen, "Unknown game detected.");
        return false;
    }

    SetupConsoleColors();

    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pSchemaSystem2, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceService, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetServerFactory, server, ISource2Server, INTERFACEVERSION_SERVERGAMEDLL);

    HandleConfigExamples();

    if(g_Config.LoadConfiguration())
        PRINT("The configurations has been succesfully loaded.\n");
    else
        PRINTRET("Failed to load configurations. The framework will not work.\n", false);

    META_CONVAR_REGISTER(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

    g_Logger.AddLogger("core", false);

    g_sdk.LoadSDKData();
    g_Config.LoadPluginConfigurations();
    g_GameData.LoadGameData();
    g_GameData.PerformPatches();

    g_translations.LoadTranslations();

    g_pluginManager.LoadPlugins("");
    g_pluginManager.StartPlugins();

    PRINT("Succesfully started.\n");

    return true;
}

bool SwiftlyS2::Unload(char *error, size_t maxlen)
{
    g_pluginManager.StopPlugins(false);
    g_pluginManager.UnloadPlugins();
    return true;
}

void SwiftlyS2::AllPluginsLoaded()
{

}

void SwiftlyS2::OnLevelInit( char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background )
{

}

void SwiftlyS2::OnLevelShutdown()
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
    return "GNU GPL 3";
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