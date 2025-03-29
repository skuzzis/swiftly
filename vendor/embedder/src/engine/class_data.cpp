#include "classes.h"
#include "../Context.h"
#include "../Engine.h"
#include <regex>

ClassData::ClassData(std::map<std::string, std::any> data, std::string className, EContext* ctx)
{
    m_classData = data;
    m_className = className;
    m_ctx = ctx;
}

ClassData::~ClassData()
{
    if (!m_ctx) return;
    std::string str_key = m_className + " ~" + m_className;
    void* func = m_ctx->GetClassFunctionCall(str_key);
    if (!func) return;

    ScriptingClassFunctionCallback cb = reinterpret_cast<ScriptingClassFunctionCallback>(func);
    FunctionContext fctx(str_key, m_ctx->GetKind(), m_ctx, {}, 0);
    FunctionContext* fptr = &fctx;
    ClassData* data = this;
    bool ignoreCustomReturn = false;

    auto functionPreCalls = m_ctx->GetClassFunctionPreCalls();
    auto functionPostCalls = m_ctx->GetClassFunctionPostCalls();
    bool stopExecution = false;
    JSValue ret = JS_UNDEFINED;

    // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
    for (auto it = functionPreCalls.begin(); it != functionPreCalls.end(); ++it)
        if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
        {
            for (void* func : it->second)
            {
                reinterpret_cast<ScriptingClassFunctionCallback>(func)(fptr, data);
                if (fctx.ShouldStopExecution())
                {
                    stopExecution = true;
                    break;
                }
            }
            if (stopExecution)
                break;
        }

    if (!stopExecution) {
        cb(fptr, data);

        // @todo smarter approach, maybe at first function execution try to see if everything is valid, and if it is, cache it in a map the list and just iterate through it
        for (auto it = functionPostCalls.begin(); it != functionPostCalls.end(); ++it)
            if (std::regex_search(str_key, std::regex(it->first.c_str(), std::regex_constants::ECMAScript | std::regex_constants::optimize | std::regex_constants::nosubs)))
            {
                for (void* func : it->second)
                {
                    reinterpret_cast<ScriptingClassFunctionCallback>(func)(fptr, data);
                    if (fctx.ShouldStopExecution())
                    {
                        stopExecution = true;
                        break;
                    }
                }
                if (stopExecution)
                    break;
            }
    }
}

void ClassData::SetData(std::string key, std::any value)
{
    m_classData[key] = value;
}

std::string ClassData::GetClassname()
{
    return m_className;
}