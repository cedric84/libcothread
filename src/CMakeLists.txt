#---Add sources to the target---#
target_sources(${COTHREAD_TARGET_NAME}
	PRIVATE
		cothread.c
)

#---Add the subdirectories---#
if((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") AND (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
	add_subdirectory(x86_64-gnu_linux)
elseif((CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64") AND (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD"))
	add_subdirectory(x86_64-freebsd)
elseif((CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64") AND (CMAKE_SYSTEM_NAME STREQUAL "Darwin"))
	add_subdirectory(x86_64-macos)
else()
	message(FATAL_ERROR "configuration is not supported.")
endif()
