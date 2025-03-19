#ifndef _utils_common_h
#define _utils_common_h

#include <string>

#ifdef _WIN32
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ __FUNCTION__
#endif

#ifdef _WIN32
#ifdef GetTickCount
#undef GetTickCount
#endif
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif
#ifdef VOID
#undef VOID
#endif
#ifdef GetObject
#undef GetObject
#endif
#endif

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

#define PREFIX "[Swiftly]"

#define GCC_COMPILER (defined(__GNUC__) && !defined(__clang__))

void PLUGIN_PRINT(std::string category, std::string str);
void PLUGIN_PRINTF(std::string category, std::string str, ...);

#define PRINT(str) PLUGIN_PRINT(__FUNCTION_NAME__, str)
#define PRINTF(str, ...) PLUGIN_PRINTF(__FUNCTION_NAME__, str, __VA_ARGS__)
#define PRINTRET(FORMAT_STR, RET) \
    {                             \
        PRINT(FORMAT_STR);        \
        return RET;               \
    }
#define PRINTFRET(FORMAT_STR, RET, ...)  \
    {                                    \
        PRINTF(FORMAT_STR, __VA_ARGS__); \
        return RET;                      \
    }
#define CLIENT_PRINT(SLOT, CATEGORY, FORMAT_STR) g_SMAPI->ClientConPrint(SLOT, std::string(PREFIX).append(" [").append(CATEGORY).append("] ").append(FORMAT_STR).c_str())
#define CLIENT_PRINTF(SLOT, CATEGORY, FORMAT_STR, ...) g_SMAPI->ClientConPrintf(SLOT, std::string(PREFIX).append(" [").append(CATEGORY).append("] ").append(FORMAT_STR).c_str(), __VA_ARGS__)


#endif