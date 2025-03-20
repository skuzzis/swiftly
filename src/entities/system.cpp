#include "system.h"

CGameEntitySystem* GameEntitySystem()
{
    return g_pGameEntitySystem;
}

void EntitySystem::Initialize()
{
    SH_ADD_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &EntitySystem::StartupServer, true);
}

void EntitySystem::Destroy()
{
    SH_REMOVE_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &EntitySystem::StartupServer, true);
}

bool bDone = false;
void EntitySystem::StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
    if (bDone) return;

    g_pGameEntitySystem = ((CGameResourceService*)g_pGameResourceService)->GetGameEntitySystem();
    g_pEntitySystem = g_pGameEntitySystem;

    bDone = true;
}