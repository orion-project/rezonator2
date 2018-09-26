# Release

This directory contains some stuff used during preparation of release version of the application and making redistributable packages.

**Version format**

```
MAJOR . MINOR . PATCH [-CODENAME]
```

See: [Semantic Versioning](https://semver.org)


**Prepare new release**

* Decide version numbers for new release

Increase at least one of `MAJOR`, `MINOR` or `MICRO` numbers when create a new release.

* Update version info that will be built into the application
 
```bash
 ./release/make_version.py 2.0.2-alpha2
```

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

* Make new release tag

```bash
git tag -a v2.0.2-alpha2 -m 'Short version description'
git push origin v2.0.2-alpha2
```
