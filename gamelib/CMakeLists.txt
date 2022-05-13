include(compile_options)

add_library(gamelib STATIC
	src/entity/entity.c
	src/entity/physicscomponent.c
	src/gameutil.c
	src/physics.c
	src/platformmovement.c
	src/player.c
	src/terrain.c
	src/trace.c
)

target_include_directories(gamelib PUBLIC include)
target_include_directories(gamelib PRIVATE src)
target_link_libraries(gamelib PUBLIC raylib)
apply_pedantic_compile_options(gamelib)
