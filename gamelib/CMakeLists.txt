include(compile_options)

add_library(gamelib STATIC
	src/descriptor/descriptorutil.c
	src/descriptor/spritesheetdescriptor.c
	src/entity/cameracomponentimpl.c
	src/entity/entityimpl.c
	src/entity/logiccomponentimpl.c
	src/entity/physicscomponentimpl.c
	src/entity/terraincomponentimpl.c
	src/entity/spritecomponentimpl.c
	src/external/cJSON_util.c
	src/external/cJSON_wrapper.c
	src/logic/playerlogic.c
	src/rendering/spriterenderer.c
	src/gameutil.c
	src/memoryallocator.c
	src/physics.c
	src/presetmeshes.c
	src/resourcepool.c
	src/trace.c
	src/world.c
)

target_include_directories(gamelib PUBLIC
	include
)

target_include_directories(gamelib PRIVATE
	src
)

target_link_libraries(gamelib PUBLIC
	raylib
)

target_link_libraries(gamelib PRIVATE
	cJSON
)

if(WIN32 AND WIN32_DEBUG_MEMORY_LEAKS)
	target_compile_definitions(gamelib PUBLIC -DWIN32_DEBUG_MEMORY_LEAKS)
endif()

apply_pedantic_compile_options(gamelib)
