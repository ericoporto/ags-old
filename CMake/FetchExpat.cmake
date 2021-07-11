FetchContent_Declare(
        expat_content
        URL https://github.com/libexpat/libexpat/archive/ee695c78954edded24e5674722869daa69fd230a.tar.gz
        URL_HASH MD5=0607ac983ccdfb3f89ae8c99cd36a78e
)

FetchContent_GetProperties(expat_content)
if(NOT expat_content_POPULATED)
    FetchContent_Populate(expat_content)

    add_subdirectory(${expat_content_SOURCE_DIR}/expat/ ${expat_content_BINARY_DIR} EXCLUDE_FROM_ALL)

    set(EXPAT_LIBRARY expat)
    set(EXPAT_INCLUDE_DIR ${expat_content_SOURCE_DIR}/expat/lib/)
endif()


