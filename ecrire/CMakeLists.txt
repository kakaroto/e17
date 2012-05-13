cmake_minimum_required(VERSION 2.6)
project(ecrire)
set(VMAJ 0)
set(VMIN 1)
set(VMIC 0)
set(PACKAGE_VERSION ${VMAJ}.${VMIN}.${VMIC})

set(PACKAGE ${CMAKE_PROJECT_NAME})
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

if(DEFINED ECORE_X_FOUND)
   set(HAVE_ECORE_X 1)
endif(DEFINED ECORE_X_FOUND)


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

find_package(Gettext)
if (GETTEXT_FOUND)
   set(GettextTranslate_ALL true)
   set(GettextTranslate_GMO_BINARY true)
   include(GettextTranslate)
   set(ENABLE_NLS 1)
endif (GETTEXT_FOUND)

configure_file (
   "${PROJECT_SOURCE_DIR}/cmakeconfig.h.in"
   "${PROJECT_BINARY_DIR}/config.h"
   )

add_subdirectory(src/bin)
add_subdirectory(data)
add_subdirectory(po)

# Make dist
set(CPACK_PACKAGE_VERSION_MAJOR ${VMAJ})
set(CPACK_PACKAGE_VERSION_MINOR ${VMIN})
set(CPACK_PACKAGE_VERSION_PATCH ${VMIC})
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
   "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
   "${CMAKE_BINARY_DIR};/.git/;.svn/;.swp$;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

include(MakeDistcheck)
add_distcheck()
