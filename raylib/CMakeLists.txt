if(MSVC)
	add_compile_options(
		/wd4244
		/wd4305
		/wd4267
		/wd4018
	)
endif()

add_subdirectory(raylib EXCLUDE_FROM_ALL)
add_subdirectory(raygui/projects/CMake EXCLUDE_FROM_ALL)
add_subdirectory(rayguilib)
