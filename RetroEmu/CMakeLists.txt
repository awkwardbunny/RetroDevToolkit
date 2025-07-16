set(RETROEMU_SOURCES
	common/cpu.hpp
	common/machine.hpp
	common/ram.hpp
	common/registers.cpp
	common/registers.hpp
	cpu/6502.cpp
	cpu/6502.hpp
	machine/apple_iie.cpp
	machine/apple_iie.hpp
)

add_library(RetroEmu ${RETROEMU_SOURCES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(RetroEmu INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RetroEmu PUBLIC libgoodasm spdlog::spdlog Qt6::Quick gtest_main)

add_executable(RetroEmuTest
	${RETROEMU_SOURCES}
)
target_include_directories(RetroEmuTest INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RetroEmuTest PUBLIC libgoodasm spdlog::spdlog Qt6::Quick gtest_main)
include(GoogleTest)
gtest_discover_tests(RetroEmuTest)