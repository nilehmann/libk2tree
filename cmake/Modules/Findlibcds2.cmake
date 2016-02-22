include(LibFindMacros)

libfind_pkg_check_modules(libcds2_PKGCONF cds2)

find_path(libcds2_INCLUDE_DIR
  NAMES libcds.h
  PATHS ${libcds2_PKGCONF_INCLUDE_DIRS}
)

find_library(libcds2_LIBRARY
  NAMES cds2
  PATHS ${libcds2_PKGCONF_LIBRARY_DIRS}
)

set(libcds2_PROCESS_INCLUDES LIBCDS2_INCLUDE_DIR)
set(libcds2_PROCESS_LIBS LIBCDS2_LIBRARY)
libfind_process(libcds2)
