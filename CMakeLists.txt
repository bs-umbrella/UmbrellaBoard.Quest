cmake_minimum_required(VERSION 3.22)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/quest.cmake)
project(umbrella-board VERSION ${PACKAGE_VERSION})
include(${EXTERN_DIR}/includes/kaleb/shared/cmake/assets.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB_RECURSE c_files ${SRC_DIR}/*.c)
file(GLOB_RECURSE cpp_files ${SRC_DIR}/*.cpp)

add_library(
    umbrella-board
    SHARED
    ${c_files}
    ${cpp_files}
)

# assets include
add_assets(
    umbrella-board-assets
    STATIC
    ${CMAKE_CURRENT_LIST_DIR}/assets
    ${INCLUDE_DIR}/assets.hpp
)

target_compile_options(umbrella-board PRIVATE -Wno-invalid-offsetof)
target_compile_options(umbrella-board PRIVATE -DNEED_UNSAFE_CSHARP)
target_compile_options(umbrella-board PRIVATE -fdeclspec)
target_compile_options(umbrella-board PRIVATE -DUNITY_2021)
target_compile_options(umbrella-board PRIVATE -DHAS_CODEGEN)
target_compile_options(umbrella-board PRIVATE -DFMT_HEADER_ONLY)
target_compile_options(umbrella-board PRIVATE -DMOD_ID="BeatSaberIRC")
target_compile_options(umbrella-board PRIVATE -DVERSION="${PACKAGE_VERSION}")

string(JSON DEPENDENCIES_LENGTH LENGTH ${PACKAGE_JSON} restoredDependencies)
math(EXPR DEPENDENCIES_LENGTH "${DEPENDENCIES_LENGTH} - 1")

foreach(DEP_IDX RANGE 0 ${DEPENDENCIES_LENGTH} )
    string(JSON DEPENDENCY GET ${PACKAGE_JSON} restoredDependencies ${DEP_IDX})
    string(JSON INTERNAL_DEPENDENCY GET ${DEPENDENCY} dependency)
    string(JSON DEPENDENCY_ID GET ${INTERNAL_DEPENDENCY} id)

    string(COMPARE EQUAL "${DEPENDENCY_ID}" "kaleb" IS_KALEB)

    if (${IS_KALEB})
        string(JSON KALEB_VERSION GET ${DEPENDENCY} version)
    endif()
endforeach()

target_compile_options(umbrella-board PRIVATE -DKALEB_VERSION="${KALEB_VERSION}")
target_include_directories(umbrella-board PRIVATE ${INCLUDE_DIR})
target_include_directories(umbrella-board PRIVATE ${SHARED_DIR})
target_include_directories(umbrella-board PRIVATE ${EXTERN_DIR}/includes/bs-cordl/include)
target_include_directories(umbrella-board PRIVATE ${EXTERN_DIR}/includes)
target_include_directories(umbrella-board PRIVATE ${CMAKE_CURRENT_LIST_DIR}/vcpkg_installed/arm64-android/include)
target_include_directories(umbrella-board SYSTEM PRIVATE ${EXTERN_DIR}/includes/paper/shared/utfcpp/source)
target_include_directories(umbrella-board SYSTEM PRIVATE ${EXTERN_DIR}/includes/fmt/fmt/include/)
target_include_directories(umbrella-board SYSTEM PRIVATE ${EXTERN_DIR}/includes/libil2cpp/il2cpp/libil2cpp)
target_include_directories(umbrella-board SYSTEM PRIVATE ${EXTERN_DIR}/includes/beatsaber-hook/shared/rapidjson/include/)
target_compile_options(umbrella-board PUBLIC -fdeclspec)
target_compile_definitions(umbrella-board PRIVATE FMT_HEADER_ONLY)

# get all .so and .a files from the extern dir to link to
file(GLOB so_files ${EXTERN_DIR}/libs/lib*.so)
file(GLOB a_files ${EXTERN_DIR}/libs/lib*.a)

target_link_libraries(umbrella-board PRIVATE -llog -lz ${so_files} ${a_files} umbrella-board-assets)

add_custom_command(
    TARGET umbrella-board POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    ${CMAKE_CURRENT_BINARY_DIR}/debug
    COMMENT "Ensure debug dir exists"
)

add_custom_command(
    TARGET umbrella-board POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
    ${CMAKE_CURRENT_BINARY_DIR}/libumbrella-board.so ${CMAKE_CURRENT_BINARY_DIR}/debug/libumbrella-board.so
    COMMENT "move normal binary into debug folder"
)

add_custom_command(
    TARGET umbrella-board POST_BUILD
    COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
    ${CMAKE_CURRENT_BINARY_DIR}/debug/libumbrella-board.so -o ${CMAKE_CURRENT_BINARY_DIR}/libumbrella-board.so
    COMMENT "Strip binary"
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/debug)
foreach(bin_file IN LISTS so_list a_list)
    cmake_path(GET bin_file FILENAME file_name)

    add_custom_command(
        TARGET umbrella-board POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${bin_file} ${CMAKE_CURRENT_BINARY_DIR}/debug/${file_name}
        COMMENT "Copy so files for ndk stack"
    )

    add_custom_command(
        TARGET umbrella-board POST_BUILD
        COMMAND ${CMAKE_STRIP} -g -S -d --strip-all
        ${bin_file} -o ${CMAKE_CURRENT_BINARY_DIR}/${file_name}
        COMMENT "Strip debug symbols from the dependencies"
    )
endforeach()
