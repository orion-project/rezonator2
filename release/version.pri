# These values are updated by update_version.py

APP_VER_MAJOR=2
APP_VER_MINOR=0
APP_VER_PATCH=14
APP_VER_CODENAME=rc1
APP_VER_YEAR=2024

DEFINES += "APP_VER_MAJOR=$$APP_VER_MAJOR"
DEFINES += "APP_VER_MINOR=$$APP_VER_MINOR"
DEFINES += "APP_VER_PATCH=$$APP_VER_PATCH"
DEFINES += "APP_VER_CODENAME=\"\\\"$$APP_VER_CODENAME\\\"\""
DEFINES += "APP_VER_YEAR=$$APP_VER_YEAR"

win32 {
    DEFINES += "BUILDDATE=\"\\\"$$system(date /T)\\\"\""
    DEFINES += "BUILDTIME=\"\\\"$$system(time /T)\\\"\""
}
else {
    DEFINES += "BUILDDATE=\"\\\"$$system(date '+%F')\\\"\""
    DEFINES += "BUILDTIME=\"\\\"$$system(date '+%T')\\\"\""
}
