# reZonator 2

[reZonator](http://rezonator.orion-project.org) is a software for designing laser resonators 
and calculation the propagation of light beams (as gaussian so as ray vectors) in complex optical systems. 

This is official port of reZonator to [Qt](qt.io) framework. The goal is to make it open-source and cross-platform.

Currently version 2 is working but it is in early alpha state and does not implement most functions of version 1.
[Here](http://rezonator.orion-project.org/index.php?page=ver2) are some details and prebuild packages.

## Prepare build environment
Clone the repository
```
git clone https://github.com/orion-project/rezonator2
```
Update submodules:
```
cd rezonator2
$ git submodule init
$ git submodule update
```
Note, that submodules are in 'detached head' state by default.

## Building
Only building via Qt Creator IDE is currently supported. 
Just open project file `rezonator.pro` in IDE and configure it to use some of installed Qt-kits. 
Any of modern Qt 5.* kits should suit.
