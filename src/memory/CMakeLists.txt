set(LIB memory)

add_library(${LIB} STATIC src/zone.c)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_link_libraries(${LIB} PUBLIC common)
target_link_libraries(${LIB} PRIVATE
    cmd
    console
    sys
)
