#include <scripting/core.h>

#include <sdk/game.h>
#include <server/player/manager.h>

#include "network_connection.pb.h"

LoadScriptingComponent(player, [](PluginObject plugin, EContext* ctx) -> void {
    ADD_FUNCTION("GetPlayer", [](FunctionContext* context) -> void {
        int playerid = context->GetArgumentOr<int>(0, -1);
        if (playerid < 0 || playerid >= GetMaxGameClients()) return;

        auto player = g_playerManager.GetPlayer(playerid);
        if (!player) return;

        context->SetReturn(player->GetPlayerObject());
    });

    ADD_CLASS("Player");

    ADD_CLASS_FUNCTION("Player", "CBaseEntity", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn(MakeSDKClassInstance("CBaseEntity", nullptr, context->GetPluginContext()));

        context->SetReturn(MakeSDKClassInstance("CBaseEntity", player->GetPlayerPawn(), context->GetPluginContext()));
    });

    ADD_CLASS_FUNCTION("Player", "CBasePlayerController", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn(MakeSDKClassInstance("CBasePlayerController", nullptr, context->GetPluginContext()));

        context->SetReturn(MakeSDKClassInstance("CBasePlayerController", player->GetController(), context->GetPluginContext()));
    });

    ADD_CLASS_FUNCTION("Player", "CBasePlayerPawn", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn(MakeSDKClassInstance("CBasePlayerPawn", nullptr, context->GetPluginContext()));

        context->SetReturn(MakeSDKClassInstance("CBasePlayerPawn", player->GetPawn(), context->GetPluginContext()));
    });

    ADD_CLASS_FUNCTION("Player", "CCSPlayerController", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn(MakeSDKClassInstance("CCSPlayerController", nullptr, context->GetPluginContext()));

        context->SetReturn(MakeSDKClassInstance("CCSPlayerController", player->GetController(), context->GetPluginContext()));
    });

    ADD_CLASS_FUNCTION("Player", "CCSPlayerPawn", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn(MakeSDKClassInstance("CCSPlayerPawn", nullptr, context->GetPluginContext()));

        context->SetReturn(MakeSDKClassInstance("CCSPlayerPawn", player->GetPlayerPawn(), context->GetPluginContext()));
    });

    ADD_CLASS_FUNCTION("Player", "CCSPlayerPawnBase", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn(MakeSDKClassInstance("CCSPlayerPawnBase", nullptr, context->GetPluginContext()));

        context->SetReturn(MakeSDKClassInstance("CCSPlayerPawnBase", player->GetPlayerPawn(), context->GetPluginContext()));
    });

    ADD_CLASS_FUNCTION("Player", "Drop", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return;

        int reason = context->GetArgumentOr<int>(0, -1);
        if (reason == -1) return;

        std::string str_reason = context->GetArgumentOr<std::string>(1, "");

        if (!engine) return;

        engine->DisconnectClient(CPlayerSlot(playerid), (ENetworkDisconnectionReason)reason, str_reason == "" ? nullptr : str_reason.c_str());
    });

    ADD_CLASS_FUNCTION("Player", "GetChatTag", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn("");

        context->SetReturn(player->GetInternalVar("tag"));
    });

    ADD_CLASS_FUNCTION("Player", "GetChatTagColor", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn("");

        context->SetReturn(player->GetInternalVar("tagcolor"));
    });

    ADD_CLASS_FUNCTION("Player", "GetNameColor", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn("");

        context->SetReturn(player->GetInternalVar("namecolor"));
    });

    ADD_CLASS_FUNCTION("Player", "GetChatColor", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return context->SetReturn("");

        context->SetReturn(player->GetInternalVar("chatcolor"));
    });

    ADD_CLASS_FUNCTION("Player", "SetChatTag", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return;

        std::string value = context->GetArgumentOr<std::string>(0, "");

        player->SetInternalVar("tag", value);
    });

    ADD_CLASS_FUNCTION("Player", "SetChatTagColor", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return;

        std::string value = context->GetArgumentOr<std::string>(0, "");

        player->SetInternalVar("tagcolor", value);
    });

    ADD_CLASS_FUNCTION("Player", "SetNameColor", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return;

        std::string value = context->GetArgumentOr<std::string>(0, "");

        player->SetInternalVar("namecolor", value);
    });

    ADD_CLASS_FUNCTION("Player", "SetChatColor", [](FunctionContext* context, ClassData* data) -> void {
        int playerid = data->GetData<int>("playerid");

        Player* player = g_playerManager.GetPlayer(playerid);
        if (!player) return;

        std::string value = context->GetArgumentOr<std::string>(0, "");

        player->SetInternalVar("chatcolor", value);
    });
})