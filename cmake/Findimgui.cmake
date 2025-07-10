include(FetchContent)
FetchContent_Declare(imgui_ext GIT_REPOSITORY https://github.com/ocornut/imgui.git)
FetchContent_MakeAvailable(imgui_ext)

add_library(imgui
    ${imgui_ext_SOURCE_DIR}/imconfig.h
    ${imgui_ext_SOURCE_DIR}/imgui.cpp
    ${imgui_ext_SOURCE_DIR}/imgui.h
    ${imgui_ext_SOURCE_DIR}/imgui_internal.h
    ${imgui_ext_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_ext_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_ext_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_ext_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_ext_SOURCE_DIR}/imstb_rectpack.h
    ${imgui_ext_SOURCE_DIR}/imstb_textedit.h
    ${imgui_ext_SOURCE_DIR}/imstb_truetype.h
    ${imgui_ext_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_ext_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
    ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_glfw.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdl2.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdl3.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_opengl2.h

    ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_opengl3.h
    ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_opengl3_loader.h
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    # ${imgui_ext_SOURCE_DIR}/backends/imgui_impl_vulkan.h
)
target_include_directories(imgui PUBLIC ${imgui_ext_SOURCE_DIR})

target_link_libraries(imgui PUBLIC glfw)

# find_package(SDL2 REQUIRED)
# target_link_libraries(imgui PUBLIC ${SDL2_LIBARIES})
# target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})

# find_package(OpenGL)
