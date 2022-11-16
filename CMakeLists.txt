cmake_minimum_required(VERSION 3.10)
project(painter)

set(CMAKE_CXX_STANDARD 20)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /DNOMINMAX")
endif()

add_compile_definitions(
    KENGINE_COMPONENT_COUNT=256
    KENGINE_MAX_CSM_COUNT=10
	KENGINE_USE_SPINLOCK
)

# These must be set before processing kengine, as conan DLLs need to be copied here
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/kengine/putils/reflection/meta/cmake_helpers/CMakeModules)
include(putils_set_output_directories)
putils_set_output_directories(bin bin/plugins lib)

## Kengine

set(KENGINE_ALL_SYSTEMS ON)
set(KENGINE_TYPE_REGISTRATION ON)

option(TRACY_STATIC ON)
set(TRACY_STATIC OFF) # Use a shared instance of tracy for all plugins
add_subdirectory(kengine)

## API
add_library(api INTERFACE)
target_include_directories(api INTERFACE common)

## Executable

set(exe_name painter)

file(GLOB exeFiles src/*.cpp src/*.hpp
	src/types/*.cpp src/types/*.hpp
        src/components/*.cpp src/components/*.hpp
        src/gameobjects/*.cpp src/gameobjects/*.hpp
        src/systems/*.cpp src/systems/*.hpp)

add_executable(${exe_name} ${exeFiles} ${commonFiles})
target_include_directories(${exe_name} PRIVATE src)
target_link_libraries(${exe_name} PRIVATE kengine api)

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