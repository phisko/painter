putils_get_subdirectories(${CMAKE_CURRENT_LIST_DIR} subdirs)

foreach(subdir ${subdirs})
    set(name ${subdir})

    file(GLOB src ${subdir}/*.cpp)
    add_library(${name} MODULE ${src})

    file(GLOB headers ${subdir}/*.hpp)
    list(LENGTH headers headers_count)
    if(${headers_count} GREATER 0)
        putils_generate_reflection_headers(
                TARGET ${name}
                SOURCES ${headers}
        )
        plugin_register_headers(${headers})
    endif()

    target_link_libraries(${name} api)
endforeach()
