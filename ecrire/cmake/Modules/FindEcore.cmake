# - Try to find ecore
# Once done this will define
#  ECORE_FOUND - System has ecore
#  ECORE_INCLUDE_DIRS - The ecore include directories
#  ECORE_LIBRARIES - The libraries needed to use ecore
#  ECORE_DEFINITIONS - Compiler switches required for using ecore

find_package(PkgConfig)
pkg_check_modules(PC_LIBECORE QUIET ecore)
set(ECORE_DEFINITIONS ${PC_LIBECORE_CFLAGS_OTHER})

find_path(ECORE_INCLUDE_DIR Ecore.h
          HINTS ${PC_LIBECORE_INCLUDEDIR} ${PC_LIBECORE_INCLUDE_DIRS}
          PATH_SUFFIXES ecore )

find_library(ECORE_LIBRARY NAMES ecore
             HINTS ${PC_LIBECORE_LIBDIR} ${PC_LIBECORE_LIBRARY_DIRS} )

set(ECORE_LIBRARIES ${ECORE_LIBRARY} )
set(ECORE_INCLUDE_DIRS ${ECORE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ECORE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ecore  DEFAULT_MSG
                                  ECORE_LIBRARY ECORE_INCLUDE_DIR)

mark_as_advanced(ECORE_INCLUDE_DIR ECORE_LIBRARY )
