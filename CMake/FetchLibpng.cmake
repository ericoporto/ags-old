FetchContent_Declare(
        libpng_content
        URL https://github.com/glennrp/libpng/archive/a37d4836519517bdce6cb9d956092321eca3e73b.tar.gz
        URL_HASH MD5=ab9aae9db44464a908993e5dda84e18f
)

FetchContent_GetProperties(libpng_content)
if(NOT libpng_content_POPULATED)
    FetchContent_Populate(libpng_content)

    set(SKIP_INSTALL_EXPORT ON CACHE BOOL "" FORCE)
    set(PNG_SHARED OFF CACHE BOOL "" FORCE)
    set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
    set(PNG_TESTS OFF CACHE BOOL "" FORCE)
    set(PNG_STATIC ON CACHE BOOL "" FORCE)
    set(PNG_BUILD_ZLIB ON CACHE BOOL "" FORCE)
    add_subdirectory(${libpng_content_SOURCE_DIR} ${libpng_content_BINARY_DIR})

    add_library(Libpng::Libpng ALIAS png_static)
    add_library(libpng ALIAS png_static)
    set(LIBPNG_LIBRARY libpng)
    set(LIBPNG_INCLUDE_DIR ${libpng_content_SOURCE_DIR})
endif()


