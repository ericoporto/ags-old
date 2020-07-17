add_library(compiler)

set_target_properties(compiler PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS NO
        C_STANDARD 11
        C_EXTENSIONS NO
        )

target_include_directories(compiler PUBLIC .)

target_sources(compiler
        PRIVATE
        fmem.cpp
        fmem.h
        script/cc_compiledscript.cpp
        script/cc_compiledscript.h
        script/cc_internallist.cpp
        script/cc_internallist.h
        script/cc_macrotable.cpp
        script/cc_macrotable.h
        script/cc_symboldef.h
        script/cc_symboltable.cpp
        script/cc_symboltable.h
        script/cc_treemap.cpp
        script/cc_treemap.h
        script/cc_variablesymlist.h
        script/cs_compiler.cpp
        script/cs_compiler.h
        script/cs_parser.cpp
        script/cs_parser.h
        script/cs_parser_common.cpp
        script/cs_parser_common.h
        script/cs_prepro.cpp
        script/cs_prepro.h
        )

target_link_libraries(compiler PUBLIC AGS::Common)

get_target_property(COMMON_SOURCES compiler SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${COMMON_SOURCES})

add_library(AGS::Compiler ALIAS compiler)

add_executable(agscc agscc.cpp ../libsrc/cxxopts/cxxopts.hpp)
target_include_directories(agscc PUBLIC ../libsrc/cxxopts)
set_target_properties(agscc PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS NO
        C_STANDARD 11
        C_EXTENSIONS NO
        )

target_link_libraries(agscc PUBLIC AGS::Compiler)
