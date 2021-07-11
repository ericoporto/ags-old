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

    file(GLOB PNGLIB_INCLUDE_FILES ${libpng_content_SOURCE_DIR}/*.h)
    file(GLOB PNGLIB_BIN_INCLUDE_FILES ${libpng_content_BINARY_DIR}/*.h)
    foreach(_FNAME ${PNGLIB_BIN_INCLUDE_FILES})
        get_filename_component(_INCNAME ${_FNAME} NAME)
        list(REMOVE_ITEM PNGLIB_INCLUDE_FILES ${libpng_content_SOURCE_DIR}/${_INCNAME})
    endforeach()
    list(APPEND PNGLIB_INCLUDE_FILES ${PNGLIB_BIN_INCLUDE_FILES})

    if(NOT EXISTS "${libpng_content_BINARY_DIR}/include")
        file(MAKE_DIRECTORY "${libpng_content_BINARY_DIR}/include")
        file(COPY ${PNGLIB_INCLUDE_FILES}  DESTINATION "${libpng_content_BINARY_DIR}/include/")
    endif()


    add_library(Libpng::Libpng ALIAS png_static)
    add_library(libpng ALIAS png_static)
    set(LIBPNG_LIBRARY libpng)
    set(PNG_LIBRARY Libpng::Libpng)
    set(PNG_LIBRARIES Libpng::Libpng)
    set(PNG_INCLUDE_DIR "${libpng_content_BINARY_DIR}/include/")
    set(PNG_INCLUDE_DIRS ${PNG_INCLUDE_DIR})
    set(PNG_PNG_INCLUDE_DIR ${PNG_INCLUDE_DIR})
    set(LIBPNG_INCLUDE_DIR ${PNG_INCLUDE_DIR})
endif()


