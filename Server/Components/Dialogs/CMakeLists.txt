get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_server_component(${ProjectId})
target_link_libraries(Dialogs PRIVATE tis620)