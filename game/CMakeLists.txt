add_executable(game
	src/main.c
)

target_link_libraries(game PRIVATE gamelib raylib)

if(MSVC)
	add_compile_options(/W4 /WX)
	add_link_options(/WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
	add_link_options(-Wl,--fatal-warnings)
endif()

install(TARGETS game
	RUNTIME DESTINATION game
)

install(DIRECTORY res DESTINATION game)
