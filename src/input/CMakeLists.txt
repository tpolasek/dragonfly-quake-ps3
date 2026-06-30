set(LIB input)

add_library(${LIB} STATIC
    src/in_gamepad.c
    src/in_gamepad.h
    src/in_main.c
    src/in_mouse.h
    src/keys.c
)

if(CHOCOLATE_QUAKE_PS3)
    # Pad-only on PS3: no keyboard, no SDL mouse. PSL1GHT pad is polled
    # directly in in_gamepad.c; in_mouse_ps3.c is a no-op stub.
    target_sources(${LIB} PRIVATE src/in_mouse_ps3.c)
else()
    target_sources(${LIB} PRIVATE
        src/in_keyboard.c
        src/in_keyboard.h
        src/in_mouse.c)
endif()

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
