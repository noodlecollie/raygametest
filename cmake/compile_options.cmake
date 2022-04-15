function(apply_pedantic_compile_options target_name)
	if(MSVC)
		target_compile_options(${target_name} PRIVATE /W4 /WX)
		target_link_options(${target_name} PRIVATE /WX)
	else()
		target_compile_options(${target_name} PRIVATE -Wall -Wextra -pedantic -Werror)
		target_link_options(${target_name} PRIVATE -Wl,--fatal-warnings)
	endif()
endfunction()