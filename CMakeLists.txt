list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.21.0)
project("Chocolate Quake" VERSION 2.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

# C89's implicit-function-declaration fallback has caused real ABI-truncation
# bugs on PS3 (PPC64: int return read from r3 while the real function returns
# double in f1, silently corrupting parsed values). Force this class of bug
# to be a compile error.
add_compile_options(
    -Werror=implicit-function-declaration
    -Werror=implicit-int
)

string(TIMESTAMP CURRENT_YEAR "%Y")
set(PACKAGE_NAME ${PROJECT_NAME})
set(PACKAGE_TARNAME "chocolate-quake")
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_COPYRIGHT "Copyright (C) 1996-${CURRENT_YEAR}")
set(PACKAGE_LICENSE "GNU General Public License, version 3")
set(APPSTREAM_APP_ID "io.github.henrique194.chocolate-quake")

# Find math library
find_library(MATH m)
if(MATH)
    link_libraries(${MATH})
endif()

if(CHOCOLATE_QUAKE_PS3)
	# PS3 (PPU/PSL1GHT) path: native PSL1GHT APIs, no SDL2.
	#
	# Expose CHOCOLATE_QUAKE_PS3 to source so platform branches (e.g.
	# Sys_GetDefaultBaseDir in src/sys) can pick the PS3 layout.
	add_compile_definitions(CHOCOLATE_QUAKE_PS3=1)

	# PSL1GHT SDK headers (sysutil/sysutil.h, rsx/*.h, audio/audio.h,
	# io/pad.h, etc.) live in ppu/include; codec headers (vorbis/vorbisfile.h,
	# mad.h, FLAC/stream_decoder.h) live in the portlibs include dir. Neither
	# is on the compiler's default search path.
	include_directories("${PS3DEV}/portlibs/ppu/include" "${PS3DEV}/ppu/include")

	# PSL1GHT runtime libraries (were transitively pulled in by SDL2 before
	# its removal). Applied globally so every target links them. -lm is
	# required by the codec libs (vorbis/FLAC); find_library(MATH m) at the
	# top of this file can't see it because CMAKE_FIND_ROOT_PATH points at
	# the install prefix, not the powerpc64-ps3-elf sysroot where libm.a lives.
	link_libraries(-lgcm_sys -lrsx -lsysutil -lio -laudio -lrt -llv2 -lm)

	find_library(VORBIS_LIBRARY vorbis REQUIRED)
	find_library(VORBISFILE_LIBRARY vorbisfile REQUIRED)
	find_library(MAD_LIBRARY mad REQUIRED)
	find_library(FLAC_LIBRARY FLAC REQUIRED)
	find_library(OGG_LIBRARY ogg REQUIRED)
else()
	find_package(SDL2 CONFIG REQUIRED)
	find_package(SDL2_net CONFIG REQUIRED)
	if(UNIX AND NOT APPLE)
		find_library(VORBIS_LIBRARY vorbis REQUIRED)
		find_library(VORBISFILE_LIBRARY vorbisfile REQUIRED)
		find_library(MAD_LIBRARY mad REQUIRED)
		find_library(FLAC_LIBRARY FLAC REQUIRED)
		find_library(OGG_LIBRARY ogg REQUIRED)
	else()
		find_package(Vorbis CONFIG REQUIRED)
		find_package(mad CONFIG REQUIRED)
		find_package(FLAC CONFIG REQUIRED)
	endif()
endif()

configure_file(config.h.in config.h)

add_subdirectory("src")

if (UNIX AND NOT APPLE)
	install(FILES ${CMAKE_BINARY_DIR}/src/chocolate-quake
		DESTINATION bin
		PERMISSIONS WORLD_READ WORLD_EXECUTE GROUP_READ GROUP_EXECUTE OWNER_READ OWNER_WRITE OWNER_EXECUTE)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/${APPSTREAM_APP_ID}.svg
		DESTINATION share/icons/hicolor/scalable/apps
		PERMISSIONS WORLD_READ GROUP_READ OWNER_READ OWNER_WRITE)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/${APPSTREAM_APP_ID}.desktop
		DESTINATION share/applications
		PERMISSIONS WORLD_READ GROUP_READ OWNER_READ OWNER_WRITE)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/linux/${APPSTREAM_APP_ID}.metainfo.xml
		DESTINATION share/metainfo
		PERMISSIONS WORLD_READ GROUP_READ OWNER_READ OWNER_WRITE)
endif()
