set(name character_animation)

file(GLOB src
        *.cpp *.hpp)

add_library(${name} MODULE ${src})
target_link_libraries(${name} api)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
