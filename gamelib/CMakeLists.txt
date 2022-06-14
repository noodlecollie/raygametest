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
	src/logic/playermovementlogic.c
	src/presets/presetmeshes.c
	src/presets/presetshaders.c
	src/presets/presettextures.c
	src/rendering/builtinshaders.c
	src/rendering/renderutils.c
	src/rendering/spriterenderer.c
	src/rendering/terrainrenderer.c
	src/resourcepool/resourcepool_internal.c
	src/resourcepool/resourcepool_mesh.c
	src/resourcepool/resourcepool_shader.c
	src/resourcepool/resourcepool_spritesheet.c
	src/resourcepool/resourcepool_texture.c
	src/gameutil.c
	src/memoryallocator.c
	src/physics.c
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
