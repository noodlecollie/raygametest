include(compile_options)

add_library(gamelib STATIC
	src/entity/cameracomponentimpl.c
	src/entity/entityimpl.c
	src/entity/logiccomponentimpl.c
	src/entity/physicscomponentimpl.c
	src/entity/terraincomponentimpl.c
	src/entity/spritecomponentimpl.c
	src/logic/playerlogic.c
	src/gameutil.c
	src/imagepool.c
	src/memoryallocator.c
	src/physics.c
	src/trace.c
	src/world.c
)

target_include_directories(gamelib PUBLIC include)
target_include_directories(gamelib PRIVATE src)
target_link_libraries(gamelib PUBLIC raylib)
apply_pedantic_compile_options(gamelib)
