 add_executable(server server.c)
 add_executable(client client.c)

 target_link_libraries(server PRIVATE toptions)
 target_link_libraries(client PRIVATE toptions)