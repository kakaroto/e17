cmake_minimum_required (VERSION 2.6)
project (ecrire)
set (ecrire_VERSION_MAJOR 0)
set (ecrire_VERSION_MINOR 1)
set (PACKAGE ${CMAKE_PROJCT_NAME} )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

configure_file (
   "${PROJECT_SOURCE_DIR}/cmakeconfig.h.in"
   "${PROJECT_BINARY_DIR}/config.h"
   )

include_directories ("${PROJECT_BINARY_DIR}")

find_package(Eina REQUIRED)
find_package(Eet REQUIRED)
find_package(Evas REQUIRED)
find_package(Ecore REQUIRED)
find_package(Edje REQUIRED)
find_package(Efreet REQUIRED)
find_package(Elementary REQUIRED)

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

add_subdirectory(src/bin)
add_subdirectory(data)
add_subdirectory(po)
