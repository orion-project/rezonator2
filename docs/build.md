# Building from sources

## Prepare build environment

QtCreator must be installed and runnable.
Automatic project configuration is not supported.
So before running build scripts open the project in QtCreator and configure it for using particular Qt kit.
The latest 5.* is supported (it is 5.15.2 currently).

### Qt quirks

**[Ubuntu] Platform plugin**

If QtCreator refuses to run on Ubuntu because of

```log
qt.qpa.plugin: Could not load the Qt platform plugin "xcb" in "" even though it was found.
```

try to run 

```bash
sudo apt-get install --reinstall libxcb-xinerama0
```

I use 16.04 for builds and this works.

**[Ubuntu] GL lib**

First time when building on Ubuntu it says

```log
../Qt/5.15.2/gcc_64/include/QtGui/qopengl.h:141:22: fatal error: GL/gl.h: No such file or directory
```

additional dev libs must be installed:

```bash
sudo apt-get install build-essential libgl1-mesa-dev
```

These also mentioned in QCustomPlot [instructions](https://www.qcustomplot.com/index.php/tutorials/settingup) so probably will be needed:

```bash
sudo apt-get install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev
```

**[macOS] Latest Qt for Sierra**

Version Qt 5.12.10 is the latest of 5.* LTSs availablbe for Sierra (10.12).
QtCreator 4.10.2 is the latest as well and must be downloaded and installed manually.
Also 5.12.10 kit must be registered manually in that "obsolete" QtCreator.

## Clone git repository

```bash
git clone https://github.com/orion-project/rezonator2
cd rezonator2
git submodule init
git submodule update
```

Note that submodules are in 'detached head' state by default.

## Prepare third-party libraries

### Linux/MacOS

```bash
./scripts/prepare_deps.sh
```

### Windows

Windows does not provide default command line tools like `wget`, `tar` or `make`, so you have to prepare dependencies using tools you have installed following [these instructions](prepare-deps-win.md).

[QCustomPlot](https://www.qcustomplot.com) is used via wrapper [repository](https://github.com/orion-project/custom-plot-lab) (in submodule). The library source file `qcustomplot.cpp` is so large that it even fails to build in debug mode (`x86_64-w64-mingw32/bin/as.exe: debug\qcustomplot.o: too many sections (33061) Fatal error: debug\qcustomplot.o: file too big`). So if the debug mode is needed, you have to use QCustomPlot as shared library. Build the library:

```bash
cd libs\custom-plot-lab\qcustomplot
qmake
mingw32-make release
```

Then enable option `qcustomplotlab_shared` in `rezonator.pro` and rebuild the app in debug mode.

## Dev build

Use *Qt Creator IDE* to do dev builds. Just open project file `rezonator.pro` in the IDE and configure it to use some of the installed Qt-kits. Any of modern Qt 5.15.2+ kits should suit.

Target file is `bin/rezonator` (Linux), `bin/rezonator.app` (MacOS), or `bin\rezonator.exe` (Windows). 

## Release build

```bash
# Linux/MacOS
./scripts/build_release.sh

# Windows
scripts\build_release.bat
```

Ensure that Qt bin directory is in your `PATH` to make the above scripts working. On Windows you have to put MinGW `bin` directory `<qt-root>\<qt-version>\mingw*\bin` into the `PATH` too. The scripts do the full rebuild and it takes a while.

## Source code documentation

```bash
sudo apt install doxygen
sudo apt install graphviz
mkdir -p out/src-doc
doxygen
```

## User manual

See [building documentation](../help/README.md).
