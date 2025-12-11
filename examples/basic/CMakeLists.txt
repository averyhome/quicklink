cmake_minimum_required(VERSION 3.13)

project(quicklink_basic_example)

set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

# Link against quicklink component
target_link_libraries(${COMPONENT_LIB} PUBLIC quicklink)
