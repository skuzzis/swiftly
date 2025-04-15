#include "functions.h"
#include "manager.h"

#include <memory/gamedata/gamedata.h>

FunctionHook::FunctionHook(std::string signature_name, dyno::CallbackType callbackType, dyno::CallbackHandler handler, std::string function_args, char function_return)
{
    void* signature = g_GameData.FetchRawSignature(signature_name);
    if (!signature) return;

    m_callbackType = callbackType;
    m_handler = handler;
    m_pfn = nullptr;
    m_signature_name = signature_name;
    m_function_args = function_args;
    m_function_return = function_return;

    g_hookManager.AddToQueue(this);
}

FunctionHook::FunctionHook(const char* signature_name, dyno::CallbackType callbackType, dyno::CallbackHandler handler, std::string function_args, char function_return)
{
    m_callbackType = callbackType;
    m_handler = handler;
    m_pfn = nullptr;
    m_signature_name = signature_name;
    m_function_args = function_args;
    m_function_return = function_return;

    g_hookManager.AddToQueue(this);
}

FunctionHook::FunctionHook(void* signature_name, dyno::CallbackType callbackType, dyno::CallbackHandler handler, std::string function_args, char function_return)
{
    m_callbackType = callbackType;
    m_handler = handler;
    m_pfn = signature_name;
    m_signature_name = "";
    m_function_args = function_args;
    m_function_return = function_return;

    g_hookManager.AddToQueue(this);
}

void FunctionHook::Initialize()
{
    if (m_pfn == nullptr && m_signature_name != "") {
        m_pfn = g_GameData.FetchRawSignature(m_signature_name);
    }
    if (!m_pfn) return;

    dyno::IHookManager& manager = dyno::IHookManager::Get();
    hookFunc = manager.hookDetour(m_pfn, [this] { return new DEFAULT_CALLCONV(GetDataObjectList(m_function_args), GetDataObject(m_function_return)); }).get();

    hookFunc->addCallback(m_callbackType, m_handler);
}

FunctionHook::~FunctionHook()
{
    Disable();
    if (!m_pfn) return;

    dyno::IHookManager& manager = dyno::IHookManager::Get();
    manager.unhookDetour(m_pfn);
}

void FunctionHook::Enable()
{
    if (!m_pfn) return;
    if (enabled) return;

    enabled = true;
    hookFunc->addCallback(m_callbackType, m_handler);
}

void FunctionHook::Disable()
{
    if (!m_pfn) return;
    if (!enabled) return;

    enabled = false;
    hookFunc->removeCallback(m_callbackType, m_handler);
}