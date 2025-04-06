_G = setmetatable(_G, {
    __index = function(t, k)
        if IsSDKClass(k) then
            return function(ptr)
                local ent = GenerateSDKFactory(k)
                ent(ptr)
                return ent
            end
        elseif IsTypeClass(k) then
            return GenerateTypeFactory(k)
        end
    end
})

sdk = setmetatable({}, {
    __index = function(t, k)
        if IsSDKClass(k) then
            return function(ptr)
                local ent = GenerateSDKFactory(k)
                ent(ptr)
                return ent
            end
        elseif IsTypeClass(k) then
            return GenerateTypeFactory(k)
        end
    end
})