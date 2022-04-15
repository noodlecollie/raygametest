add_executable(level_collision
	src/main.c
)

target_link_libraries(level_collision PRIVATE raylib)

if(MSVC)
	add_compile_options(/W4 /WX)
	add_link_options(/WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
	add_link_options(-Wl,--fatal-warnings)
endif()

install(TARGETS level_collision
	RUNTIME DESTINATION playground/level_collision
)

install(DIRECTORY res DESTINATION playground/level_collision)
