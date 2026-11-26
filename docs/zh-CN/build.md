# 从源码构建

## 准备构建环境

### Qt

必须安装 [Qt for Open Source](https://www.qt.io/download-open-source)。目前所有平台的目标 Qt 版本是 5.15.2。对于 Windows，请选择 “MSVC 2019 64-bit” 版本。QtCreator 是本项目默认使用的 IDE。

### Visual Studio

在 Windows 上使用 Microsoft C++ 编译器，因此必须单独安装 [Visual Studio Community 2022](https://visualstudio.microsoft.com/en/vs/community/)。唯一必需的组件是 “Desktop development with C++”（使用 C++ 的桌面开发）。

Visual Studio 也可以配合 [Qt Visual Studio Tools](https://doc.qt.io/qtvstools-2) 扩展用作 Qt 项目的 IDE。在 VS 中进入 “Extensions -> Manage Extensions”，搜索该扩展并安装，然后进入 “Extensions -> Qt VS Tools -> Qt Versions”，注册 Qt 的安装路径。

### CMake

CMake 可以单独安装，也可以作为 Qt 安装的一部分安装。

### vcpkg

[vcpkg](https://vcpkg.io/) 是一个开源的 C++ 包管理器。它必须安装在与项目分开的目录中，用于准备第三方依赖，例如 Python：

```bash
git clone https://github.com/microsoft/vcpkg.git
```

然后根据平台运行 `bootstrap-vcpkg.sh` 或 `bootstrap-vcpkg.bat`，将 vcpkg 目录添加到 `PATH` 中，并创建包含安装路径的 `VCPKG_ROOT` 变量。

在项目目录中下载并编译依赖：

```bash
vcpkg install
```

**Ubuntu：** 至少需要 22（jammy）才能构建 Python。

**macOS：** 应使用 Qt 6.9.3 将 Python 链接到应用（详见下文 “Quirks” 一节）

在 QtCreator 中配置项目时，在 CMake Initial Configuration 选项卡中添加额外的选项并重新配置。它必须指向已安装的 vcpkg 工具链文件的位置，例如：

```
-DCMAKE_TOOLCHAIN_FILE:FILEPATH=/home/user/vcpkg/scripts/buildsystems/vcpkg.cmake
```


### Qt 的注意事项（Quirks）

**[Ubuntu] 平台插件**

如果 QtCreator 在 Ubuntu 上因以下原因拒绝运行

```log
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
```

尝试运行

```bash
sudo apt-get install --reinstall libxcb-xinerama0
```

我用 16.04 进行构建，这个方法有效。（更新：自 reZonator-2.1 起使用 Ubuntu 22.04）

**[Ubuntu] GL 库**

第一次在 Ubuntu 上构建时，它会提示

```log
../Qt/5.15.2/gcc_64/include/QtGui/qopengl.h:141:22: fatal error: GL/gl.h: No such file or directory
```

必须安装额外的开发库：

```bash
sudo apt-get install build-essential libgl1-mesa-dev
```

QCustomPlot 的[说明](https://www.qcustomplot.com/index.php/tutorials/settingup)中也提到了这些，因此可能也需要：

```bash
sudo apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev
```

**（已过时）[macOS] 适用于 Sierra 的最新 Qt**

Qt 5.12.10 是可用于 Sierra（10.12）的 5.* LTS 的最新版本。
QtCreator 4.10.2 也是最新版本，必须手动下载并安装。
同样，5.12.10 套件（kit）也必须在那个“过时”的 QtCreator 中手动注册。

**[macOS] 适用于 Monterey 的最新 Qt**

为了提供更广泛的兼容性，使用 macOS 12 来编译应用并构建可再分发的 dmg 镜像。当使用 Qt 5.15.2 时，由于 Python 与应用目标模块之间的架构名称存在某种不一致（?），Python 库的链接会失败。因此应使用 Qt 6.9.3，它是支持 macOS 12 的 6.* 版本中的最新版本。支持 macOS 12 的最新 Qt Creator 是 [17.0.2](https://github.com/qt-creator/qt-creator/releases/tag/v17.0.2)，应从 GitHub 单独安装。Qt Maintenance Tool 安装的是更新的版本，与之不兼容。

## 克隆 git 仓库

```bash
git clone https://github.com/orion-project/rezonator2
cd rezonator2
git submodule init
git submodule update
```

注意，子模块默认处于 'detached head'（分离头指针）状态。

<!--
## 准备第三方库

### Windows

[QCustomPlot](https://www.qcustomplot.com) 通过封装[仓库](https://github.com/orion-project/custom-plot-lab)（在子模块中）使用。该库的源文件 `qcustomplot.cpp` 非常大，以至于在调试模式下甚至无法构建（`x86_64-w64-mingw32/bin/as.exe: debug\qcustomplot.o: too many sections (33061) Fatal error: debug\qcustomplot.o: file too big`）。因此，如果需要调试模式，你必须将 QCustomPlot 用作共享库。构建该库：

```bash
cd libs\custom-plot-lab\qcustomplot
qmake
mingw32-make release
```

然后在 `rezonator.pro` 中启用选项 `qcustomplotlab_shared`，并在调试模式下重新构建应用。
-->

## 开发构建

使用 *Qt Creator IDE* 进行开发构建。只需在 IDE 中打开项目文件 `CMakeLists.txt`，并将其配置为使用某个已安装的 Qt 套件。任何现代的 Qt 5.15.2+ 套件都应该适用。

目标文件是 `bin/rezonator`（Linux）、`bin/rezonator.app`（MacOS）或 `bin\rezonator.exe`（Windows）。

<!--
## 发布构建

```bash
# Linux/MacOS
./scripts/build_release.sh

# Windows
scripts\build_release.bat
```

确保 Qt 的 bin 目录在你的 `PATH` 中，以使上述脚本能够工作。在 Windows 上，你还必须将 MinGW 的 `bin` 目录 `<qt-root>\<qt-version>\mingw*\bin` 也放入 `PATH` 中。这些脚本会执行完整的重新构建，需要一段时间。
-->

## 源码文档

```bash
sudo apt install doxygen
sudo apt install graphviz
mkdir -p out/src-doc
doxygen
```

## 用户手册

见[构建文档](../help/README.md)。
