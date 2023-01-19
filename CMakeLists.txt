cmake_minimum_required(VERSION 3.10)
project(painter)

add_compile_definitions(KENGINE_IMGUI_ASSERT_HANDLER)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /DNOMINMAX")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
endif()

# We're using plugins, so we need shared libraries to share global state
set(BUILD_SHARED_LIBS ON)

# These must be set before processing kengine, as conan DLLs need to be copied here
include(${CMAKE_CURRENT_LIST_DIR}/kengine/putils/reflection/meta/cmake_helpers/putils_set_output_directories.cmake)
putils_set_output_directories(bin bin/plugins lib)

# Kengine
set(KENGINE_ALL_SYSTEMS ON)
set(KENGINE_TYPE_REGISTRATION ON)
set(TRACY_STATIC OFF) # Use a shared instance of tracy for all plugins
add_subdirectory(kengine)

# API
file(GLOB_RECURSE api_files common/*.hpp)
add_library(api INTERFACE ${api_files})
target_link_libraries(api INTERFACE kengine)
target_include_directories(api INTERFACE common)

# Executable
set(exe_name painter)
add_executable(${exe_name} src/main.cpp)
target_link_libraries(${exe_name} PRIVATE api)

kengine_generate_type_registration(
	TARGET painter
	INCLUDE_DIR painter/types
	SOURCES ${api_files}
	REGISTRATIONS_JSON ${CMAKE_CURRENT_LIST_DIR}/kengine/scripts/registrations.json
	NAMESPACE painter::types
)
target_link_libraries(painter_type_registration api)
target_link_libraries(painter PRIVATE painter_type_registration)

option(PAINTER_GENERATE_REFLECTION OFF)
if(PAINTER_GENERATE_REFLECTION)
	putils_generate_reflection_headers(
		TARGET ${exe_name}
		SOURCES ${api_files}
	)
endif()

putils_copy_dlls(${exe_name})

add_subdirectory(plugins)

## Resources

add_custom_command(TARGET ${exe_name} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_LIST_DIR}/resources
		$<TARGET_FILE_DIR:painter>/resources)
add_custom_command(TARGET ${exe_name} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_LIST_DIR}/scripts
		$<TARGET_FILE_DIR:painter>/scripts)
add_custom_command(TARGET ${exe_name} POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_LIST_DIR}/adjust.ini
		$<TARGET_FILE_DIR:painter>/adjust.ini)