#include "game.h"

const char* GetGameName()
{
#if SOURCE_ENGINE == SE_CS2
    return "cs2";
#else
    return "unknown";
#endif
}