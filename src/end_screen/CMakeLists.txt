if(CHOCOLATE_QUAKE_PS3)
    # End screen disabled on PS3 (game exits to XMB on quit).
    return()
endif()

set(LIB end_screen)

add_library(${LIB} STATIC
    src/fonts/large.h
    src/fonts/normal.h
    src/fonts/small.h
    src/es_buffer.c
    src/es_buffer.h
    src/es_font.c
    src/es_font.h
    src/es_main.c
    src/es_palette.c
    src/es_palette.h
    src/es_time.c
    src/es_time.h
    src/es_window.c
    src/es_window.h
)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_link_libraries(${LIB} PUBLIC common)
target_link_libraries(${LIB} PRIVATE ${SDL2_LIBRARIES} console sys)
