#include "vfunctions.h"

#include <memory/gamedata/gamedata.h>
#include <dynlibutils/module.h>

DynLibUtils::CModule DetermineModuleByLibrary(std::string library);

VFunctionHook::VFunctionHook(std::string library, std::string vtable, std::string offsetName, dyno::CallbackType callbackType, dyno::CallbackHandler handler, std::string function_args, char function_return)
{
    m_library = library;
    m_vtable = vtable;
    m_offsetName = offsetName;
    m_pvtable = nullptr;
    m_ioffset = 0;
    m_callbackType = callbackType;
    m_handler = handler;
    m_function_args = function_args;
    m_function_return = function_return;
}

VFunctionHook::~VFunctionHook()
{
    Disable();
    if (!m_pvtable) return;

    dyno::IHookManager& manager = dyno::IHookManager::Get();
    manager.unhookVirtual(m_pvtable, m_ioffset);
}

void VFunctionHook::Initialize()
{
    auto module = DetermineModuleByLibrary(m_library);
    auto vtable = module.GetVirtualTableByName(m_vtable);
    if (!vtable) return;

    int offset = g_GameData.GetOffset(m_offsetName);
    if (offset == -1) return;

    m_pvtable = vtable;
    m_ioffset = offset;

    dyno::IHookManager& manager = dyno::IHookManager::Get();
    hookFunc = manager.hookVirtual(m_pvtable, m_ioffset, [this] { return new DEFAULT_CALLCONV(GetDataObjectList(m_function_args), GetDataObject(m_function_return)); }).get();

    hookFunc->addCallback(m_callbackType, m_handler);
}

void VFunctionHook::Enable()
{
    if (enabled) return;
    if (!m_pvtable) return;

    enabled = true;
    hookFunc->addCallback(m_callbackType, m_handler);
}

void VFunctionHook::Disable()
{
    if (!enabled) return;
    if (!m_pvtable) return;

    enabled = false;
    hookFunc->removeCallback(m_callbackType, m_handler);
}