cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project(nealog LANGUAGES CXX)

option(NEALOG_HEADERONLY "When OFF is compiled into a static lib. Default=OFF" OFF)

message(STATUS "NEALOG_HEADERONLY=${NEALOG_HEADERONLY}")
set(nealog_HEADERONLY ${NEALOG_HEADERONLY})

function(enable_precompiled_headers_if_supported target visibility)
    if(${CMAKE_VERSION} VERSION_EQUAL "3.16.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.16.0")
        message(STATUS "CMAKE_VERSION is greater 3.16.0. PCH is active")
        target_precompile_headers(${target} ${visibility} ${CMAKE_CURRENT_LIST_DIR}/src/pch.h)
    else()
        message(STATUS "CMAKE_VERSION is less 3.16.0. PCH is active")
    endif()
endfunction()

# ==================
# logger lib
# ==================
add_subdirectory(thirdparty/fmt)

if(nealog_HEADERONLY)
    # header only --------------------------------
    add_library(nealog_ho INTERFACE)
    target_include_directories(nealog_ho INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include/"
                                                   "${CMAKE_CURRENT_LIST_DIR}/src/")

    target_compile_definitions(nealog_ho INTERFACE NL_INLINE=inline
        NEALOG_HEADERONLY=1 FMT_HEADER_ONLY=1)

    enable_precompiled_headers_if_supported(nealog_ho INTERFACE)

    add_library(nealog::headeronly ALIAS nealog_ho)
    target_link_libraries(nealog_ho INTERFACE fmt)
else()
    # static --------------------------------------
    add_library(nealog)
    set_target_properties(nealog PROPERTIES LINKER_LANGUAGE CXX)
    enable_precompiled_headers_if_supported(nealog PUBLIC)
    target_include_directories(nealog PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/")
    message(STATUS "${CMAKE_CURRENT_LIST_DIR}/include/")
    target_compile_definitions(nealog PRIVATE NL_INLINE=)
    target_link_libraries(nealog PUBLIC fmt)
    add_subdirectory(src)
endif()

# ==================
# test lib
# ==================
add_subdirectory(thirdparty/catch2)
add_subdirectory(thirdparty/trompeloeil)
add_subdirectory(test)
