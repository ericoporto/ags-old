FetchContent_Declare(
        harfbuzz_content
        URL https://github.com/harfbuzz/harfbuzz/releases/download/2.8.1/harfbuzz-2.8.1.tar.xz
        URL_HASH MD5=10d34b59062e526c0ddeded785c26730
)

FetchContent_Declare(
        freetype_content
        URL https://github.com/freetype/freetype/archive/refs/tags/VER-2-10-4.tar.gz
        URL_HASH MD5=00496c4147705ec55c9cac47ba53049b
)

FetchContent_GetProperties(harfbuzz_content)
FetchContent_GetProperties(freetype_content)
if(NOT harfbuzz_content AND NOT freetype_content)
    FetchContent_Populate(harfbuzz_content)
    FetchContent_Populate(freetype_content)


    set(SAVED_SKIP_INSTALL_ALL "${SKIP_INSTALL_ALL}")
    set(SKIP_INSTALL_ALL ON)

    file(COPY CMake/Extra/Harfbuzz/CMakeLists.txt DESTINATION "${harfbuzz_content_SOURCE_DIR}")

    set(FREETYPE_LIBRARY freetype)
    set(FREETYPE_LIBRARIES FreeType::FreeType)
    set(FREETYPE_INCLUDE_DIRS ${freetype_content_SOURCE_DIR}/include/)

    set(HARFBUZZ_LIBRARY harfbuzz)
    set(HARFBUZZ_LIBRARIES HarfBuzz::HarfBuzz)
    set(HARFBUZZ_INCLUDE_DIRS ${harfbuzz_content_SOURCE_DIR}/src/)

    set(HB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(HB_HAVE_FREETYPE ON CACHE BOOL "" FORCE)
    message("Including harfbuzz ...")
    message("harfbuzz_content_SOURCE_DIR: ${harfbuzz_content_SOURCE_DIR}")
    add_subdirectory(${harfbuzz_content_SOURCE_DIR} ${harfbuzz_content_BINARY_DIR} EXCLUDE_FROM_ALL)
    include_directories(${harfbuzz_content_SOURCE_DIR}/src/)

    set(SAVED_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
    set(SAVED_CMAKE_DISABLE_FIND_PACKAGE_ZLIB_LIBS "${CMAKE_DISABLE_FIND_PACKAGE_ZLIB}")
    set(SAVED_CMAKE_DISABLE_FIND_PACKAGE_BZip2_LIBS "${CMAKE_DISABLE_FIND_PACKAGE_BZip2}")
    #  set(SAVED_CMAKE_DISABLE_FIND_PACKAGE_PNG_LIBS "${CMAKE_DISABLE_FIND_PACKAGE_PNG}")
    set(SAVED_CMAKE_DISABLE_FIND_PACKAGE_BrotliDec "${CMAKE_DISABLE_FIND_PACKAGE_BrotliDec}")

    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB ON)
    set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 ON)
    #  set(CMAKE_DISABLE_FIND_PACKAGE_PNG ON)
    set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec ON)

    message("Including FreeType ...")

    set(FT_WITH_HARFBUZZ ON CACHE BOOL "" FORCE)
    set(FT_CONFIG_OPTION_USE_PNG ON CACHE BOOL "" FORCE)
    add_subdirectory(${freetype_content_SOURCE_DIR} ${freetype_content_BINARY_DIR} EXCLUDE_FROM_ALL)
    include_directories(${freetype_content_SOURCE_DIR}/include/)
    add_library(FreeType::FreeType ALIAS freetype)
    add_library(Freetype::Freetype ALIAS freetype)

    set(BUILD_SHARED_LIBS "${SAVED_BUILD_SHARED_LIBS}")
    set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB "${SAVED_CMAKE_DISABLE_FIND_PACKAGE_ZLIB_LIBS}")
    set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 "${SAVED_CMAKE_DISABLE_FIND_PACKAGE_BZip2_LIBS}")
    #  set(CMAKE_DISABLE_FIND_PACKAGE_PNG "${SAVED_CMAKE_DISABLE_FIND_PACKAGE_PNG_LIBS}")
    set(CMAKE_DISABLE_FIND_PACKAGE_BrotliDec "${SAVED_CMAKE_DISABLE_FIND_PACKAGE_BrotliDec}")
    set(SKIP_INSTALL_ALL "${SAVED_SKIP_INSTALL_ALL}")

    add_library(Harfbuzz::Harfbuzz ALIAS harfbuzz)
    add_library(HarfBuzz::HarfBuzz ALIAS harfbuzz)
    add_library(HarfBuzz ALIAS harfbuzz)
    target_link_libraries(harfbuzz Freetype::Freetype)
endif()
