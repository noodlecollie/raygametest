add_library(rayguilib STATIC
	src/impl.c
)

target_link_libraries(rayguilib PUBLIC
	raygui
	raylib
)

target_include_directories(rayguilib PRIVATE src)
target_compile_definitions(rayguilib PRIVATE _CRT_SECURE_NO_WARNINGS)
