include(compile_options)

add_executable(game
	src/main.c
)

target_link_libraries(game PRIVATE gamelib raylib)
apply_pedantic_compile_options(game)

install(TARGETS game
	RUNTIME DESTINATION game
)

# For some reason CMake absolutely sucks at excluding files (it basically excludes everything),
# so instead we include files that don't end with ~
install(DIRECTORY res DESTINATION game
	FILES_MATCHING REGEX "^.*[^~]$"
)
