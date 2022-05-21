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

install(DIRECTORY res DESTINATION playground/level_collision_2
	FILES_MATCHING PATTERN "*.*~" EXCLUDE
)
