#include "manager.h"

#include <core/entrypoint.h>
#include <sdk/game.h>

SH_DECL_EXTERN3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

uint64_t playerMask = 0;

PlayerManager::PlayerManager()
{
    g_Players = new Player*[GetMaxGameClients()];
}

PlayerManager::~PlayerManager()
{
    for(int i = 0; i < GetMaxGameClients(); i++) {
        if(g_Players[i]) {
            delete g_Players[i];
        }
    }
    delete g_Players;
}

Player* PlayerManager::GetPlayer(CPlayerSlot slot)
{
    return GetPlayer(slot.Get());
}

Player* PlayerManager::GetPlayer(int slot)
{
    if(slot < 0 || slot > GetMaxGameClients()-1) return nullptr;
    return g_Players[slot];
}

void PlayerManager::RegisterPlayer(CPlayerSlot slot, Player* player)
{
    int playerid = slot.Get();
    if(playerid < 0 || playerid >= GetMaxGameClients()) return;

    if(g_Players[playerid] != nullptr) return;

    g_Players[playerid] = player;
    playerMask |= (1ULL << playerid);
}

void PlayerManager::UnregisterPlayer(CPlayerSlot slot)
{
    int playerid = slot.Get();
    if(playerid < 0 || playerid >= GetMaxGameClients()) return;

    if((playerMask & (1ULL << playerid)) != 0) return;

    delete g_Players[playerid];
    g_Players[playerid] = nullptr;
    playerMask &= ~(1ULL << playerid);
}

int PlayerManager::GetPlayers()
{
    int count = 0;

    for(int i = 0; i < GetMaxGameClients(); i++)
        if(g_Players[i] != nullptr)
            ++count;

    return count;
}

int PlayerManager::GetPlayerCap()
{
    return GetMaxGameClients();
}

void PlayerManager::Initialize()
{
    SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, this, &PlayerManager::GameFrame, false);
}

void PlayerManager::Shutdown()
{
    SH_REMOVE_HOOK_MEMFUNC(IServerGameDLL, GameFrame, server, this, &PlayerManager::GameFrame, false);
}

void PlayerManager::GameFrame(bool a, bool b, bool c)
{
    for(int i = 0; i < GetPlayerCap(); i++) {
        Player* player = GetPlayer(i);
        if(player) player->Think();
    }
}