include(compile_options)

add_executable(parametric_demo
	src/main.c
)

target_link_libraries(parametric_demo PRIVATE
	raylib
	gamelib
)

apply_pedantic_compile_options(parametric_demo)

install(TARGETS parametric_demo
	RUNTIME DESTINATION playground/parametric_demo
)
