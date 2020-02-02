#ifndef Z_LUA_HELPER_H
#define Z_LUA_HELPER_H

#include "CommonTypes.h"

struct lua_State;

namespace Z {

class Lua {
public:
    Lua();
    ~Lua();


    QString open();
    Z::Result<double> calculate(const QString& code);
    QString setCode(const QString& code);
    QString execute();
    Z::Result<double> getGlobalVar(const char* name);
    QMap<QString, double> getGlobalVars();
    void setGlobalVar(const QString& name, double value);
    void setGlobalVars(const QMap<QString, double>& vars);
    void removeGlobalVar(const QString& name);

    static void registerGlobalFuncs(lua_State* lua);

private:
    lua_State* _lua = nullptr;

    QString getLuaError(int errCode) const;
    QString refineLuaError(const QString& err) const;
};

} // namespace Z

#endif // Z_LUA_HELPER_H
