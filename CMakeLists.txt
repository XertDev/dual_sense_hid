cmake_minimum_required(VERSION 3.26)
project(
        dual_sense_hid
        VERSION 0.1
        DESCRIPTION "Dual Sense Gamepad via HID api"
        LANGUAGES CXX
)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
target_compile_features(options_target INTERFACE cxx_std_23)

if(!MSVC)
    option(ENABLE_COVERAGE "Enable coverage reporting (gss/clang)" FALSE)

    if(ENABLE_COVERAGE)
        target_compile_options(options_target INTERFACE --coverage -O0 -g)
        target_link_libraries(options_target INTERFACE --coverage)
    endif()
endif()

# External
find_package(hidapi REQUIRED)

# Library
add_library(dual_sense_hid)

target_include_directories(
        dual_sense_hid
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/>
)

target_link_libraries(
        dual_sense_hid
        PRIVATE
        $<BUILD_LOCAL_INTERFACE:options_target>
        $<BUILD_LOCAL_INTERFACE:warnings_target>
)

target_link_libraries(
        dual_sense_hid
        PRIVATE
        hidapi::hidapi
)

# Sources

target_sources(
        dual_sense_hid
        PRIVATE
        include/dual_sense_hid/enums.hpp
        include/dual_sense_hid/gamepad.hpp
        include/dual_sense_hid/device_info.hpp
        include/dual_sense_hid/state.hpp
        include/dual_sense_hid/calibration.hpp
        include/dual_sense_hid/detail/report_input.hpp
        include/dual_sense_hid/detail/report_output.hpp
        include/dual_sense_hid/detail/crc32.hpp
        include/dual_sense_hid/detail/helper.hpp

        src/gamepad.cpp
        src/detail/crc32.cpp
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS dual_sense_hid EXPORT ${PROJECT_NAME}Targets)
install(DIRECTORY include/dual_sense_hid DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/dual_sense_hidConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

if(${CMAKE_VERSION} VERSION_GREATER 3.13)
    write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
else()
    write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" COMPATIBILITY SameMajorVersion)
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
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
            GIT_REPOSITORY https://github.com/mosra/m.css.git
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

option(ENABLE_TESTS "Enable tests" FALSE)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
