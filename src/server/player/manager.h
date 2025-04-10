#ifndef _server_player_h
#define _server_player_h

#include "player.h"

#include <public/playerslot.h>

class PlayerManager
{
private:
    Player** g_Players = nullptr;
public:
    PlayerManager();
    ~PlayerManager();

    Player* GetPlayer(CPlayerSlot slot);
    Player* GetPlayer(int slot);

    void RegisterPlayer(CPlayerSlot slot, Player* player);
    void UnregisterPlayer(CPlayerSlot slot);
    int GetPlayers();
    int GetPlayerCap();

    void Initialize();
    void Shutdown();

    void GameFrame(bool a, bool b, bool c);
};

extern PlayerManager g_playerManager;

#endif