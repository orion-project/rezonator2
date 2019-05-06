#ifndef Z_LUA_HELPER_H
#define Z_LUA_HELPER_H

#include "CommonTypes.h"

struct lua_State;

namespace Z {

class Lua {
public:
    Lua();
    ~Lua();

    Z::Result<double> calculate(const QString& code);

private:
    lua_State* _lua = nullptr;

    void registerGlobalFuncs();
};

} // namespace Lua

#endif // Z_LUA_HELPER_H
