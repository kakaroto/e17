cmake_minimum_required(VERSION 2.6)
project(ecrire)
set(ECRIRE_VERSION_MAJOR 0)
set(ECRIRE_VERSION_MINOR 1)
set(ECRIRE_VERSION_MICRO 0)
set(ECRIRE_VERSION ${ECRIRE_VERSION_MAJOR}.${ECRIRE_VERSION_MINOR}.${ECRIRE_VERSION_MICRO})

set(PACKAGE ${CMAKE_PROJCT_NAME})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include_directories ("${PROJECT_BINARY_DIR}")

# FIXME: add version checking!
find_package(Eina REQUIRED)
find_package(Eet REQUIRED)
find_package(Evas REQUIRED)
find_package(Ecore REQUIRED)
find_package(Ecore COMPONENTS Ecore-x)
find_package(Edje REQUIRED)
find_package(Efreet REQUIRED)
find_package(Elementary REQUIRED)
find_package(Gettext)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)

# likely put this into an FindCompilerAttribute.cmake:
INCLUDE(CheckCSourceCompiles)
SET(HAVE___ATTRIBUTE__)
CHECK_C_SOURCE_COMPILES(
   "void  foo (int bar __attribute__((unused)) ) { }
   static void baz (void) __attribute__((unused));
   static void baz (void) { }
   int main(){}
   " HAVE___ATTRIBUTE__
   )

ADD_DEFINITIONS(-DPACKAGE_BIN_DIR="${CMAKE_INSTALL_PREFIX}/bin"
   -DPACKAGE_DATA_DIR="${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}"
   -DPACKAGE_LIB_DIR="${CMAKE_INSTALL_PREFIX}/lib")

set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/locale")
 
ADD_DEFINITIONS(
   -DLOCALE_DIR="${LOCALEDIR}"
   )

ADD_DEFINITIONS(-DHAVE_CONFIG_H)

configure_file (
   "${PROJECT_SOURCE_DIR}/cmakeconfig.h.in"
   "${PROJECT_BINARY_DIR}/config.h"
   )

include(GettextTranslate)
GettextTranslateTargets(ALL)

add_subdirectory(src/bin)
add_subdirectory(data)
add_subdirectory(po)

# Make dist
set(CPACK_PACKAGE_VERSION_MAJOR ${ECRIRE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${ECRIRE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${ECRIRE_VERSION_MICRO})
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
   "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
   "/build/;/.git/;.svn/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

find_package(Distcheck)
add_distcheck()
