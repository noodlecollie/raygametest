function(install_dir_with_filters input output)
if(WIN32)
	# For some reason, trying to exclude *.*~ files on Windows
	# doesn't work properly (it excludes all the files), so instead
	# we explicitly *include* files which *don't* end in ~
	install(DIRECTORY ${input} DESTINATION ${output}
	FILES_MATCHING REGEX "^.*[^~]$"
	)
else()
	# The nicer way:
	install(DIRECTORY ${input} DESTINATION ${output}
		FILES_MATCHING PATTERN "*.*~" EXCLUDE
	)
endif()
endfunction()
