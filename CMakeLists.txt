#---Check the cmake version---#
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

#---Set the name & version for the project---#
project(cothread
	VERSION
		1.0.0.0
	LANGUAGES
		C ASM
)

#---Provide user options---#
option(BUILD_SHARED_LIBS			"build shared libraries"			TRUE)

#---Set the default build type if none already specified---#
# Note: only meaningful to single-configuration generators (such as Makefile Generators and Ninja).
# see https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html
# see https://cmake.org/cmake/help/latest/prop_gbl/GENERATOR_IS_MULTI_CONFIG.html
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE	"Debug")
endif()

#---Initialize global variables---#
# install directories
# see https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html#module:GNUInstallDirs
# see https://cmake.org/cmake/help/latest/command/install.html#targets
set(COTHREAD_INSTALL_DESTINATION_ARCHIVE			lib)
set(COTHREAD_INSTALL_DESTINATION_LIBRARY			lib)
set(COTHREAD_INSTALL_DESTINATION_RUNTIME			bin)
set(COTHREAD_INSTALL_DESTINATION_PUBLIC_HEADER		include/${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

#---Set the default RPATH configuration---#
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH	TRUE)
set(CMAKE_MACOSX_RPATH					TRUE)
if(FALSE
	OR (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	OR (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
	)
	set(CMAKE_INSTALL_RPATH				\$ORIGIN/../${COTHREAD_INSTALL_DESTINATION_LIBRARY})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CMAKE_INSTALL_RPATH				@executable_path/../${COTHREAD_INSTALL_DESTINATION_LIBRARY})
endif()

#---Specify the standards---//
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

#---Add the generic compilation options---#
if(FALSE
	OR (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	OR (CMAKE_C_COMPILER_ID STREQUAL "Clang")
	OR (CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
	)
	add_compile_options(-Wall -Werror)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(/W3 /WX)
else()
	message(FATAL_ERROR "generic compilation options are missing for the \"${CMAKE_C_COMPILER_ID}\" compiler")
endif()

#---Add the library----#
set(COTHREAD_TARGET_NAME	${PROJECT_NAME})
add_library(${COTHREAD_TARGET_NAME})
set_target_properties(${COTHREAD_TARGET_NAME} PROPERTIES
	VERSION				${PROJECT_VERSION}
	DEFINE_SYMBOL		COTHREAD_LINK=COTHREAD_LINK_EXPORT
)

#---Add include directories to the target---#
target_include_directories(${COTHREAD_TARGET_NAME}
	PUBLIC
		include
)

#---Add the subdirectories---#
add_subdirectory(src)

#---Set the list of public headers to install---#
set_property(TARGET ${COTHREAD_TARGET_NAME}
	PROPERTY PUBLIC_HEADER
		include/cothread/config.h
		include/cothread/cothread.h
)

#---Specify the install rules---#
install(TARGETS ${COTHREAD_TARGET_NAME}
	ARCHIVE
		DESTINATION		${COTHREAD_INSTALL_DESTINATION_ARCHIVE}
	LIBRARY
		DESTINATION		${COTHREAD_INSTALL_DESTINATION_LIBRARY}
	RUNTIME
		DESTINATION		${COTHREAD_INSTALL_DESTINATION_RUNTIME}
	PUBLIC_HEADER
		DESTINATION		${COTHREAD_INSTALL_DESTINATION_PUBLIC_HEADER}/${PROJECT_NAME}
)
