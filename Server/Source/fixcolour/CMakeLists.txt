set(PROJECT FixColour)

add_library(FixColour STATIC fixce.cpp)

target_include_directories(FixColour PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})