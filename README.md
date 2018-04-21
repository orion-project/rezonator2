# reZonator 2

[reZonator](http://rezonator.orion-project.org) is a software for designing laser resonators 
and calculation the propagation of light beams (as gaussian so as ray vectors) in complex optical systems. 

This is official port of reZonator to [Qt](qt.io) framework. The goal is to make it open-source and cross-platform.

Currently version 2 is working but it is in early alpha state and does not implement most functions of version 1.
[Here](http://rezonator.orion-project.org/index.php?page=ver2) are some details and prebuild packages.


## Prepare build environment

### Clone git repository
```
git clone https://github.com/orion-project/rezonator2
cd rezonator2
git submodule init
git submodule update
```
Note that submodules are in 'detached head' state by default.

### Prepare third-party libraries

#### Linux/MacOS
```
chmod +x ./scripts/prepare_deps.sh
./scripts/prepare_deps.sh
```

#### Windows
Windows does not provide default command line tools like `wget`, `tar` or `make`, so you have to prepare dependencies using tools you have installed following [these instructions](docs/prepare-deps-win.md).


## Build

### Application
Only building via *Qt Creator IDE* is currently supported. 
Just open project file `rezonator.pro` in IDE and configure it to use some of installed Qt-kits. 
Any of modern Qt 5.* kits should suit.

### Source code documentation
```
sudo apt install doxygen
sudo apt install graphviz
mkdir -p out/src-doc
doxygen
```

### User manual
    TODO

## Run
Target file is `bin/rezonator` (Linux) or `bin\rezonator.exe` (Windows). 

To run program outside of IDE on Linux, you should use script `bin/rezonator.sh`.
It modifies `LD_LIBRARY_PATH` environment variable allowing program to load shared libraries from its `bin` directory.
