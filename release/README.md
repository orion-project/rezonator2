# Release

This directory contains some stuff used during preparation of release version of the application and making redistributable packages.

** Version format**

```
MAJOR . MINOR . MICRO . COMMITS [-CODENAME]
```

**Prepare new release**

Make new release tag:

```bash
git tag -a version-2.0.1-alpha1 -m 'Short version description'
git push origin version-2.0.1-alpha1
```

* `COMMITS` version number is calculated as a number of commits after the release tag.  
So when new release tag is created `COMMITS` number is reset and we have to increase `MICRO` number to avoid version duplication. E.g.: `2.0.0.139-alpha0 -> 2.0.1.0-alpha1`

Update version info that will be built into application:
 
```bash
 ./release/make_version.py
```

Push updated version info to be able to build package having the same version on other platforms:

```bash
 git commit -am 'Version info updated'
 git push
```

Build package:

```bush
./scripts/build_release.sh
./scripts/make_package_linux.sh
```

Build package on different platform (e.g. MacOS):

```bash
git pull --prune --tags
git pull
./scripts/build_release.sh
./scripts/make_package_macos.sh
```
