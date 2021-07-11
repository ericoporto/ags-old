FetchContent_Declare(
        zlib_content
        URL https://github.com/madler/zlib/archive/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz
        URL_HASH MD5=4df8ccfbd8f637d556f56da8ad348c19
)

FetchContent_GetProperties(zlib_content)
if(NOT zlib_content_POPULATED)
    FetchContent_Populate(zlib_content)

    add_subdirectory(${zlib_content_SOURCE_DIR} ${zlib_content_BINARY_DIR})

    file(GLOB ZLIB_INCLUDE_FILES ${zlib_content_SOURCE_DIR}/*.h)
    file(GLOB ZLIB_BIN_INCLUDE_FILES ${zlib_content_BINARY_DIR}/*.h)
    foreach(_FNAME ${ZLIB_BIN_INCLUDE_FILES})
        get_filename_component(_INCNAME ${_FNAME} NAME)
        list(REMOVE_ITEM ZLIB_INCLUDE_FILES ${zlib_content_SOURCE_DIR}/${_INCNAME})
    endforeach()
    list(APPEND ZLIB_INCLUDE_FILES ${ZLIB_BIN_INCLUDE_FILES})

    if(NOT EXISTS "${zlib_content_BINARY_DIR}/include")
        file(MAKE_DIRECTORY "${zlib_content_BINARY_DIR}/include")
        file(COPY ${ZLIB_INCLUDE_FILES}  DESTINATION "${zlib_content_BINARY_DIR}/include/")
    endif()

    add_library(Zlib::Zlib ALIAS zlibstatic)
    set(ZLIB_LIBRARY Zlib::Zlib)
    set(ZLIB_LIBRARIES Zlib::Zlib)
    set(ZLIB_INCLUDE_DIRS "${zlib_content_BINARY_DIR}/include/")
    set(ZLIB_INCLUDE_DIR "${zlib_content_BINARY_DIR}/include/")
endif()


