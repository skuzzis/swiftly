#ifndef _embedder_helpers_h
#define _embedder_helpers_h

inline const void* getContextKey()
{
#ifdef _NDEBUG
    static char value;
    return &value;
#else
    return reinterpret_cast<void*>(0xbee);
#endif
}

#endif