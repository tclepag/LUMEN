#pragma once

#include "../framework.h"
#include "lua_interpreter.h"

#ifdef LULUA_EXPORTS
#define LULUA_API __declspec(dllexport)
#else
#define LULUA_API __declspec(dllimport)
#endif

namespace lua {
    class LuaInterpreter;

    class LuaScript {
    public:
        LuaScript(LuaInterpreter* luaState, const std::string& scriptPath)
            : l(luaState), path(scriptPath), scriptRef(0) {}
        ~LuaScript() = default;

        void load();
        void execute();
    private:
        LuaInterpreter* l;
        std::string path;
        int scriptRef;
    };
}
