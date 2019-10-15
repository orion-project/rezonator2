# Building from sources

## Prepare build environment

### Clone git repository
```bash
git clone https://github.com/orion-project/rezonator2
cd rezonator2
git submodule init
git submodule update
```
Note that submodules are in 'detached head' state by default.

### Prepare third-party libraries

#### Linux/MacOS
```bash
chmod +x ./scripts/prepare_deps.sh
./scripts/prepare_deps.sh
```

#### Windows
Windows does not provide default command line tools like `wget`, `tar` or `make`, so you have to prepare dependencies using tools you have installed following [these instructions](prepare-deps-win.md).


## Build
Where it is possible, commands are the same for different OS, with some exceptions. E.g. Windows doesn't have `sudo` command, and you have to run `cmd` as Administrator instead. 

### Application

#### Linux/MacOS
```bash
./scripts/build_release.sh
```
#### Windows
```bash
scripts\build_release.bat
```
Ensure that Qt bin directory is in your `PATH` to make above scripts working. On Windows, you have to put MinGW `bin` directory into the `PATH` too.

Note that above scripts do the full rebuild and it takes a while. Use *Qt Creator IDE* to do dev builds. Just open project file `rezonator.pro` in the IDE and configure it to use some of the installed Qt-kits. Any of modern Qt 5.8+ kits should suit.

### Source code documentation
```bash
sudo apt install doxygen
sudo apt install graphviz
mkdir -p out/src-doc
doxygen
```

### User manual
See [building documentation](../help/README.md).

## Run
Target file is `bin/rezonator` (Linux), `bin/rezonator.app` (MacOS), or `bin\rezonator.exe` (Windows). 
