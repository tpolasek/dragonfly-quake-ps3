set(LIB host)

add_library(${LIB} STATIC
    src/host.c
    src/host_cmd.c
)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_link_libraries(${LIB} PUBLIC common console)
target_link_libraries(${LIB} PRIVATE
    camera
    client
    cmd
    input
    menu
    model
    progs
    renderer
    screen
    server
    sound
    sys
    status_bar
    wad
)
