# 在 Windows 上准备第三方库

确保 MinGW 的 `bin` 目录在你的 `PATH` 中。建议使用与 Qt 相同的 MinGW 版本。例如：
```
set PATH=C:\Qt\Tools\mingw530_32\bin;%PATH%
```

假设当前目录是项目根目录，即包含 `rezonator.pro` 文件的目录。

## Lua

从 http://www.lua.org/ftp/lua-5.3.4.tar.gz 下载源码包，并将其解压到 `libs` 目录中，这样你应该会得到位于 `libs/lua-5.3.4` 的 Lua。
