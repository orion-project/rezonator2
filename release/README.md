# Release

This directory contains some stuff used during preparation of release version of the application and making redistributable packages.

Commands for running scripts suppose that current dir is the project directory containing `rezonator.pro` file. On Linux and MacOS you have to mark scripts as executables to be able to run them:

```bash
chmod +x make_*.py
```

**Version format**

```
MAJOR . MINOR . PATCH [-CODENAME]
```

See [Semantic Versioning](https://semver.org)


**Prepare new release**

- Decide a version numbers for the new release. Increase at least one of `MAJOR`, `MINOR` or `MICRO` numbers when creating a new release. Update version info that will be built into the application (current version is stored in `version.txt` file):

```bash
 ./release/make_version.py 2.0.2-alpha2
```

* Put version number and release date and release info in the `history.json` file.

* Push updated version info to be able to build package having the same version on other platforms:

```bash
 git commit -am 'Version info updated 2.0.2-alpha2'
 git push
```

* Make a new release tag

```bash
git tag -a v2.0.2-alpha2 -m 'Short version description'
git push origin v2.0.2-alpha2
```

* Build package

```bash
./release/make_release.py

# for winwdows
.\help\make.bat

# for limux and macos
./help/make.sh

./release/make_package.py
```

Target package is in `../out` subdirectory, it's named

- on Windows `rezonator-{version}-win-{x32|x64}.zip`
- on Linux: `rezonator-{version}-linux-{x32|x64}.AppImage`
- on MacOS `rezonator-{version}.dmg`

### Notes

#### `qmake` version

Though the build scripts check if qmake is in PATH, you have to be sure it is the proper qmake version. For example, there could be available command `/usr/bin/qmake` on Ubuntu. But in general, for development, I use different Qt version installed in the home directory, e.g., `~/Qt/5.15.2/`. In this case, scripts' check for qmake will pass, but it highly probably won't be able to build the app. If it is the case, add proper Qt version into PATH before running build scripts:

```bash
export PATH=/home/user/Qt/5.15.2/gcc_64/bin/:$PATH
./release/make_release.py
```

