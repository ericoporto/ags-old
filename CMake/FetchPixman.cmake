FetchContent_Declare(
        pixman_content
        URL https://www.cairographics.org/releases/pixman-0.40.0.tar.gz
        URL_HASH MD5=73858c0862dd9896fb5f62ae267084a4
)

FetchContent_GetProperties(pixman_content)
if(NOT pixman_content_POPULATED)
    FetchContent_Populate(pixman_content)

    set(PIXMAN_BUILD_STATIC ON)
    set(PIXMAN_BUILD_SHARED OFF)

    file(MAKE_DIRECTORY ${pixman_content_SOURCE_DIR}/cmake)
    file(COPY CMake/Extra/Pixman/CMakeLists.txt DESTINATION ${pixman_content_SOURCE_DIR})
    file(COPY CMake/Extra/Pixman/pixman/CMakeLists.txt DESTINATION ${pixman_content_SOURCE_DIR}/pixman/)
    file(COPY CMake/Extra/Pixman/cmake/arch_configure.cmake DESTINATION ${pixman_content_SOURCE_DIR}/cmake/)
    file(COPY CMake/Extra/Pixman/cmake/arch_detect.cmake DESTINATION ${pixman_content_SOURCE_DIR}/cmake/)
    file(COPY CMake/Extra/Pixman/cmake/cmake_package.cmake DESTINATION ${pixman_content_SOURCE_DIR}/cmake/)
    file(COPY CMake/Extra/Pixman/cmake/config_configure.cmake DESTINATION ${pixman_content_SOURCE_DIR}/cmake/)
    file(COPY CMake/Extra/Pixman/cmake/config_source.cmake DESTINATION ${pixman_content_SOURCE_DIR}/cmake/)
    file(COPY CMake/Extra/Pixman/cmake/PixmanConfig.cmake DESTINATION ${pixman_content_SOURCE_DIR}/cmake/)

    add_subdirectory("${pixman_content_SOURCE_DIR}/" ${pixman_content_BINARY_DIR} EXCLUDE_FROM_ALL)

    add_library(pixman-interface INTERFACE)
    target_link_libraries(pixman-interface INTERFACE pixman-1_static)

    add_library(Pixman::Pixman  ALIAS pixman-interface)
    target_include_directories(pixman-interface INTERFACE
            "${PROJECT_BINARY_DIR}"
            "${PROJECT_BINARY_DIR}/libsrc"
            "${PROJECT_BINARY_DIR}/libsrc/pixman/pixman"
            "${PROJECT_SOURCE_DIR}/libsrc/pixman/pixman"
            )

    set(PIXMAN_INCLUDE_DIRS
            "${PROJECT_BINARY_DIR}"
            "${PROJECT_BINARY_DIR}/libsrc"
            "${PROJECT_BINARY_DIR}/libsrc/pixman/pixman"
            "${PROJECT_SOURCE_DIR}/libsrc/pixman/pixman")

    set(PIXMAN_INCLUDE_DIR ${PIXMAN_INCLUDE_DIRS})
    set(PIXMAN_LIBRARY Pixman::Pixman)
    set(PIXMAN_LIBRARIES Pixman::Pixman)
endif()




