include(compile_options)

add_executable(level_collision
	src/main.c
)

target_link_libraries(level_collision PRIVATE raylib)
apply_pedantic_compile_options(level_collision)

install(TARGETS level_collision
	RUNTIME DESTINATION playground/level_collision
)

install(DIRECTORY res DESTINATION playground/level_collision)
