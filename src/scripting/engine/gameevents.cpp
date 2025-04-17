#include <memory/hooks/functions.h>

#include <plugins/manager.h>
#include <sdk/schema.h>
#include <sdk/components/CTakeDamageInfo.h>

#include "ehandle.h"

dyno::ReturnAction CCSPlayerPawnBase_PostThinkPre(dyno::CallbackType type, dyno::IHook& hook)
{
    void* _this = hook.getArgument<void*>(0);
    if (!_this) return dyno::ReturnAction::Ignored;

    CHandle<CEntityInstance> originalController = schema::GetProp<CHandle<CEntityInstance>>(_this, "CCSPlayerPawnBase", "m_hOriginalController");
    if (!originalController) return dyno::ReturnAction::Ignored;

    auto playerid = originalController.GetEntryIndex() - 1;

    if (g_pluginManager.ExecuteEvent("core", "OnPlayerPostThink", { playerid }, {}) == EventResult::Stop)
        return dyno::ReturnAction::Supercede;

    return dyno::ReturnAction::Ignored;
}

FunctionHook CCSPlayerPawnBase_PostThink("CCSPlayerPawnBase_PostThink", dyno::CallbackType::Pre, CCSPlayerPawnBase_PostThinkPre, "p", 'v');

bool OnClientChat(int playerid, std::string text, bool teamonly)
{
    ClassData data({ { "plugin_name", "core" } }, "Event", nullptr);
    g_pluginManager.ExecuteEvent("core", "OnClientChat", { playerid, text, teamonly }, &data);

    bool response = true;
    try
    {
        response = std::any_cast<bool>(data.GetData<std::any>("event_return"));
    }
    catch (std::bad_any_cast& e)
    {
        response = true;
    }

    return response;
}

void OnClientConvarQuery(int playerid, std::string convar_name, std::string convar_value)
{
    g_pluginManager.ExecuteEvent("core", "OnClientConvarQuery", { playerid, convar_name, convar_value }, {});
}

dyno::ReturnAction Hook_CGameRules_TerminateRound(dyno::CallbackType type, dyno::IHook& hook)
{
    ClassData data({ { "plugin_name", "core" } }, "Event", nullptr);
    g_pluginManager.ExecuteEvent("core", "OnTerminateRound", { hook.getArgument<float>(1), hook.getArgument<uint32_t>(2) }, &data);

    bool response = true;
    try
    {
        response = std::any_cast<bool>(data.GetData<std::any>("event_return"));
    }
    catch (std::bad_any_cast& e)
    {
        response = true;
    }

    if (!response) return dyno::ReturnAction::Supercede;

    return dyno::ReturnAction::Ignored;
}

FunctionHook CGameRules_TerminateRound("CGameRules_TerminateRound", dyno::CallbackType::Pre, Hook_CGameRules_TerminateRound, "pfuIu", 'v');

dyno::ReturnAction Hook_CEntityIdentity_AcceptInput(dyno::CallbackType type, dyno::IHook& hook)
{
    ClassData data({ { "plugin_name", "core" } }, "Event", nullptr);
    ClassData ThisPlayer({ { "class_name", "CEntityInstance" }, { "class_ptr", hook.getArgument<CEntityIdentity*>(0)->m_pInstance } }, "SDKClass", nullptr);
    ClassData Activator({ { "class_name", "CEntityInstance" }, { "class_ptr", hook.getArgument<CEntityInstance*>(2) } }, "SDKClass", nullptr);
    ClassData Caller({ { "class_name", "CEntityInstance" }, { "class_ptr", hook.getArgument<CEntityInstance*>(3) } }, "SDKClass", nullptr);

    g_pluginManager.ExecuteEvent("core", "OnEntityAcceptInput", { &ThisPlayer, hook.getArgument<CUtlSymbolLarge*>(1)->String(), &Activator, &Caller, hook.getArgument<variant_t*>(4)->ToString(), hook.getArgument<int>(5) }, &data);

    bool response = true;
    try
    {
        response = std::any_cast<bool>(data.GetData<std::any>("event_return"));
    }
    catch (std::bad_any_cast& e)
    {
        response = true;
    }

    if (!response) return dyno::ReturnAction::Supercede;

    return dyno::ReturnAction::Ignored;
}

FunctionHook CEntityIdentity_AcceptInput("CEntityIdentity_AcceptInput", dyno::CallbackType::Pre, Hook_CEntityIdentity_AcceptInput, "pppppi", 'v');

dyno::ReturnAction Hook_CBaseEntity_TakeDamage(dyno::CallbackType type, dyno::IHook& hook)
{
    void* _this = hook.getArgument<void*>(0);
    CTakeDamageInfo* info = hook.getArgument<CTakeDamageInfo*>(1);

    CHandle<CEntityInstance> playerController = schema::GetProp<CHandle<CEntityInstance>>(_this, "CBasePlayerPawn", "m_hController");
    if (!playerController) return dyno::ReturnAction::Ignored;

    int playerid = playerController.GetEntryIndex() - 1;
    int attackerid = -1;

    auto attackerPawn = info->m_hAttacker;
    if (attackerPawn) {
        CHandle<CEntityInstance> attackerController = schema::GetProp<CHandle<CEntityInstance>>(attackerPawn.Get(), "CBasePlayerPawn", "m_hController");
        if (attackerController) attackerid = attackerController.GetEntryIndex() - 1;
    }

    ClassData data({ { "plugin_name", "core" } }, "Event", nullptr);
    ClassData damageinfo({ { "class_name", "CTakeDamageInfo" }, { "class_ptr", info } }, "SDKClass", nullptr);
    ClassData Inflictor({ { "class_name", "CBaseEntity" }, { "class_ptr", info->m_hInflictor.Get() } }, "SDKClass", nullptr);
    ClassData Ability({ { "class_name", "CBaseEntity" }, { "class_ptr", info->m_hAbility.Get() } }, "SDKClass", nullptr);

    g_pluginManager.ExecuteEvent("core", "OnPlayerDamage", { playerid, attackerid, &damageinfo, &Inflictor, &Ability }, &data);

    bool response = true;
    try
    {
        response = std::any_cast<bool>(data.GetData<std::any>("event_return"));
    }
    catch (std::bad_any_cast& e)
    {
        response = true;
    }

    if (!response) return dyno::ReturnAction::Supercede;

    return dyno::ReturnAction::Ignored;
}

FunctionHook CBaseEntity_TakeDamage("CBaseEntity_TakeDamage", dyno::CallbackType::Pre, Hook_CBaseEntity_TakeDamage, "pp", 'v');