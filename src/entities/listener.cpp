#include "system.h"

#include <utils/platform/platform.h>
#include <memory/gamedata/gamedata.h>
#include <sdk/schema.h>

CEntityListener g_entityListener;

void CEntityListener::OnEntitySpawned(CEntityInstance* pEntity)
{
}

void CEntityListener::OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent)
{
}

void EntityAllowHammerID(CEntityInstance* pEntity)
{
    Plat_WriteMemory((*(void***)pEntity)[g_GameData.GetOffset("GetHammerUniqueID")], (uint8_t*)"\xB0\x01", 2);
}

void CEntityListener::OnEntityCreated(CEntityInstance* pEntity)
{
    ExecuteOnce(EntityAllowHammerID(pEntity));

    if (std::string(pEntity->GetClassname()) == "cs_gamerules")
        gameRules = schema::GetProp<void*>(pEntity, "CCSGameRulesProxy", "m_pGameRules");
}

void CEntityListener::OnEntityDeleted(CEntityInstance* pEntity)
{
}