#include <memory/hooks/functions.h>

dyno::ReturnAction CCSPlayerPawnBase_PostThinkPre(dyno::CallbackType type, dyno::IHook& hook)
{
    printf("PostThinkPre Ptr: %p\n", hook.getArgument<void*>(0));
    return dyno::ReturnAction::Ignored;
}

FunctionHook CCSPlayerPawnBase_PostThink("CCSPlayerPawnBase_PostThink", dyno::CallbackType::Pre, CCSPlayerPawnBase_PostThinkPre, "p", 'v');