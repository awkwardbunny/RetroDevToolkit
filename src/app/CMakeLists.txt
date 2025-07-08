qt_add_executable(RetroDevToolkit
    WIN32 MACOSX_BUNDLE
    main.cpp
    windowdisplay.cpp
    windowdisplay.h
    windowdisplay.ui
    registermodel.h registermodel.cpp
    stackmodel.h stackmodel.cpp
    codemodel.h codemodel.cpp




)

target_link_libraries(RetroDevToolkit
    PRIVATE
        Qt::Core
        Qt::Widgets
)

target_link_libraries(RetroDevToolkit
    PUBLIC
    machines
    spdlog::spdlog
)
