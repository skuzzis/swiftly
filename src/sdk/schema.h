#ifndef _sdk_schema_h
#define _sdk_schema_h

#include <stdint.h>
#include <string>
#include <map>
#include <set>
#include <memory/gamedata/gamedata.h>
#include <schemasystem/schemasystem.h>

#ifdef GetProp
#undef GetProp
#endif

void SetStateChanged(uintptr_t entityPtr, std::string className, std::string fieldName, int extraOffset);

namespace sch
{
    int32_t FindChainOffset(const char* className);
    
    int32_t GetOffset(const char* className, const char* memberName);
    int32_t GetOffset(uint64_t path);
    
    bool IsNetworked(const char* className, const char* memberName);
    bool IsNetworked(uint64_t path);

    bool IsStruct(const char* cName);
    bool IsClassLoaded(const char* cName);
};

namespace schema
{
    template<class T>
    T GetProp(void* entity, const char* className, const char* fieldName)
    {
        return *(T*)((uintptr_t)entity + sch::GetOffset(className, fieldName));
    }

    template<class T>
    T* GetPropPtr(void* entity, const char* className, const char* fieldName)
    {
        return (T*)((uintptr_t)entity + sch::GetOffset(className, fieldName));
    }
};

#endif