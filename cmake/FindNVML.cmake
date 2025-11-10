# FindNVML.cmake
# Tries to locate NVIDIA Management Library (NVML)
# Provides:
#   NVML_FOUND
#   NVML_INCLUDE_DIR
#   NVML_LIBRARY
#   NVML::NVML  (imported target)

include(FindPackageHandleStandardArgs)

find_path(NVML_INCLUDE_DIR
    NAMES nvml.h
    PATHS
        /usr/include
        /usr/local/include
        ENV CPATH
        ENV C_INCLUDE_PATH
    DOC "Path to NVML header"
)

find_library(NVML_LIBRARY
    NAMES nvidia-ml
    PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        ENV LD_LIBRARY_PATH
    DOC "Path to NVML shared library"
)

if (NOT NVML_INCLUDE_DIR OR NOT NVML_LIBRARY)
    find_package(CUDAToolkit QUIET)
    if (CUDAToolkit_FOUND)
        message(STATUS "Falling back to CUDAToolkit for NVML detection")
        find_path(NVML_INCLUDE_DIR
            NAMES nvml.h
            PATHS ${CUDAToolkit_INCLUDE_DIRS}
        )
        find_library(NVML_LIBRARY
            NAMES nvidia-ml
            PATHS ${CUDAToolkit_LIBRARY_DIR}
        )
    endif()
endif()

find_package_handle_standard_args(NVML
    REQUIRED_VARS NVML_INCLUDE_DIR NVML_LIBRARY
    HANDLE_COMPONENTS
)

if (NVML_FOUND)
    add_library(NVML::NVML UNKNOWN IMPORTED)
    set_target_properties(NVML::NVML PROPERTIES
        IMPORTED_LOCATION "${NVML_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${NVML_INCLUDE_DIR}"
    )
endif()
