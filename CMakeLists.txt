set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries since to share global state between plugins" FORCE)

cmake_minimum_required(VERSION 3.15)
project(painter)

add_compile_definitions(KENGINE_IMGUI_ASSERT_HANDLER)

set(CMAKE_CXX_STANDARD 20)

# These must be set before processing kengine, as conan DLLs need to be copied here
include(kengine/putils/reflection/meta/cmake_helpers/putils_set_output_directories.cmake)
putils_set_output_directories(bin bin/plugins lib)

# Kengine
set(PUTILS_PLUGIN_MANAGER ON CACHE BOOL "Build plugin manager to load plugins" FORCE)
set(KENGINE_ALL_LIBRARIES ON CACHE BOOL "Enable all libraries" FORCE)
set(KENGINE_TYPE_REGISTRATION ON CACHE BOOL "Enable type registration" FORCE)
set(TRACY_STATIC OFF CACHE BOOL "Use a shared instance of Tracy for all plugins" FORCE)
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

option(PAINTER_GENERATE_REFLECTION "Generate reflection headers for painter data types")
if(PAINTER_GENERATE_REFLECTION)
	putils_generate_reflection_headers(
		TARGET ${exe_name}
		SOURCES ${api_files}
	)
endif()

putils_copy_dlls(${exe_name})

list(APPEND headers_to_register ${api_files})
macro(plugin_register_headers)
	list(APPEND headers_to_register ${ARGN})
	set(headers_to_register ${headers_to_register} PARENT_SCOPE)
endmacro()
add_subdirectory(plugins)

kengine_generate_type_registration(
		TARGET painter
		INCLUDE_DIR painter/types
		SOURCES ${headers_to_register}
		REGISTRATIONS_JSON ${CMAKE_CURRENT_LIST_DIR}/kengine/scripts/registrations.json
		NAMESPACE painter::types
)
target_link_libraries(painter_type_registration api)
target_link_libraries(painter PRIVATE painter_type_registration)

# Symlinks
set(links_to_create
	resources
	scripts
	config.json
)
set(output_dir $<TARGET_FILE_DIR:${exe_name}>)
foreach(link ${links_to_create})
	set(source ${CMAKE_CURRENT_LIST_DIR}/${link})
	set(dest ${output_dir}/${link})
	add_custom_command(
			TARGET ${exe_name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${dest}
	)
endforeach()