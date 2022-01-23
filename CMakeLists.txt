cmake_minimum_required(VERSION 3.21)
project(
        dual_sense
        VERSION 0.1
        DESCRIPTION "Dual Sense Gamepad"
        LANGUAGES CXX
)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(ENABLE_TESTING "Enable building of test" FALSE)

# Warnings config
add_library(warnings_target INTERFACE)
if(MSVC)
    target_compile_options(
            warnings_target
            INTERFACE
            /W4
            /WX "/permissive-"
    )
else()
    target_compile_options(
            warnings_target
            INTERFACE
            -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wdouble-promotion
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Werror
    )
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(
                warnings_target
                INTERFACE
                -Waddress-of-packed-member
                -Wmisleading-indentation
                -Wduplicated-cond
                -Wduplicated-branches
                -Wlogical-op
                -Wuseless-cast
        )
    endif()
endif()

# Project options config

add_library(options_target INTERFACE)
target_compile_features(options_target INTERFACE cxx_std_20)

if(!MSVC)
    option(ENABLE_COVERAGE "Enable coverage reporting (gss/clang)" FALSE)

    if(ENABLE_COVERAGE)
        target_compile_options(options_target INTERFACE --coverage -O0 -g)
        target_link_libraries(options_targer INTERFACE --coverage)
    endif()
endif()

# External
find_package(hidapi REQUIRED)

# Library
add_library(dual_sense)

target_include_directories(
        dual_sense
        PUBLIC
        include
)
target_link_libraries(
        dual_sense
        PRIVATE
        options_target
        warnings_target
)

target_link_libraries(
        dual_sense
        PRIVATE
        hidapi::hidapi
)

# Sources

target_sources(
        dual_sense
        PRIVATE
        include/dual_sense/enums.hpp
        include/dual_sense/gamepad.hpp
        include/dual_sense/device_info.hpp
        include/dual_sense/state.hpp
        include/dual_sense/calibration.hpp
        include/dual_sense/detail/report.hpp
        include/dual_sense/detail/helper.hpp

        src/gamepad.cpp
)

# Examples
option(ENABLE_EXAMPLES "Enable examples building" FALSE)

if(ENABLE_EXAMPLES)
    add_subdirectory(example)
endif()

option(ENABLE_DOCS "Enable docs" FALSE)
if(ENABLE_DOCS)
    find_package(Doxygen REQUIRED)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

    configure_file(docs/Doxyfile ${DOXYGEN_OUT})

    include(FetchContent)
    FetchContent_Declare(
            mcss
            GIT_REPOSITORY git://github.com/mosra/m.css
    )
    FetchContent_MakeAvailable(mcss)

    add_custom_target(
            docs
            COMMAND ${Python3_EXECUTABLE} ${mcss_SOURCE_DIR}/documentation/doxygen.py ${DOXYGEN_OUT} --debug
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
    )

endif()
