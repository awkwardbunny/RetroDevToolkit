#include <spdlog/spdlog.h>
#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include <common/machine.hpp>
#include <common/registers.hpp>
#include <machine/apple_iie.hpp>

#include <goodasm.h>

static void glfw_error_callback(int error, const char* description) {
    spdlog::error("GLFW Error {}: {}\n", error, description);
}

class AppState {
public:
    bool isCPUShown = true;
    bool isStackShown = true;
    bool isMemoryShown = true;
    bool isCodeShown = true;
    bool running = false;

    REMachine *mach = 0;
    std::map<std::string,Register *> *regs;
    GoodASM *gas;

    AppState() {
        mach = new AppleIIe();
        regs = mach->getRegs()->getAll();
        gas = new GoodASM("6502");
    }
};

void CPUWindow(AppState *state) {
    if(!state->isCPUShown)
        return;

    ImGui::Begin("CPU", &(state->isCPUShown), ImGuiWindowFlags_AlwaysAutoResize);
    
    if(ImGui::Button("RESET")) state->mach->reset();
    ImGui::SameLine();
    if(ImGui::Button("RUN")) {
        state->running = !state->running;

        AppleIIe *m = (AppleIIe *)(state->mach);
        if(state->running) {
            m->load("/tmp/0", 0);
        } else {
            m->unload();
        }
        // state->mach->step();
    }
    ImGui::SameLine();
    if(ImGui::Button("STEP")) state->mach->step();

    if(state->running) {
        ImGui::SameLine();
        ImGui::Text("Running...");
    }

    ImGui::SeparatorText("Registers");

    ImGui::BeginDisabled(state->running);

    for(auto it = state->regs->begin(); it != state->regs->end(); it++) {
        std::string name = it->first;
        Register *r = it->second;

        ImGui::Text("%6s", name.c_str());
        ImGui::SameLine();

        if(r->disp == Register::BIN && r->help != nullptr) {
            for(int i = r->width-1; i; i--) {
                std::string flagname = std::string() + r->help[i];
                ImGui::PushID(flagname.c_str());
                if(r->help[i] == '-') {
                    ImGui::BeginDisabled();
                    ImGui::CheckboxFlags("##", (unsigned int *)r->ptr(), 1<<i);
                    ImGui::EndDisabled();
                } else {
                    ImGui::CheckboxFlags("##", (unsigned int *)r->ptr(), 1<<i);
                }
                ImGui::PopID();
                ImGui::SameLine();
            }
            std::string flagname = std::string() + r->help[0];
            ImGui::PushID(flagname.c_str());
            if(r->help[0] == '-') {
                ImGui::BeginDisabled();
                ImGui::CheckboxFlags("##", (unsigned int *)r->ptr(), 1);
                ImGui::EndDisabled();
            } else {
                ImGui::CheckboxFlags("##", (unsigned int *)r->ptr(), 1);
            }
            ImGui::PopID();

            ImGui::Text("%6s", name.c_str());
            ImGui::SameLine();
            for(int i = r->width-1; i; i--) {
                ImGui::Text(" %c", r->help[i]);
                ImGui::SameLine();
            }
            ImGui::Text(" %c", r->help[0]);

        } else {
            int flags = ImGuiInputTextFlags_CharsUppercase;
            if(r->disp != Register::DEC) {
                flags |= ImGuiInputTextFlags_CharsHexadecimal;
            }

            // ImGui::PushItemWidth(80);
            ImGui::PushID(name.c_str());
            ImGui::InputInt("##", (int *)r->ptr(), 0, 0, flags);
            ImGui::PopID();
            // ImGui::PopItemWidth();

            r->set(r->get()); // Fix invalid values
        }

    }

    ImGui::EndDisabled();
    ImGui::End();

}

void CodeWindow(AppState *state) {
    if(!state->isCodeShown)
        return;

    ImGui::SetNextWindowSize(ImVec2(0, 500));
    ImGui::Begin("Code", &(state->isCodeShown), ImGuiWindowFlags_AlwaysAutoResize);
    ImU32 hl = ImGui::GetColorU32(ImVec4(0.9f, 0.0f, 0.0f, 0.9f));
    if(ImGui::BeginTable("code", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersV )) {
        int addr = (**(*state->regs)["PC"]);
        for(int i = 0; i < 40; i++) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%x", addr);
            if(!i) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, hl);

            state->gas->clear();
            QByteArray instr = QByteArray();
            AppleIIe *m = (AppleIIe *)(state->mach);
            instr.append((*(m->mem))[addr]);
            instr.append((*(m->mem))[addr+1]);
            instr.append((*(m->mem))[addr+2]);
            state->gas->load(instr);
            QList<GAInstruction> ins = state->gas->instructions;

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s %s", ins[0].verb.toStdString().c_str(), ins[0].params.toStdString().c_str());
            if(!i) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, hl);

            ImGui::TableSetColumnIndex(2);
            QByteArray b = ins[0].data;
            ImGui::Text("%s", b.toHex(' ').toStdString().c_str());
            addr += b.length();
        }

        ImGui::EndTable();
    }
    ImGui::End();
}

void MemoryWindow(AppState *state) {
    if(!state->isMemoryShown)
        return;

    ImGui::SetNextWindowSize(ImVec2(0, 800));
    ImGui::Begin("Memory", &(state->isMemoryShown), ImGuiWindowFlags_AlwaysAutoResize);
    if(ImGui::BeginTable("Memory", 17, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY )) {
        for(int i = 0; i < 1 << 16; i += 16) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%04x", i);
        
            for(int j = 0; j < 16; j++) {
                ImGui::TableSetColumnIndex(j+1);
                AppleIIe *m = (AppleIIe *)(state->mach);
                ImGui::PushID(i+j);
                // ImGui::Text("%02x", (*(m->mem))[i+j]);
                ImGui::PushItemWidth(22);
                ImGui::InputScalar("##mem", ImGuiDataType_U8, (int *)m->mem->ptr(i+j), NULL, NULL, "%02X", ImGuiInputTextFlags_CharsUppercase );
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
            // ImGui::PushID(addr);
            // int flags = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal;
            // ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

void StackWindow(AppState *state) {
    if(!state->isStackShown)
        return;

    ImGui::SetNextWindowSize(ImVec2(0, 600));
    ImGui::Begin("Stack", &(state->isStackShown), ImGuiWindowFlags_AlwaysAutoResize);

    ImU32 hl = ImGui::GetColorU32(ImVec4(0.9f, 0.0f, 0.0f, 0.9f));
    if(ImGui::BeginTable("stack", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY )) {
        for(int i = 255; i >= 0; i--) {
            int addr = 0x100+i;
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%x", addr);
            if(addr == (**(*state->regs)["SP"])) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, hl);
            }
        
            ImGui::TableSetColumnIndex(1);
            ImGui::PushID(addr);
            int flags = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_CharsHexadecimal;
            AppleIIe *m = (AppleIIe *)(state->mach);
            ImGui::PushItemWidth(22);
            // ImGui::InputInt("##stack", (int *)m->mem->ptr(addr),0, 0, flags);
            ImGui::InputScalar("##stack", ImGuiDataType_U8, (int *)m->mem->ptr(addr), NULL, NULL, "%02x", ImGuiInputTextFlags_CharsUppercase );
            ImGui::PopItemWidth();
            ImGui::PopID();

            if(addr == (**(*state->regs)["SP"])) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, hl);
            }
        }
        ImGui::EndTable();
    }

    ImGui::End();
}    

void mainLoop(AppState *state) {
    if(ImGui::BeginMainMenuBar()){
        if(ImGui::BeginMenu("View")) {
            if(ImGui::MenuItem("CPU", NULL, state->isCPUShown, true)) { state->isCPUShown ^= 1; }
            if(ImGui::MenuItem("Stack", NULL, state->isStackShown, true)) { state->isStackShown ^= 1; }
            if(ImGui::MenuItem("Memory", NULL, state->isMemoryShown, true)) { state->isMemoryShown ^= 1; }
            if(ImGui::MenuItem("Code", NULL, state->isCodeShown, true)) { state->isCodeShown ^= 1; }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    CPUWindow(state);
    StackWindow(state);
    MemoryWindow(state);
    CodeWindow(state);
}

int startGui(AppState *state) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        spdlog::error("Could not initialize GLFW!");
        return 1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "RetroDevToolkit", nullptr, nullptr);
    if (window == nullptr) {
        spdlog::error("Could not create window!");
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        mainLoop(state);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
}

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug);

    AppState *state = new AppState();

    return startGui(state);

    // AppleIIe *machine = new AppleIIe();

    // machine->print();
    // // machine->run();
    // for (int i = 0; i < 3; i++)
    // {
    //     machine->step();
    //     machine->print();
    // }

    // delete machine;
}
