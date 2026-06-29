set(LIB camera)

add_library(${LIB} STATIC
    src/chase.c
    src/view.c
)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_include_directories(${LIB} PRIVATE ../sys/include)
target_link_libraries(${LIB} PUBLIC common console mathlib)
target_link_libraries(${LIB} PRIVATE
    client
    cmd
    host
    model
    renderer
    screen
    server
)
