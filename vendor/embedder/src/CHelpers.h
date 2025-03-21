#ifndef _embedder_internal_chelpers_h
#define _embedder_internal_chelpers_h

#include <lua.hpp>
#include <quickjs.h>
#include <cassert>
#include <string>
#include <iostream>

class CHelpers
{
public:
    static JSValue js_print_to_console(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv) {
        int i;
        const char* str;
        size_t len;

        for (i = 0; i < argc; i++) {
            if (i != 0) fputc('\t', stdout);
            str = JS_ToCStringLen(ctx, &len, argv[i]);
            if (!str) return JS_EXCEPTION;
            fwrite(str, 1, len, stdout);
            JS_FreeCString(ctx, str);
        }
        fputc('\n', stdout);
        return JS_UNDEFINED;
    }
};

#endif