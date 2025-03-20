#ifndef _entity_system_h
#define _entity_system_h

#include <core/entrypoint.h>
#include <memory/gamedata/gamedata.h>

SH_DECL_EXTERN3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);

class CGameResourceService
{
public:
    CGameEntitySystem* GetGameEntitySystem()
    {
        return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(this) + g_GameData.GetOffset("GameEntitySystem"));
    }
};

class EntitySystem
{
public:
    void Initialize();
    void Destroy();

    void StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);
};

#endif