#add_subdirectory(6502)
add_library(cpus
	6502.cpp
	6502.hpp
)

target_include_directories(cpus INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cpus PUBLIC libgoodasm spdlog::spdlog Qt6::Quick)
#target_link_libraries(cpu PRIVATE Qt6::Quick)
