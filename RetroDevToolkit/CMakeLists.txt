qt_add_executable(RetroDevToolkit
    WIN32 MACOSX_BUNDLE
    main.cpp
)

target_include_directories(RetroDevToolkit INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(RetroDevToolkit
    PRIVATE
        imgui
)

target_link_libraries(RetroDevToolkit
    PUBLIC
    RetroEmu
    spdlog::spdlog
)
