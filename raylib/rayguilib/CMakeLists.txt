add_library(rayguilib STATIC
	src/impl.c
)

target_link_libraries(rayguilib PUBLIC
	raygui
	raylib
)

target_include_directories(rayguilib PRIVATE src)
