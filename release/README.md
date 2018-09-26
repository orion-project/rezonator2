# Release

This directory contains some stuff used during preparation of release version of the application and making redistributable packages.

**Version format**

Version numbers are
```
MAJOR . MINOR . MICRO . COMMITS [-CODENAME]
```

and corresponding git tag should have format:
```
version-MAJOR.MINOR.MICRO-CODENAME
```

e.g.:
```
version-2.0.1-alpha1
```

**Prepare new release**

* Make new release tag

Increase at least one of `MAJOR`, `MINOR` or `MICRO` numbers when create a new release tag.

```bash
git tag -a version-2.0.2-alpha2 -m 'Short version description'
git push origin version-2.0.2-alpha2
```

* Update version info that will be built into the application:
 
```bash
 ./release/make_version.py
```

The script searches for the latest git tag having the format described above, then it extracts version numbers from the tag and puts them into files `version.pri`, `version.rc` and `version.txt`.

`COMMITS` version number is calculated as a number of commits since the release tag. When a new release tag is created the `COMMITS` number is reset. `COMMITS` identifies intermediate (bugfix) releases between tagged releases. E.g.: `2.0.0.13-alpha0 -> ... -> 2.0.0.42-alpha0 -> 2.0.1.0-alpha1`

* Push updated version info to be able to build package having the same version on other platforms:

```bash
 git commit -am 'Version info updated'
 git push
```

* Build package:

```bash
./scripts/build_release.sh
./scripts/make_package_linux.sh
```

* Build package on different platform (e.g. MacOS):

```bash
git pull --prune --tags
git pull
./scripts/build_release.sh
./scripts/make_package_macos.sh
```
