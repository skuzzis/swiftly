local eventHandlers = {}
local table_unpack = table.unpack

AddGlobalEvents(function(event, eventName, eventData)
    if not eventHandlers[eventName] then return 0 end
    if #eventHandlers[eventName] <= 0 then return 0 end

    for i = 1, #eventHandlers[eventName] do
        local handle = eventHandlers[eventName][i].handle
        if type(handle) == "function" then
            local result = (handle(event, table_unpack(eventData)) or 0)
            if result ~= 0 then return result end
        end
    end

    return 0
end)

local eventRegistryIndex = 50

function AddEventHandler(eventName, cb)
    if not eventHandlers[eventName] then eventHandlers[eventName] = {} end

    eventRegistryIndex = eventRegistryIndex + 1
    table.insert(eventHandlers[eventName], { eventRegistryIndex = eventRegistryIndex, handle = cb })

    RegisterEventHandlerPlugin(eventName)

    return { key = eventRegistryIndex, name = eventName }
end

function RemoveEventHandler(eventData)
    if not eventData.key or not eventData.name then
        print("Error: Invalid event data passed to RemoveEventHandler.")
        return
    end
    if not eventHandlers[eventData.name] then eventHandlers[eventData.name] = {} end

    for i = 1, #eventHandlers[eventData.name] do
        if eventHandlers[eventData.name].eventRegistryIndex == eventData.key then
            table.remove(eventHandlers[eventData.name], i)
            break
        end
    end

    if #eventHandlers[eventData.name] == 0 then
        RemoveEventHandlerPlugin(eventData.name)
    end
end

function TriggerEvent(eventName, ...)
    local eventResult, eventData, _ = table_unpack(TriggerEventInternal(eventName, {...}))
    return eventResult, eventData
end