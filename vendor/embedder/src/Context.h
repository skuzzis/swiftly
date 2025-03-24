#ifndef _embedder_context_h
#define _embedder_context_h

#include <set>
#include <map>
#include <string>

#include <lua.hpp>
#include <quickjs.h>

#include "ContextKinds.h"

class EValue;

class EContext
{
private:
    void* m_state;
    ContextKinds m_kind;
    std::set<EValue*> mappedValues;

    std::map<std::string, JSClassID> classIDs;
    std::map<std::string, void*> functionCalls;
public:
    EContext(ContextKinds kind);
    ~EContext();

    void RegisterLuaLib(const char* libName, lua_CFunction func);

    ContextKinds GetKind();
    int64_t GetMemoryUsage();
    void* GetState();
    lua_State* GetLuaState();
    JSContext* GetJSState();

    int RunCode(std::string code);
    int RunFile(std::string path);

    void PushValue(EValue* val);
    void PopValue(EValue* val);

    JSClassID* GetClassID(std::string className);
    std::string GetClsName(JSClassID id);

    void AddFunctionCall(std::string key, void* val);
    void* GetFunctionCall(std::string key);
};


EContext* GetContextByState(JSContext* ctx);
EContext* GetContextByState(lua_State* ctx);

#endif