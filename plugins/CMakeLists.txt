putils_get_subdirectories(${CMAKE_CURRENT_LIST_DIR} subdirs)

foreach(subdir ${subdirs})
    add_subdirectory(${subdir})
endforeach()
