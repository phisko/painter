set(name entityHighlight)

file(GLOB src
        *.cpp *.hpp)

add_library(${name}
        SHARED MODULE
        ${src}
        )
target_link_libraries(${name} kengine api)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
