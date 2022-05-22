include(compile_options)

add_executable(level_collision
	src/main.c
)

target_link_libraries(level_collision PRIVATE
	raylib
	rayguilib
	gamelib
)

apply_pedantic_compile_options(level_collision)

install(TARGETS level_collision
	RUNTIME DESTINATION playground/level_collision
)

# For some reason CMake absolutely sucks at excluding files (it basically excludes everything),
# so instead we include files that don't end with ~
install(DIRECTORY res DESTINATION playground/level_collision
	FILES_MATCHING REGEX "^.*[^~]$"
)
