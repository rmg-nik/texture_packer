#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <portable-file-dialogs.h>

#include <filesystem>

namespace TexturePackerApp
{

void DirectoryTreeViewRecursive(const std::filesystem::path& path)
{
    ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        ImGuiTreeNodeFlags node_flags = base_flags;
        std::string        name = entry.path().string();

        auto lastSlash = name.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        name = name.substr(lastSlash, name.size() - lastSlash);

        bool entryIsFile = !std::filesystem::is_directory(entry.path());
        if (entryIsFile)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }
        if (ImGui::IsItemFocused())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }
        bool node_open = ImGui::TreeNodeEx(name.c_str(), node_flags);
        if (!entryIsFile)
        {
            if (node_open)
            {
                DirectoryTreeViewRecursive(entry.path());
                ImGui::TreePop();
            }
        }
    }
}

void ProcessHierarchy(std::string directoryPath)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});

    ImGui::Begin("Hierarchy",
                 nullptr,
                 ImGuiWindowFlags_NoMove || ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    if (ImGui::CollapsingHeader(directoryPath.empty() ? " " : directoryPath.c_str()))
    {
        if (std::filesystem::exists(directoryPath))
        {
            DirectoryTreeViewRecursive(directoryPath);
        }
    }
    ImGui::End();

    ImGui::PopStyleVar();
}

int run_gui(int, char**)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Enable native IME.
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    // Create window with SDL_Renderer graphics context
    Uint32      window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
    SDL_Window* window = SDL_CreateWindow("Texture packer", 1280, 720, window_flags);
    if (window == nullptr)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    const float fontSize = 20.f;
    io.FontDefault =
        io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Regular.ttf", fontSize);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool   show_demo_window{true};
    // Main loop
    bool        done = false;
    std::string active_dir = "";
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
        // tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
        // your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
        // data to your main application, or clear/overwrite your copy of the
        // keyboard data. Generally you may always pass all inputs to dear imgui,
        // and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // ImGui::ShowDemoWindow(&show_demo_window);

        ImGuiID     dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                                                            ImGuiDockNodeFlags_PassthruCentralNode);
        static bool init = true;
        if (init)
        {
            init = false;
            ImGuiID dock_id_0{};
            ImGuiID dock_id_1{};
            ImGuiID dock_id_2{};
            ImGuiID dock_id_3{};
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
            ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, &dock_id_0, &dock_id_1);
            ImGui::DockBuilderSplitNode(dock_id_1, ImGuiDir_Left, 0.9f, &dock_id_2, &dock_id_3);
            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_0);
            ImGui::DockBuilderDockWindow("Viewport", dock_id_2);
            ImGui::DockBuilderDockWindow("Properties", dock_id_3);
            ImGui::DockBuilderFinish(dockspace_id);
        }

        // Menu Bar
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    active_dir = pfd::select_folder("Select directory", pfd::path::home()).result();
                    std::cout << "Selected dir: " << active_dir << "\n";
                }
                if (ImGui::MenuItem("Exit"))
                {
                    done = true;
                    continue;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Left Hierarchy view
        ProcessHierarchy(active_dir);
        /*
        ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::Text("Left pane contents");
        ImGui::End();
*/
        // Right Property view
        ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::Text("Right pane contents");
        ImGui::End();

        // Viewport
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::Text("Viewport contents");
        ImGui::End();

        // Rendering
        ImGui::Render();
        // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
        // io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer,
                               (Uint8)(clear_color.x * 255),
                               (Uint8)(clear_color.y * 255),
                               (Uint8)(clear_color.z * 255),
                               (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
} // namespace TexturePackerApp
