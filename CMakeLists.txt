cmake_minimum_required(VERSION 3.16)

set(TARGET_NAME rezonator) 

# get version from version.txt
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/release/version.txt versionString)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)[-]([0-9a-z]+)" MY_PROGRAM_VERSION_MATCH ${versionString})
message(STATUS "Project version:  ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}-${CMAKE_MATCH_4}")
set(PROJECT_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}) # -beta is skipped
add_definitions(-DAPP_VER_MAJOR="${CMAKE_MATCH_1}")
add_definitions(-DAPP_VER_MINOR="${CMAKE_MATCH_2}")
add_definitions(-DAPP_VER_PATCH="${CMAKE_MATCH_3}")
add_definitions(-DAPP_VER_CODENAME="${CMAKE_MATCH_4}")

# compiler definitions
add_definitions(-DQT_USE_QSTRINGBUILDER)

# define build informations
string(TIMESTAMP YEAR "%Y")
add_definitions(-DAPP_VER_YEAR=${YEAR})
string(TIMESTAMP DATE "%m-%d")
add_definitions(-DBUILDDATE="${DATE}")
string(TIMESTAMP TIME "%H:%M")
add_definitions(-DBUILDTIME="${TIME}")

# define project 
project(${TARGET_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

# cmake settings
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)


# get qt path input
set(QT_VERSION_MAJOR 5)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Gui Printsupport Network LinguistTools REQUIRED)

set(TS_FILES ${CMAKE_CURRENT_BINARY_DIR}/languages/rezonator_ru.ts)

set(PROJECT_SOURCES
    ${TS_FILES}
)

# add subdirectories
add_subdirectory(src)
add_subdirectory(libs)
add_subdirectory(libs/orion)


# include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/GeneratedFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/custom-plot-lab
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/lua-5.3.4/src
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Printsupport_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
)

# create executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${TARGET_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${TARGET_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(${TARGET_NAME} SHARED
            ${SOURCE} ${HEADERS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${TARGET_NAME}
            ${SOURCE} ${HEADERS}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

endif()



set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER http://www.rezonator.orion-project.org/
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)



if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${TARGET_NAME})
endif()

target_link_libraries(${TARGET_NAME})