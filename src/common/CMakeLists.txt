set(LIB common)

add_library(${LIB} STATIC
    src/com_argv.c
    src/com_byte.c
    src/com_ext.c
    src/com_fs.c
    src/com_init.c
    src/com_link.c
    src/com_msg.c
    src/com_sizebuf.c
    src/com_stdio.c
    src/com_stdlib.c
    src/com_string.c
    src/com_token.c
    src/com_va.c
)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_link_libraries(${LIB} PRIVATE
    cmd
    console
    crc
    memory
    net
    renderer
    sys
)
