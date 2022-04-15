add_library(gamelib STATIC
	src/gameutil.c
	src/platformerlevel.c
	src/platformmovement.c
	src/player.c
	src/trace.c
)

target_include_directories(gamelib PUBLIC include)
target_include_directories(gamelib PRIVATE src)
target_link_libraries(gamelib PUBLIC raylib)
