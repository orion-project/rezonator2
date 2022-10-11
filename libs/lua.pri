LUA = $$PWD/lua-5.3.4/src

linux: DEFINES += LUA_USE_LINUX

INCLUDEPATH += $$LUA

HEADERS += \
    $$LUA/lapi.h \
    $$LUA/lauxlib.h \
    $$LUA/lcode.h \
    $$LUA/lctype.h \
    $$LUA/ldebug.h \
    $$LUA/ldo.h \
    $$LUA/lfunc.h \
    $$LUA/lgc.h \
    $$LUA/llex.h \
    $$LUA/llimits.h \
    $$LUA/lmem.h \
    $$LUA/lobject.h \
    $$LUA/lopcodes.h \
    $$LUA/lparser.h \
    $$LUA/lprefix.h \
    $$LUA/lstate.h \
    $$LUA/lstring.h \
    $$LUA/ltable.h \
    $$LUA/ltm.h \
    $$LUA/lua.h \
    $$LUA/luaconf.h \
    $$LUA/lualib.h \
    $$LUA/lundump.h \
    $$LUA/lvm.h \
    $$LUA/lzio.h

SOURCES += \
    $$LUA/lapi.c \
    $$LUA/lauxlib.c \
    $$LUA/lbaselib.c \
    $$LUA/lbitlib.c \
    $$LUA/lcode.c \
    $$LUA/lcorolib.c \
    $$LUA/lctype.c \
    $$LUA/ldblib.c \
    $$LUA/ldebug.c \
    $$LUA/ldo.c \
    $$LUA/ldump.c \
    $$LUA/lfunc.c \
    $$LUA/lgc.c \
    $$LUA/linit.c \
    $$LUA/liolib.c \
    $$LUA/llex.c \
    $$LUA/lmathlib.c \
    $$LUA/lmem.c \
    $$LUA/loadlib.c \
    $$LUA/lobject.c \
    $$LUA/lopcodes.c \
    $$LUA/loslib.c \
    $$LUA/lparser.c \
    $$LUA/lstate.c \
    $$LUA/lstring.c \
    $$LUA/lstrlib.c \
    $$LUA/ltable.c \
    $$LUA/ltablib.c \
    $$LUA/ltm.c \
    $$LUA/lundump.c \
    $$LUA/lutf8lib.c \
    $$LUA/lvm.c \
    $$LUA/lzio.c
