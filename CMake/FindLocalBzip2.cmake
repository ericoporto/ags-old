add_subdirectory(libsrc/bzip2 EXCLUDE_FROM_ALL)

set(BZIP2_LIBRARIES BZip2::BZip2)
set(BZIP2_INCLUDE_DIR libsrc/bzip2)
