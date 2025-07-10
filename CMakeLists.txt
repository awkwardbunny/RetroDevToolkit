cmake_minimum_required(VERSION 3.19)
project(RetroDevToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(spdlog REQUIRED)
find_package(imgui REQUIRED)
find_package(goodasm)

qt_standard_project_setup(REQUIRES 6.5)

add_subdirectory(RetroDevToolkit)
add_subdirectory(RetroEmu)

# include(GNUInstallDirs)

# install(TARGETS RetroDevToolkit
#     BUNDLE  DESTINATION .
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

# qt_generate_deploy_app_script(
#     TARGET RetroDevToolkit
#     OUTPUT_SCRIPT deploy_script
#     NO_UNSUPPORTED_PLATFORM_ERROR
# )
# install(SCRIPT ${deploy_script})
