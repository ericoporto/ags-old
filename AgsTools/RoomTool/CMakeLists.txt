add_library(roomlib)

set_target_properties(roomlib PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS NO
        C_STANDARD 11
        C_EXTENSIONS NO
        )

target_include_directories(roomlib PUBLIC .)

target_sources(roomlib
        PRIVATE
        roomlib.cpp
        roomlib.h
        )

target_link_libraries(compiler PUBLIC AGS::Common)

get_target_property(COMMON_SOURCES roomlib SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${COMMON_SOURCES})

add_library(AGS::Roomlib ALIAS roomlib)

add_executable(roomtool roomtool.cpp ../../libsrc/cxxopts/cxxopts.hpp)
target_include_directories(roomtool PUBLIC ../../libsrc/cxxopts)
set_target_properties(roomtool PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS NO
        C_STANDARD 11
        C_EXTENSIONS NO
        )

target_link_libraries(roomtool PUBLIC AGS::Roomlib)