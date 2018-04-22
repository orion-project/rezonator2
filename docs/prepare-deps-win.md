# Preparation third-party libraries on Windows

Ensure that MinGW `bin` directory is in your `PATH`. It is recommended to use the same MinGW version as Qt uses. For example:
```
set PATH=C:\Qt\Tools\mingw530_32\bin;%PATH%
```

It is supposed that current directory is the project root, i.e. this is a directory containing `rezonator.pro` file.

## Lua
Download source package from http://www.lua.org/ftp/lua-5.3.4.tar.gz and unpack it into `libs` dir, so you should have Lua in `libs/lua-5.3.4`. Then navigate to this dir and buid Lua:
```
cd libs/lua-5.3.4
mingw32-make mingw
```