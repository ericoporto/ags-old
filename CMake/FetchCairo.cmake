FetchContent_Declare(
        cairo_content
        URL https://cairographics.org/snapshots/cairo-1.17.4.tar.xz
        URL_HASH MD5=bf9d0d324ecbd350d0e9308125fa4ce0
)

FetchContent_GetProperties(cairo_content)
if(NOT cairo_content_POPULATED)
    FetchContent_Populate(cairo_content)

    file(COPY CMake/Extra/Cairo/config.h.cmake DESTINATION ${cairo_content_SOURCE_DIR})
    file(COPY CMake/Extra/Cairo/features.h.cmake DESTINATION ${cairo_content_SOURCE_DIR})
    file(COPY CMake/Extra/Cairo/Configure_config.cmake DESTINATION ${cairo_content_SOURCE_DIR})
    file(COPY CMake/Extra/Cairo/Configure_features.cmake DESTINATION ${cairo_content_SOURCE_DIR})
    file(COPY CMake/Extra/Cairo/CMakeLists.txt DESTINATION ${cairo_content_SOURCE_DIR})
    file(COPY CMake/Extra/Cairo/src/CMakeLists.txt DESTINATION ${cairo_content_SOURCE_DIR}/src/)
    file(COPY CMake/Extra/Cairo/src/sources.cmake DESTINATION ${cairo_content_SOURCE_DIR}/src/)

    add_subdirectory(${cairo_content_SOURCE_DIR} ${cairo_content_BINARY_DIR})
endif()



