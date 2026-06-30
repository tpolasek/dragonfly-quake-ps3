set(LIB input)

add_library(${LIB} STATIC
    src/in_gamepad.c
    src/in_gamepad.h
    src/in_main.c
    src/in_mouse.h
    src/in_mouse_ps3.c
    src/keys.c
)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_link_libraries(${LIB} PUBLIC client common)
target_link_libraries(${LIB} PRIVATE
    cmd
    console
    host
    memory
    menu
    screen
    sys
    video
)
