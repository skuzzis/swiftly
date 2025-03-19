#include "game.h"

std::string GetGameName()
{
#if SOURCE_ENGINE == SE_CS2
    return "cs2";
#else
    return "unknown";
#endif
}