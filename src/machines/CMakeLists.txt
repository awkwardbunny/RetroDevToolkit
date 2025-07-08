#add_subdirectory(generic)
#add_subdirectory(apple_iie)

add_library(machines
	apple_iie.cpp
	apple_iie.hpp
)

target_include_directories(machines INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(machines PUBLIC cpus spdlog::spdlog)
