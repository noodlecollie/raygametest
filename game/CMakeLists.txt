add_executable(Game
	src/Main.c
)

target_link_libraries(Game PRIVATE raylib)

install(TARGETS Game
	RUNTIME DESTINATION raygametest
)

install(DIRECTORY res DESTINATION raygametest)
