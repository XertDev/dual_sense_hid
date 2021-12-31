#  Find libhidapi

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_hidapi_hidraw QUIET hidapi-hidraw)
endif()

find_path(
        hidapi_INCLUDE_DIR
        NAMES hidapi.h
        PATH_SUFFIXES
        hidapi
        included
        include/hidapi
        HINTS ${PC_hidapi_hidraw_INCLUDEDIR}
)

find_library(
        hidapi_undecorated_LIBRARY
        NAMES hidapi
        PATH_SUFFIXES
        lib
)

if(CMAKE_SYSTEM MATCHES "Linux")
    find_library(
            hidapi_hidraw_LIBRARY
            NAMES hidapi-hidraw
            HINTS ${PC_hidapi_hidraw_LIBRARY_DIRS}
    )
endif()

if(hidapi_hidraw_LIBRARY)
    set(hidapi_LIBRARY ${hidapi_hidraw_LIBRARY})
elseif(hidapi_undecorated_LIBRARY)
    set(hidapi_LIBRARY ${hidapi_undecorated_LIBRARY})
endif()

#FPHSA

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
        hidapi
        DEFAULT_MSG
        hidapi_INCLUDE_DIR
        hidapi_LIBRARY
)

if(hidapi_FOUND)
    set(hidapi_INCLUDE_DIRS ${hidapi_INCLUDE_DIR})
    set(hidapi_LIBRARIES ${hidapi_LIBRARY})
    if(NOT TARGET hidapi::hidapi)
        add_library(hidapi::hidapi UNKNOWN IMPORTED)
        set_target_properties(
                hidapi::hidapi
                PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES ${hidapi_INCLUDE_DIR}
                IMPORTED_LOCATION ${hidapi_LIBRARY}
        )
    endif()
endif()

mark_as_advanced(hidapi_LIBRARY hidapi_INCLUDE_DIR)