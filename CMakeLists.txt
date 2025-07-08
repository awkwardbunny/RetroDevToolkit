cmake_minimum_required(VERSION 3.19)
project(RetroDevToolkit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Quick)
find_package(spdlog REQUIRED)

qt_standard_project_setup()

add_subdirectory(ext/goodasm)
add_subdirectory(src/cpus)
add_subdirectory(src/machines)
add_subdirectory(src/app)
add_subdirectory(src/retroemu)

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
