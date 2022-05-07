include(compile_options)

add_executable(movement_control
	src/main.c
)

target_link_libraries(movement_control PRIVATE
	raylib
	rayguilib
	gamelib
)

apply_pedantic_compile_options(movement_control)

install(TARGETS movement_control
	RUNTIME DESTINATION playground/movement_control
)

install(DIRECTORY res DESTINATION playground/movement_control)
