qt_add_executable(RetroDevToolkit
    WIN32 MACOSX_BUNDLE
    main.cpp

    windowdisplay.cpp
    windowdisplay.h
    windowdisplay.ui

    registermodel.cpp
    registermodel.h
    stackmodel.cpp
    stackmodel.h
    codemodel.cpp
    codemodel.h
)

target_include_directories(RetroDevToolkit INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(RetroDevToolkit
    PRIVATE
        Qt::Core
        Qt::Widgets
)

target_link_libraries(RetroDevToolkit
    PUBLIC
    RetroEmu
    spdlog::spdlog
)
