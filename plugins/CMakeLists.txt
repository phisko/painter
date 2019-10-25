putils_get_subdirectories(subdirs ${CMAKE_CURRENT_LIST_DIR})

foreach(dir ${subdirs})
    add_subdirectory(${dir})
endforeach()
