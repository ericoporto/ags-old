cmake_minimum_required(VERSION 3.13)

set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX ${CMAKE_CURRENT_SOURCE_DIR}/CMake/cxx_flag_overrides.cmake)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")

project(AGS
    VERSION 3.5.1.0
    LANGUAGES CXX C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

option(AGS_NO_MP3_PLAYER "Disable MP3" OFF)
option(AGS_NO_VIDEO_PLAYER "Disable Video" OFF)
option(AGS_BUILTIN_PLUGINS "Built in plugins" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)
set(AGS_BUILD_STR "" CACHE STRING "Engine Build Information")

# Tools like ninja don't have a pseudo-terminal so compilers assume no coloured output
option (AGS_FORCE_COLOURED_OUTPUT "Always produce ANSI-coloured output (GNU/Clang only)." OFF)
if (${AGS_FORCE_COLOURED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED on)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # WIN32 is set by CMake for any Windows platform
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(ANDROID TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(EMSCRIPTEN TRUE)
else()
    message(FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")
endif ()

message( "CMAKE_SYSTEM_NAME:" ${CMAKE_SYSTEM_NAME} )
message( "WIN32:" ${WIN32} )
message( "LINUX:" ${LINUX} )
message( "MACOS:" ${MACOS} )
message( "ANDROID:" ${ANDROID} )
message( "EMSCRIPTEN:" ${EMSCRIPTEN} )

if (WIN32 AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "Windows builds only support 32bit for now")
endif()

if(LINUX)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9.0)
            # without no-pie we can't double-click run ags in Nautilus
            # this should only be needed in newer gcc versions
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-pie -no-pie")
        endif ()
    endif ()
endif (LINUX)


include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_not_supported_reason)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE          TRUE)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO   TRUE)
else()
    message(STATUS "Interprocedural optimisation (IPO/LTO) not supported: <${ipo_not_supported_reason}>")
endif()

if(EMSCRIPTEN)
    set(USE_FLAGS " -s USE_SDL=2  -s USE_FREETYPE=1  -s USE_VORBIS=1  -s USE_OGG=1 ")
    set(AGS_DISABLE_THREADS TRUE)
    if(AGS_DISABLE_THREADS)
        add_compile_definitions("AGS_DISABLE_THREADS=1")
        set(EMSDK_FLAGS "\
        -s ALLOW_MEMORY_GROWTH=1 ")
    else()
        set(EMSDK_FLAGS "\
        -pthread \
		-s INITIAL_MEMORY=536870912 ")
    endif()

    set(EMSDK_FLAGS "${EMSDK_FLAGS} \
        -s ASYNCIFY \
        -O3 \
        -s ALLOW_TABLE_GROWTH=1 \
        -s WASM=1 \
        -s INVOKE_RUN=0 \
		-s EXIT_RUNTIME=1  \
		-s EMULATE_FUNCTION_POINTER_CASTS=1 \
        -s DISABLE_EXCEPTION_CATCHING=0 ")

#    set(EMSDK_FLAGS "${EMSDK_FLAGS} \
#        -g4 \
#        -fsanitize=null -fsanitize-minimal-runtime \
#        --emit-symbol-map \
#        --source-map-base http://0.0.0.0:8000/../ ")

    set(EMSDK_LINKER_FLAGS "\
        -s FULL_ES2=1 \
        -s FORCE_FILESYSTEM=1 -lidbfs.js  \
        -s EXPORTED_FUNCTIONS=['_main'] \
        -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','callMain']")

    set(EMSDK_LINKER_FLAGS "${EMSDK_LINKER_FLAGS} --shell-file ${CMAKE_SOURCE_DIR}/Emscripten/launcher_index.html ")

    SET(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS} ${EMSDK_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS} ${EMSDK_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS} ${EMSDK_FLAGS} ${EMSDK_LINKER_FLAGS}")

    add_compile_options(-sFORCE_FILESYSTEM=1)
    add_compile_options(-fexceptions)

    include_directories(${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${VORBIS_INCLUDE_DIRS} ${OGG_INCLUDE_DIRS})
endif(EMSCRIPTEN)

add_compile_definitions("_FILE_OFFSET_BITS=64")

add_compile_definitions("$<$<CONFIG:DEBUG>:_DEBUG>")
add_compile_definitions("$<$<CONFIG:RELEASE>:NDEBUG>")

if(MSVC)
    add_compile_options(/MP)    # Build with Multiple Processes
    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
    add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)
else()
    add_compile_options(
        -fsigned-char
        -fno-strict-aliasing
        -fwrapv

        -Wall
        -Wextra

        -Wendif-labels
        -Wfloat-equal
        -Wformat
        -Wformat-security
        -Winit-self
        -Winline
        -Wmissing-noreturn
        -Wpointer-arith
        -Wshadow
        -Wundef
        -Wwrite-strings
        -Wunused-result

        # probably need fixing but disable until we have time
        -Wno-unknown-pragmas
        -Wno-deprecated-declarations
        -Wno-unused-parameter
        -Wno-sign-compare
        -Wno-cast-align
        -Wno-cast-qual
        -Wno-missing-declarations
        -Wno-switch-enum
        # -Wlarger-than-4096
        -Wno-redundant-decls

        -Werror=write-strings
        #-Werror=implicit-function-declaration
        #-Werror=unused-result
    )

    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wno-old-style-cast>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Werror=delete-non-virtual-dtor>)

    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wbad-function-cast>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wdeclaration-after-statement>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wno-missing-prototypes>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)

endif()

find_package(PkgConfig)
find_package(Threads)

if (WIN32)
    find_package(DirectX)
elseif (LINUX)
    find_package(X11)
endif()

if(NOT EMSCRIPTEN)
include(FindLocalSDL2)
endif()
include(FindLocalSdlSound)
include(FindLocalOpenAL)
include(FindLocalAllegro)

if(NOT EMSCRIPTEN)
include(FetchOgg)
include(FetchVorbis)
endif()
include(FetchTheora)

add_subdirectory(Common/libsrc/aastr-0.1.1      EXCLUDE_FROM_ALL)
add_subdirectory(Common/libsrc/alfont-2.0.9     EXCLUDE_FROM_ALL)
add_subdirectory(Common/libsrc/freetype-2.1.3   EXCLUDE_FROM_ALL)
add_subdirectory(Common                         EXCLUDE_FROM_ALL)

add_subdirectory(Engine/libsrc/apeg-1.2.1       EXCLUDE_FROM_ALL)
if (ANDROID OR EMSCRIPTEN)
    add_subdirectory(Engine/libsrc/glad-gles2       EXCLUDE_FROM_ALL)
else()
    add_subdirectory(Engine/libsrc/glad             EXCLUDE_FROM_ALL)
endif()
add_subdirectory(Engine/libsrc/hq2x             EXCLUDE_FROM_ALL)
add_subdirectory(Engine/libsrc/libcda-0.5       EXCLUDE_FROM_ALL)

SET(GLM_TEST_ENABLE OFF CACHE BOOL "GLM Build unit tests")
add_subdirectory(Engine/libsrc/glm     EXCLUDE_FROM_ALL)
add_library(GLM::GLM ALIAS glm_static)

add_subdirectory(Engine)

set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ags)
