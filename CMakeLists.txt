project(EMA)
cmake_minimum_required(VERSION 3.17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Threads
find_package(Threads REQUIRED)

# NVML
find_package(NVML)

# MQTT
find_package(Mosquitto)

# hashmap
add_library(
    hashmap STATIC
    EMA/ext/c-hashmap/map.c
    EMA/ext/c-hashmap/map.h
)
set_property(TARGET hashmap PROPERTY POSITION_INDEPENDENT_CODE ON)

# EMA
add_library(EMA SHARED)
target_sources(
    EMA
    # core
    PRIVATE EMA/core/device.c
    PRIVATE EMA/core/device.h
    PUBLIC  EMA/core/device.user.h
    PRIVATE EMA/core/overflow.c
    PRIVATE EMA/core/overflow.h
    PRIVATE EMA/core/plugin.h
    PUBLIC  EMA/core/plugin.user.h
    PRIVATE EMA/core/registry.c
    PRIVATE EMA/core/registry.h
    PRIVATE EMA/core/utils.c
    PRIVATE EMA/core/utils.h
    # plugins
    PRIVATE EMA/plugins/plugin_rapl.c
    PRIVATE EMA/plugins/plugin_rapl.h
    # region
    PRIVATE EMA/region/filter.c
    PRIVATE EMA/region/filter.h
    PUBLIC  EMA/region/filter.user.h
    PRIVATE EMA/region/output.c
    PRIVATE EMA/region/output.h
    PUBLIC  EMA/region/output.user.h
    PRIVATE EMA/region/region_store.c
    PRIVATE EMA/region/region_store.h
    PRIVATE EMA/region/region.c
    PRIVATE EMA/region/region.h
    PUBLIC  EMA/region/region.user.h
    # utils
    PRIVATE EMA/utils/error.h
    PRIVATE EMA/utils/time.c
    PRIVATE EMA/utils/time.h
    PUBLIC  EMA/utils/time.user.h
    #
    PRIVATE EMA/user.c
    PUBLIC  EMA/user.h
)
target_include_directories(EMA PRIVATE .)
target_compile_definitions(EMA PRIVATE _GNU_SOURCE)
target_link_libraries(EMA PRIVATE hashmap Threads::Threads)

if(NVML_FOUND)
    # NVML plugin
    target_sources(
        EMA
        PRIVATE EMA/plugins/plugin_nvml.c
        PRIVATE EMA/plugins/plugin_nvml.h
    )
    target_compile_definitions(EMA PRIVATE EMA_HAVE_NVML)
    target_link_libraries(EMA PRIVATE NVML::NVML)
endif()

if( Mosquitto_FOUND )
    # MQTT plugin
    target_sources(
        EMA
        PRIVATE EMA/plugins/plugin_mqtt.c
        PRIVATE EMA/plugins/plugin_mqtt.h
        PUBLIC EMA/plugins/plugin_mqtt.user.h
    )
    target_compile_definitions(EMA PRIVATE EMA_HAVE_MQTT)
    target_include_directories(EMA PRIVATE ${MOSQUITTO_INCLUDE_DIR})
    target_link_libraries(EMA PRIVATE ${MOSQUITTO_LIBRARY})
endif()

# tests
add_subdirectory(tests)

# install
# main public header
install(FILES "EMA.h" DESTINATION "include")

# other public headers - keep directory structure
get_target_property(EMA_PUBLIC_HEADERS EMA INTERFACE_SOURCES)
foreach(hfile ${EMA_PUBLIC_HEADERS})
    file(RELATIVE_PATH hfile_rel "${PROJECT_SOURCE_DIR}" "${hfile}")
    get_filename_component(hdir "${hfile_rel}" DIRECTORY)
    install(FILES "${hfile}" DESTINATION "include/${hdir}")
endforeach()

install(TARGETS EMA)
