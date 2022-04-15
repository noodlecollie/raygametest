include(compile_options)

add_executable(game
	src/main.c
)

target_link_libraries(game PRIVATE gamelib raylib)
apply_pedantic_compile_options(game)

install(TARGETS game
	RUNTIME DESTINATION game
)

install(DIRECTORY res DESTINATION game)
