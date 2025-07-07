set(PROJECT tis620)

add_library(tis620 STATIC tis620.cpp)

target_include_directories(tis620 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tis620 PRIVATE FixColour)