set(LIB sys)

add_library(${LIB} STATIC src/sys.c)

target_include_directories(${LIB} PRIVATE ${CMAKE_BINARY_DIR} "../")
target_include_directories(${LIB} PUBLIC "./include")
target_link_libraries(${LIB} PUBLIC common host)
target_link_libraries(${LIB} PRIVATE
    client
    cmd
    input
    menu
)
if(NOT CHOCOLATE_QUAKE_PS3)
    target_link_libraries(${LIB} PRIVATE end_screen)
else()
    # end_screen library is excluded on PS3, but sys.c still includes
    # end_screen.h (which provides a no-op inline ES_DisplayScreen on PS3).
    target_include_directories(${LIB} PRIVATE "../end_screen/include")
endif()
