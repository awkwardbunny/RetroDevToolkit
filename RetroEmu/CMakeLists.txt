add_library(RetroEmu
	common/cpu.cpp
	common/cpu.hpp
	common/machine.cpp
	common/machine.hpp
	cpu/6502.cpp
	cpu/6502.hpp
	machine/apple_iie.cpp
	machine/apple_iie.hpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(RetroEmu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RetroEmu PUBLIC libgoodasm spdlog::spdlog Qt6::Quick)
