#ifndef _embedder_context_h
#define _embedder_context_h

#include <set>
#include <map>
#include <string>

#include <lua.hpp>
#include <quickjs.h>
#include <vector>

#include "ContextKinds.h"

class EValue;

class EContext
{
private:
    void *m_state;
    ContextKinds m_kind;
    std::set<EValue *> mappedValues;

    std::map<std::string, JSClassID> classIDs;
    std::map<std::string, JSValue> classPrototypes;

    std::map<std::string, void *> functionCalls;
    std::map<std::string, std::vector<void *>> functionPreCalls;
    std::map<std::string, std::vector<void *>> functionPostCalls;

    std::map<std::string, void *> classFunctionCalls;
    std::map<std::string, std::vector<void *>> classFunctionPreCalls;
    std::map<std::string, std::vector<void *>> classFunctionPostCalls;

public:
    EContext(ContextKinds kind);
    ~EContext();

    void RegisterLuaLib(const char *libName, lua_CFunction func);

    ContextKinds GetKind();
    int64_t GetMemoryUsage();
    void *GetState();
    lua_State *GetLuaState();
    JSContext *GetJSState();

    JSValue &GetClassPrototype(std::string className);

    int RunCode(std::string code);
    int RunFile(std::string path);

    void PushValue(EValue *val);
    void PopValue(EValue *val);

    JSClassID *GetClassID(std::string className);
    std::string GetClsName(JSClassID id);

    void AddFunctionCall(std::string key, void *val);
    void *GetFunctionCall(std::string key);

    void AddFunctionPreCall(std::string key, void *val);
    std::map<std::string, std::vector<void *>> GetFunctionPreCalls();

    void AddFunctionPostCall(std::string key, void *val);
    std::map<std::string, std::vector<void *>> GetFunctionPostCalls();

    void AddClassFunctionCalls(std::string key, void *val);
    void *GetClassFunctionCall(std::string key);

    void AddClassFunctionPreCalls(std::string key, void *val);
    std::map<std::string, std::vector<void *>> GetClassFunctionPreCalls();

    void AddClassFunctionPostCalls(std::string key, void *val);
    std::map<std::string, std::vector<void *>> GetClassFunctionPostCalls();
};

EContext *GetContextByState(JSContext *ctx);
EContext *GetContextByState(lua_State *ctx);

#endif