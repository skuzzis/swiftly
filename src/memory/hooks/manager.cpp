#include "manager.h"

dyno::DataObject GetDataObject(char arg)
{
    if (arg == 'p')
        return dyno::DataObject(dyno::DataType::Pointer);
    else if (arg == 'f')
        return dyno::DataObject(dyno::DataType::Float);
    else if (arg == 'b')
        return dyno::DataObject(dyno::DataType::Bool);
    else if (arg == 'd')
        return dyno::DataObject(dyno::DataType::Double);
    else if (arg == 'i')
        return dyno::DataObject(dyno::DataType::Int32);
    else if (arg == 'u')
        return dyno::DataObject(dyno::DataType::UInt32);
    else if (arg == 's')
        return dyno::DataObject(dyno::DataType::String);
    else if (arg == 'I')
        return dyno::DataObject(dyno::DataType::Int64);
    else if (arg == 'U')
        return dyno::DataObject(dyno::DataType::UInt64);
    else if (arg == 'v')
        return dyno::DataObject(dyno::DataType::Void);
    else
        return dyno::DataObject(dyno::DataType::Pointer);
}

std::vector<dyno::DataObject> GetDataObjectList(std::string args)
{
    std::vector<dyno::DataObject> data;
    for (int i = 0; i < args.size(); i++) data.push_back(GetDataObject(args.at(i)));
    return data;
}

void HookManager::Initialize()
{
    for (auto hook : funchookQueue)
        hook->Initialize();

    initialized = true;
}

void HookManager::Shutdown()
{
    for (auto hook : funchookQueue)
        hook->Disable();
}

void HookManager::AddToQueue(FunctionHook* hook)
{
    if (initialized) hook->Initialize();
    funchookQueue.push_back(hook);
}

void HookManager::AddToQueue(VFunctionHook* hook)
{
    if (initialized) hook->Initialize();
    vfunchookQueue.push_back(hook);
}