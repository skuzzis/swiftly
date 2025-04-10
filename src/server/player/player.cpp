#include "player.h"
#include "ehandle.h"
#include <core/entrypoint.h>

#include <entities/system.h>
#include <memory/virtual/virtual.h>
#include <memory/gamedata/gamedata.h>
#include <sdk/schema.h>
#include <sdk/game.h>
#include <utils/utils.h>
#include <sdk/components/CSingleRecipientFilter.h>

#include "usermessages.pb.h"

typedef IGameEventListener2* (*GetLegacyGameEventListener)(CPlayerSlot slot);
typedef void (*CCSPlayerController_SwitchTeam)(void*, unsigned int);

std::map<std::string, std::string> colors = {
    {"{DEFAULT}", "\x01"},
    {"{WHITE}", "\x01"},
    {"{DARKRED}", "\x02"},
    {"{LIGHTPURPLE}", "\x03"},
    {"{GREEN}", "\x04"},
    {"{OLIVE}", "\x05"},
    {"{LIME}", "\x06"},
    {"{RED}", "\x07"},
    {"{GRAY}", "\x08"},
    {"{GREY}", "\x08"},
    {"{LIGHTYELLOW}", "\x09"},
    {"{YELLOW}", "\x09"},
    {"{SILVER}", "\x0A"},
    {"{BLUEGREY}", "\x0A"},
    {"{LIGHTBLUE}", "\x0B"},
    {"{BLUE}", "\x0B"},
    {"{DARKBLUE}", "\x0C"},
    {"{PURPLE}", "\x0E"},
    {"{MAGENTA}", "\x0E"},
    {"{LIGHTRED}", "\x0F"},
    {"{GOLD}", "\x10"},
    {"{ORANGE}", "\x10"},
};

std::string ProcessColor(std::string str, int team = CS_TEAM_CT)
{
    str = replace(str, "{TEAMCOLOR}", team == CS_TEAM_CT ? "{LIGHTBLUE}" : (team == CS_TEAM_T ? "{YELLOW}" : "{LIGHTPURPLE}"));
    str = replace(str, "{teamcolor}", team == CS_TEAM_CT ? "{lightblue}" : (team == CS_TEAM_T ? "{yellow}" : "{lightpurple}"));
    for (auto it = colors.begin(); it != colors.end(); ++it)
    {
        str = replace(str, it->first, it->second);
        str = replace(str, str_tolower(it->first), it->second);
    }
    return str;
}

Player::Player(bool m_isFakeClient, int m_slot, const char* m_name, uint64_t m_xuid, std::string ip_address)
{
    slot = m_slot;
    isFakeClient = m_isFakeClient;
    connectTime = time(0);
    name = m_name;
    xuid = m_xuid;
    ip_address = ip_address;

    centerMessageEvent = g_gameEventManager->CreateEvent("show_survival_respawn_status", true);

    centerMessageEvent->SetUint64("duration", 1);
    centerMessageEvent->SetInt("userid", slot);

    if (!isFakeClient) playerListener = g_GameData.FetchSignature<GetLegacyGameEventListener>("LegacyGameEventListener")(CPlayerSlot(slot));
}

Player::~Player()
{
    isFakeClient = true;
    g_gameEventManager->FreeEvent(centerMessageEvent);

    if(playerObject) delete playerObject;
}

bool Player::IsFakeClient()
{
    return isFakeClient;
}

int Player::GetSlot()
{
    return slot;
}

uint32_t Player::GetConnectedTime()
{
    return (time(0) - connectTime);
}

std::string Player::GetIPAddress()
{
    return ip_address;
}

bool Player::IsFirstSpawn() {
    return firstSpawn;
}

void Player::SetFirstSpawn(bool value) {
    firstSpawn = value;
}

void* Player::GetController()
{
    CEntityInstance* controller = g_pEntitySystem->GetEntityInstance(CEntityIndex(slot + 1));
    if(!controller) return nullptr;

    return (void*)controller;
}

void* Player::GetPawn()
{
    auto controller = GetController();
    if(!controller) return nullptr;

    CHandle<CEntityInstance> pawnHandle = schema::GetProp<CHandle<CEntityInstance>>(controller, "CBasePlayerController", "m_hPawn");
    return (void*)(pawnHandle.Get());
}

void* Player::GetPlayerPawn()
{
    auto controller = GetController();
    if(!controller) return nullptr;

    CHandle<CEntityInstance> pawnHandle = schema::GetProp<CHandle<CEntityInstance>>(controller, "CCSPlayerController", "m_hPlayerPawn");
    return (void*)(pawnHandle.Get());
}

const char* Player::GetName()
{
    auto controller = GetController();
    if(!controller) return name;

    return schema::GetPropPtr<char>(controller, "CBasePlayerController", "m_iszPlayerName");
}

uint64_t Player::GetSteamID()
{
    if(IsFakeClient()) return 0;

    auto controller = GetController();
    if(!controller) return xuid;

    return schema::GetProp<uint64_t>(controller, "CBasePlayerController", "m_steamID");
}

void Player::SendMsg(int dest, const char* msg, ...)
{
    auto controller = GetController();
    if (!controller) return;
    if (IsFakeClient()) return;

    if (dest == HUD_PRINTTALK || dest == HUD_PRINTNOTIFY)
    {
        va_list args;
        char buffer[4096];
        va_start(args, msg);

        uint32_t len = vsnprintf(buffer, sizeof(buffer), msg, args);
        if (len >= sizeof(buffer))
        {
            len = sizeof(buffer) - 1;
            buffer[len] = '\0';
        }
        va_end(args);

        std::string message(buffer);
        if (message.size() != 0)
        {
            if (ends_with(message, "\n"))
                message.pop_back();
            message += "\x01";

            bool startsWithColor = (message.at(0) == '{');

            message = ProcessColor(message, schema::GetProp<int>(controller, "CBaseEntity", "m_iTeamNum"));

            if (startsWithColor)
                message = " " + message;
        }

        INetworkMessageInternal* pNetMsg = g_pNetworkMessages->FindNetworkMessagePartial("TextMsg");
        auto data = pNetMsg->AllocateMessage()->ToPB<CUserMessageTextMsg>();

        data->set_dest(dest);
        data->add_param(message.c_str());

        CSingleRecipientFilter filter(GetSlot());
        g_pGameEventSystem->PostEventAbstract(-1, false, &filter, pNetMsg, data, 0);

        #ifndef _WIN32
        delete data;
        #endif
    }
    else if (dest == HUD_PRINTCENTER)
    {
        if (std::string(msg) == "") centerMessageEndTime = 0;
        else {
            centerMessageEndTime = GetTime() + 5000;
            centerMessageEvent->SetString("loc_token", msg);
        }
    }
    else if (dest == HUD_PRINTCONSOLE)
    {
        if (!g_SMAPI)
            return;

        va_list args;
        char buffer[1024];
        va_start(args, msg);

        uint32_t len = vsnprintf(buffer, sizeof(buffer), msg, args);
        if (len >= sizeof(buffer))
        {
            len = sizeof(buffer) - 1;
            buffer[len] = '\0';
        }
        va_end(args);

        std::string message(buffer);
        if (message.size() != 0)
        {
            for (auto it = colors.begin(); it != colors.end(); ++it)
            {
                message = replace(message, it->first, "");
                message = replace(message, str_tolower(it->first), "");
            }
            engine->ClientPrintf(GetSlot(), message.c_str());
        }
    }
}

void Player::Think()
{
    if(centerMessageEndTime != 0) {
        if(centerMessageEndTime >= GetTime()) {
            if(centerMessageEvent && playerListener) {
                playerListener->FireGameEvent(centerMessageEvent);
            }
        } else {
            centerMessageEndTime = 0;
        }
    }
}

std::any Player::GetInternalVar(std::string name)
{
    if (internalVars.find(name) == internalVars.end())
        return nullptr;

    return internalVars[name];
}

void Player::SetInternalVar(std::string name, std::any value)
{
    internalVars[name] = value;
}

void Player::SetListen(CPlayerSlot slot, int listen)
{
    m_listenMap[slot.Get()] = listen;
}

void Player::SetVoiceFlags(int flags)
{
    m_voiceFlag = flags;
}

int Player::GetVoiceFlags()
{
    return m_voiceFlag;
}

int Player::GetListen(CPlayerSlot slot)
{
    return m_listenMap[slot.Get()];
}

void Player::PerformCommand(std::string command)
{
    engine->ClientCommand(CPlayerSlot(GetSlot()), command.c_str());
}

void Player::SwitchTeam(int team)
{
    void* playerController = GetController();
    if (!playerController)
        return;

    if (team == CS_TEAM_SPECTATOR || team == CS_TEAM_NONE)
        CALL_VIRTUAL(void, g_GameData.GetOffset("CCSPlayerController_ChangeTeam"), playerController, team);
    else
        g_GameData.FetchSignature<CCSPlayerController_SwitchTeam>("CCSPlayerController_SwitchTeam")(playerController, team);
}

void Player::ChangeTeam(int team)
{
    void* playerController = GetController();
    if (!playerController)
        return;

    CALL_VIRTUAL(void, g_GameData.GetOffset("CCSPlayerController_ChangeTeam"), playerController, team);
}

Vector Player::GetCoords()
{
    void* pawn = GetPlayerPawn();
    if (!pawn) return Vector(0.0,0.0,0.0);

    void* bodyComponent = schema::GetProp<void*>(pawn, "CBaseEntity", "m_CBodyComponent");
    if(bodyComponent) return Vector(0.0,0.0,0.0); 

    void* sceneNode = schema::GetProp<void*>(bodyComponent, "CBodyComponent", "m_pSceneNode");
    if(!sceneNode) return Vector(0.0,0.0,0.0);

    return schema::GetProp<Vector>(sceneNode, "CGameSceneNode", "m_vecAbsOrigin");
}

void Player::SetCoords(float x, float y, float z)
{
    return SetCoords(Vector(x,y,z));
}

void Player::SetCoords(Vector vec)
{
    auto instance = GetPawn();
    if(!instance) return;

    CALL_VIRTUAL(void, g_GameData.GetOffset("CBaseEntity_Teleport"), instance, &vec, nullptr, nullptr);
}

void Player::SetClientConvar(std::string cmd, std::string val)
{
    INetworkMessageInternal* netmsg = g_pNetworkMessages->FindNetworkMessagePartial("SetConVar");
    auto msg = netmsg->AllocateMessage()->ToPB<CNETMsg_SetConVar>();
    CMsg_CVars_CVar* cvar = msg->mutable_convars()->add_cvars();
    cvar->set_name(cmd.c_str());
    cvar->set_value(val.c_str());

    CSingleRecipientFilter filter(this->slot);
    g_pGameEventSystem->PostEventAbstract(-1, false, &filter, netmsg, msg, 0);

#ifndef _WIN32
    delete msg;
#endif
}

void* Player::EnsureCustomView(int index)
{
    auto pawn = GetPlayerPawn();
    if(!pawn) return nullptr;

    if(schema::GetProp<uint32_t>(pawn, "CBaseEntity", "m_lifeState") == 2) {
        auto controller = GetController();
        if(!controller) return nullptr;
        if(schema::GetProp<bool>(controller, "CCSPlayerController", "m_bControllingBot")) return nullptr;

        auto observerServices = schema::GetProp<void*>(pawn, "CBasePlayerPawn", "m_pObserverServices");
        if(!observerServices) return nullptr;

        CHandle<CEntityInstance> observerTarget = schema::GetProp<CHandle<CEntityInstance>>(observerServices, "CPlayer_ObserverServices", "m_hObserverTarget");
        if(!observerTarget) return nullptr;

        auto observerController = schema::GetProp<CHandle<CEntityInstance>>(observerTarget.Get(), "CCSPlayerPawnBase", "m_hOriginalController");
        if(!observerController) return nullptr;

        CHandle<CEntityInstance> pawnHandle = schema::GetProp<CHandle<CEntityInstance>>(observerController, "CCSPlayerController", "m_hPlayerPawn");
        if(!pawnHandle) return nullptr;
        pawn = (void*)(pawnHandle.Get());
    }

    if(!pawn) return nullptr;

    void* vmServices = schema::GetProp<void*>(pawn, "CCSPlayerPawnBase", "m_pViewModelServices");
    if(!vmServices) return nullptr;

    auto vmHandle = (void*)(schema::GetPropPtr<CHandle<CEntityInstance>>(vmServices, "CCSPlayer_ViewModelServices", "m_hViewModel")[index].Get());
    if(!vmHandle) {
        vmHandle = g_entSystem.CreateByName("predicted_viewmodel");
        g_entSystem.Spawn(vmHandle, nullptr);
        schema::GetPropPtr<CHandle<CEntityInstance>>(vmServices, "CCSPlayer_ViewModelServices", "m_hViewModel")[index].Set((CEntityInstance*)(vmHandle));
        schema::SetProp(vmHandle, "CBaseViewModel", "m_nViewModelIndex", index);
        ((CEntityInstance*)pawn)->NetworkStateChanged();
    }

    return vmHandle;
}

ClassData* Player::GetPlayerObject()
{
    if (playerObject == nullptr) playerObject = new ClassData({ { "playerid", GetSlot() } }, "Player", nullptr);
    return playerObject;
}