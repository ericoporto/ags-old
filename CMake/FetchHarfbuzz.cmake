FetchContent_Declare(
    harfbuzz_content
    URL https://github.com/harfbuzz/harfbuzz/releases/download/2.8.1/harfbuzz-2.8.1.tar.xz
    URL_HASH MD5=10d34b59062e526c0ddeded785c26730
)

FetchContent_GetProperties(harfbuzz_content)
if(NOT harfbuzz_content)
  FetchContent_Populate(harfbuzz_content)

  file(COPY CMake/Extra/Harfbuzz/CMakeLists.txt DESTINATION "${harfbuzz_content_SOURCE_DIR}")

  set(HB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(HB_HAVE_FREETYPE ON CACHE BOOL "" FORCE)
  message("Including harfbuzz ...")
  message("harfbuzz_content_SOURCE_DIR: ${harfbuzz_content_SOURCE_DIR}")
  add_subdirectory(${harfbuzz_content_SOURCE_DIR} ${harfbuzz_content_BINARY_DIR} EXCLUDE_FROM_ALL)
  include_directories(${harfbuzz_content_SOURCE_DIR}/src/)
  #add_library(harfbuzz-interface INTERFACE)
  #add_dependencies(harfbuzz-interface ${harfbuzz_content_SOURCE_DIR})
  #target_link_libraries(harfbuzz-interface INTERFACE harfbuzz)
  add_library(Harfbuzz::Harfbuzz ALIAS harfbuzz)
  target_link_libraries(harfbuzz Freetype::Freetype)
endif()