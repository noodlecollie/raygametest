add_executable(Game
	src/gameutil.c
	src/main.c
	src/platformerlevel.c
	src/platformmovement.c
	src/player.c
)

target_link_libraries(Game PRIVATE raylib)

if(MSVC)
	add_compile_options(/W4 /WX)
	add_link_options(/WX)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
	add_link_options(-Wl,--fatal-warnings)
endif()

install(TARGETS Game
	RUNTIME DESTINATION raygametest
)

install(DIRECTORY res DESTINATION raygametest)
