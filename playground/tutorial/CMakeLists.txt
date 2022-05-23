include(compile_options)

add_executable(tutorial
	src/main.c
)

target_link_libraries(tutorial PRIVATE
	raylib
	rayguilib
	gamelib
)

apply_pedantic_compile_options(tutorial)

install(TARGETS tutorial
	RUNTIME DESTINATION playground/tutorial
)

# For some reason CMake absolutely sucks at excluding files (it basically excludes everything),
# so instead we include files that don't end with ~
install(DIRECTORY res DESTINATION playground/tutorial
	FILES_MATCHING REGEX "^.*[^~]$"
)
