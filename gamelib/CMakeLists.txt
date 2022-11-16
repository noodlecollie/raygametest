include(compile_options)

add_library(gamelib STATIC
	include/gamelib/debugging/debugging.h
	include/gamelib/debugging/debugrendercustom.h
	include/gamelib/debugging/leakcheck_includefirst.h
	include/gamelib/entity/cameracomponent.h
	include/gamelib/entity/componenttypes.h
	include/gamelib/entity/entity.h
	include/gamelib/entity/logiccomponent.h
	include/gamelib/entity/physicscomponent.h
	include/gamelib/entity/spritecomponent.h
	include/gamelib/entity/terraincomponent.h
	include/gamelib/external/raylibheaders.h
	include/gamelib/logic/playermovementlogic.h
	include/gamelib/collisionclasses.h
	include/gamelib/drawingdepth.h
	include/gamelib/gametypes.h
	include/gamelib/gameutil.h
	include/gamelib/memoryallocator.h
	include/gamelib/parametric.h
	include/gamelib/physics.h
	include/gamelib/stringutil.h
	include/gamelib/trace.h
	include/gamelib/world.h

	src/descriptor/descriptorutil.h
	src/descriptor/descriptorutil.c
	src/descriptor/entitygroupdescriptor.h
	src/descriptor/entitygroupdescriptor.c
	src/descriptor/spritesheetdescriptor.h
	src/descriptor/spritesheetdescriptor.c
	src/descriptor/terraindescriptor.h
	src/descriptor/terraindescriptor.c
	src/entity/cameracomponentimpl.h
	src/entity/cameracomponentimpl.c
	src/entity/entitygroup.h
	src/entity/entitygroup.c
	src/entity/entityimpl.h
	src/entity/entityimpl.c
	src/entity/logiccomponentimpl.h
	src/entity/logiccomponentimpl.c
	src/entity/physicscomponentimpl.h
	src/entity/physicscomponentimpl.c
	src/entity/spritecomponentimpl.h
	src/entity/spritecomponentimpl.c
	src/entity/terraincomponentimpl.h
	src/entity/terraincomponentimpl.c
	src/external/uthash/uthash.h
	src/external/cJSON_util.h
	src/external/cJSON_util.c
	src/external/cJSON_wrapper.h
	src/external/cJSON_wrapper.c
	src/external/uthash_wrapper.h
	src/logic/playermovementlogic.c
	src/presets/presetmeshes.h
	src/presets/presetmeshes.c
	src/presets/presetnames.h
	src/presets/presetshaders.h
	src/presets/presetshaders.c
	src/presets/presettextures.h
	src/presets/presettextures.c
	src/rendering/builtinshaders.h
	src/rendering/builtinshaders.c
	src/rendering/debugrendercustom_impl.h
	src/rendering/debugrendercustom_impl.c
	src/rendering/debugrendering.h
	src/rendering/debugrendering.c
	src/rendering/renderutils.h
	src/rendering/renderutils.c
	src/rendering/spriterenderer.h
	src/rendering/spriterenderer.c
	src/rendering/terrainrenderer.h
	src/rendering/terrainrenderer.c
	src/resourcepool/resourcepool_internal.h
	src/resourcepool/resourcepool_internal.c
	src/resourcepool/resourcepool_mesh.c
	src/resourcepool/resourcepool_shader.c
	src/resourcepool/resourcepool_spritesheet.c
	src/resourcepool/resourcepool_terrain.c
	src/resourcepool/resourcepool_texture.c
	src/collisionclasses.c
	src/debugging.c
	src/drawingdepth.c
	src/gameutil.c
	src/listmacros.h
	src/memoryallocator.c
	src/parametric.c
	src/physics.c
	src/stringutil.c
	src/threading.h
	src/threading.c
	src/trace.c
	src/world.c
	src/worldimpl.h
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

if(WIN32)
	target_link_libraries(gamelib PRIVATE
		pthreads-win32
	)
endif()

apply_pedantic_compile_options(gamelib)
