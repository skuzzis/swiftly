#ifndef _embedder_engine_h
#define _embedder_engine_h

#include "Context.h"
#include "Value.h"
#include <string>

#define ENGINE_VALUE(value) \
    EValue(ctx, value)
#define ENGINE_VALUE_CTX(ctx, value) \
    EValue(ctx, value)

#define ADD_VARIABLE(ns_path, var_name, value) \
    AddScriptingVariable(ctx, ns_path, var_name, ENGINE_VALUE(value))
#define ADD_VARIABLE_CTX(ctx, ns_path, var_name, value) \
    AddScriptingVariable(ctx, ns_path, var_name, ENGINE_VALUE_CTX(value))
#define ADD_VARIABLES(ns_path, values) \
    AddScriptingVariables(ctx, ns_path, values)
#define ADD_VARIABLES_CTX(ctx, ns_path, values) \
    AddScriptingVariables(ctx, ns_path, values)

void AddScriptingVariable(EContext* ctx, std::string namespace_path, std::string variable_name, EValue value);
void AddScriptingVariables(EContext* ctx, std::string namespace_path, std::map<std::string, EValue> values);

#endif