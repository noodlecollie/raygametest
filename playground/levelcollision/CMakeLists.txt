include(compile_options)
include(install_helpers)

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

install_dir_with_filters(res playground/level_collision)
