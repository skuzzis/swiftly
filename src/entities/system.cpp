#include "system.h"
#include <memory/gamedata/gamedata.h>

typedef void (*CBaseEntity_DispatchSpawn)(void*, void*);
typedef void (*UTIL_Remove)(void*);

extern std::map<void*, void*> entKeyVal;
void* gameRules = nullptr;

CGameEntitySystem* GameEntitySystem()
{
    return g_pGameEntitySystem;
}

void EntitySystem::Initialize()
{
    SH_ADD_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &EntitySystem::StartupServer, true);
}

void EntitySystem::Shutdown()
{
    SH_REMOVE_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &EntitySystem::StartupServer, true);

    g_pGameEntitySystem->RemoveListenerEntity(&g_entityListener);
}

bool bDone = false;
void EntitySystem::StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
    if (bDone) return;

    g_pGameEntitySystem = ((CGameResourceService*)g_pGameResourceService)->GetGameEntitySystem();
    g_pEntitySystem = g_pGameEntitySystem;

    bDone = true;

    g_pGameEntitySystem->AddListenerEntity(&g_entityListener);
}

void EntitySystem::Spawn(void* entity, void* keyvalues)
{
    g_GameData.FetchSignature<CBaseEntity_DispatchSpawn>("CBaseEntity_DispatchSpawn")(entity, keyvalues);

    if(entKeyVal.find(entity) != entKeyVal.end()) delete (CEntityKeyValues*)entKeyVal[entity];
    entKeyVal[entity] = keyvalues;
}

void EntitySystem::Despawn(void* entity)
{
    g_GameData.FetchSignature<UTIL_Remove>("UTIL_Remove")(entity);
}