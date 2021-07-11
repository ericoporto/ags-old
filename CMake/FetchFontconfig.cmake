FetchContent_Declare(
        fontconfig_content
        URL https://gitlab.freedesktop.org/fontconfig/fontconfig/-/archive/2.13.94/fontconfig-2.13.94.tar.gz
        URL_HASH MD5=5289f54a09a805657b8344a8414f64cf
)

FetchContent_GetProperties(fontconfig_content)
if(NOT fontconfig_content_POPULATED)
    FetchContent_Populate(fontconfig_content)

    file(COPY CMake/Extra/Fontconfig/config.h.cmake DESTINATION ${fontconfig_content_SOURCE_DIR})
    file(COPY CMake/Extra/Fontconfig/ConfigureChecks.cmake DESTINATION ${fontconfig_content_SOURCE_DIR})
    file(COPY CMake/Extra/Fontconfig/fonts.conf.cmake DESTINATION ${fontconfig_content_SOURCE_DIR})
    file(COPY CMake/Extra/Fontconfig/CMakeLists.txt DESTINATION ${fontconfig_content_SOURCE_DIR})
    file(COPY CMake/Extra/Fontconfig/fontconfig/CMakeLists.txt DESTINATION ${fontconfig_content_SOURCE_DIR}/fontconfig/)
    file(COPY CMake/Extra/Fontconfig/conf.d/CMakeLists.txt DESTINATION ${fontconfig_content_SOURCE_DIR}/conf.d/)
    file(COPY CMake/Extra/Fontconfig/src/CMakeLists.txt DESTINATION ${fontconfig_content_SOURCE_DIR}/src/)

    add_subdirectory(${fontconfig_content_SOURCE_DIR} ${fontconfig_content_BINARY_DIR} EXCLUDE_FROM_ALL)

endif()


