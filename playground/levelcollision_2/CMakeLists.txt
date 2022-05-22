include(compile_options)

add_executable(level_collision_2
	src/main.c
)

target_link_libraries(level_collision_2 PRIVATE
	raylib
	rayguilib
	gamelib
)

apply_pedantic_compile_options(level_collision_2)

install(TARGETS level_collision_2
	RUNTIME DESTINATION playground/level_collision_2
)

# For some reason CMake absolutely sucks at excluding files (it basically excludes everything),
# so instead we include files that don't end with ~
install(DIRECTORY res DESTINATION playground/level_collision_2
	FILES_MATCHING REGEX "^.*[^~]$"
)
