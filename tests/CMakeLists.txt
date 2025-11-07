# tests
add_executable(basic basic.c)
target_include_directories(basic PRIVATE ..)
target_link_libraries(basic PRIVATE EMA)

add_executable(hl_region hl_region.c)
target_include_directories(hl_region PRIVATE ..)
target_link_libraries(hl_region PRIVATE EMA)

add_executable(ll_region ll_region.c)
target_include_directories(ll_region PRIVATE ..)
target_link_libraries(ll_region PRIVATE EMA)

if( Mosquitto_FOUND )
    add_executable(test_mqtt mqtt_basic.c)
    target_include_directories(test_mqtt PRIVATE ..)
    target_link_libraries(test_mqtt PRIVATE EMA)
endif()
