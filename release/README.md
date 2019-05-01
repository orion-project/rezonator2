# Release

This directory contains some stuff used during preparation of release version of the application and making redistributable packages.

Commands for running scripts suppose that current dir is the project root - a directory containing `rezonator.pro` file.

**Version format**

```
MAJOR . MINOR . PATCH [-CODENAME]
```

See [Semantic Versioning](https://semver.org)


**Prepare new release**

* Decide a version numbers for the new release

Increase at least one of `MAJOR`, `MINOR` or `MICRO` numbers when creating a new release.

* Update version info that will be built into the application

```bash
 ./release/make_version.py 2.0.2-alpha2
```

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

* Build package on Linux:

```bash
git pull --prune --tags
git pull
./scripts/build_release.sh
./help/make.sh
./scripts/make_package_linux.sh
```

Target package is `./out/rezonator-{version}.AppImage`

* Build package on MacOS:

```bash
git pull --prune --tags
git pull
./scripts/build_release.sh
./help/make.sh
./scripts/make_package_macos.sh
```

Target package is `./out/rezonator-{version}.dmg`

* Build package on Windows:

```bash
git pull --prune --tags
git pull
scripts\build_release.bat
help\make.bat
scripts\make_package_win.bat
```

Target package is `out\redist` directory.

**TODO:** Pack this dir into zip archive (using python? there is no `zip` on Windows by default).

**TODO:** Make [InnoSetup](http://www.jrsoftware.org) installable package.
