#ifndef _embedder_engine_h
#define _embedder_engine_h

#include "Context.h"
#include "Value.h"
#include <string>
#include "engine/functions.h"

//////////////////////////////////////////////////////////////
/////////////////    Scripting Engine Value    //////////////
////////////////////////////////////////////////////////////

#define ENGINE_VALUE(value) \
    EValue(ctx, value)
#define ENGINE_VALUE_CTX(ctx, value) \
    EValue(ctx, value)

//////////////////////////////////////////////////////////////
/////////////////  Scripting Engine Variables  //////////////
////////////////////////////////////////////////////////////

#define ADD_VARIABLE(ns_path, var_name, value) \
    AddScriptingVariable(ctx, ns_path, var_name, ENGINE_VALUE(value))
#define ADD_VARIABLE_CTX(ctx, ns_path, var_name, value) \
    AddScriptingVariable(ctx, ns_path, var_name, ENGINE_VALUE_CTX(value))
#define ADD_VARIABLES(ns_path, values) \
    AddScriptingVariables(ctx, ns_path, values)
#define ADD_VARIABLES_CTX(ctx, ns_path, values) \
    AddScriptingVariables(ctx, ns_path, values)

void AddScriptingVariable(EContext *ctx, std::string namespace_path, std::string variable_name, EValue value);
void AddScriptingVariables(EContext *ctx, std::string namespace_path, std::map<std::string, EValue> values);

//////////////////////////////////////////////////////////////
/////////////////  Scripting Engine Functions  //////////////
////////////////////////////////////////////////////////////

#define ADD_FUNCTION(function_name, callback) \
    AddScriptingFunction(ctx, "_G", function_name, callback)
#define ADD_FUNCTION_CTX(ctx, function_name, callback) \
    AddScriptingFunction(ctx, "_G", function_name, callback)
#define ADD_FUNCTION_NS(ns_path, function_name, callback) \
    AddScriptingFunction(ctx, ns_path, function_name, callback)
#define ADD_FUNCTION_NS_CTX(ctx, ns_path, function_name, callback) \
    AddScriptingFunction(ctx, ns_path, function_name, callback)
#define ADD_FUNCTION_PRE(function_name, callback) \
    AddScriptingFunctionPre(ctx, "_G", function_name, callback)
#define ADD_FUNCTION_CTX_PRE(ctx, function_name, callback) \
    AddScriptingFunctionPre(ctx, "_G", function_name, callback)
#define ADD_FUNCTION_NS_PRE(ns_path, function_name, callback) \
    AddScriptingFunctionPre(ctx, ns_path, function_name, callback)
#define ADD_FUNCTION_NS_CTX_PRE(ctx, ns_path, function_name, callback) \
    AddScriptingFunctionPre(ctx, ns_path, function_name, callback)

typedef void (*ScriptingFunctionCallback)(FunctionContext *);

void AddScriptingFunction(EContext *ctx, std::string namespace_path, std::string function_name, ScriptingFunctionCallback callback);
void AddScriptingFunctionPre(EContext *ctx, std::string namespace_path, std::string function_name, ScriptingFunctionCallback callback);

#endif