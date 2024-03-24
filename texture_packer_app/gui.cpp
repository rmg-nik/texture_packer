#include "gui.hpp"

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

#include <fmt/core.h>
#include <portable-file-dialogs.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace TexturePackerApp
{

using WindowPtr = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>;
using RenderPtr = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>>;

class TexturePackerGuiApplication
{

  public:
    TexturePackerGuiApplication()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        {
            throw std::runtime_error(fmt::format("Error: SDL_Init(): {}", SDL_GetError()));
        }

        // Enable native IME.
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

        // Create window with SDL_Renderer graphics context
        Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED |
                              SDL_WINDOW_HIGH_PIXEL_DENSITY;
        m_window = WindowPtr(SDL_CreateWindow("Texture packer", 1280, 900, window_flags),
                             SDL_DestroyWindow);

        if (m_window == nullptr)
        {
            throw std::runtime_error(fmt::format("Error: SDL_CreateWindow(): {}", SDL_GetError()));
        }
        m_renderer = RenderPtr(
            SDL_CreateRenderer(
                m_window.get(), nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED),
            SDL_DestroyRenderer);
        if (m_renderer == nullptr)
        {
            throw std::runtime_error(
                fmt::format("Error: SDL_CreateRenderer(): {}", SDL_GetError()));
        }
        SDL_SetWindowPosition(m_window.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        const auto  scale = SDL_GetWindowDisplayScale(m_window.get());
        const float font_size = 20.f * scale;
        io.FontDefault =
            io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Regular.ttf", font_size);

        // Setup Platform/Renderer backends
        ImGui_ImplSDL3_InitForSDLRenderer(m_window.get(), m_renderer.get());
        ImGui_ImplSDLRenderer3_Init(m_renderer.get());
    }

    ~TexturePackerGuiApplication()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        m_renderer.release();
        m_window.release();
        SDL_Quit();
    }

    int Run() &&
    {
        bool done = false;
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
                    event.window.windowID == SDL_GetWindowID(m_window.get()))
                    done = true;
            }

            // Start the Dear ImGui frame
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            // ImGui::ShowDemoWindow(nullptr);

            ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(
                ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
            static bool init = true;
            if (init)
            {
                init = false;
                ImGuiID dock_id_0{};
                ImGuiID dock_id_1{};
                ImGuiID dock_id_2{};
                ImGuiID dock_id_3{};
                ImGuiID dock_id_4{};
                ImGuiID dock_id_5{};
                ImGui::DockBuilderRemoveNode(dockspace_id);
                ImGui::DockBuilderAddNode(dockspace_id);
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
                ImGui::DockBuilderSplitNode(
                    dockspace_id, ImGuiDir_Left, 0.2f, &dock_id_0, &dock_id_1);
                ImGui::DockBuilderSplitNode(dock_id_1, ImGuiDir_Left, 0.8f, &dock_id_2, &dock_id_3);
                ImGui::DockBuilderSplitNode(dock_id_0, ImGuiDir_Up, 0.65f, &dock_id_4, &dock_id_5);
                ImGui::DockBuilderDockWindow("Hierarchy", dock_id_4);
                ImGui::DockBuilderDockWindow("Preview", dock_id_5);
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
                        m_selected_folder = pfd::select_folder("Select directory").result();
                        std::cout << "Selected dir: " << m_selected_folder << "\n";
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
            ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove);
            ProcessHierarchy();
            ImGui::End();

            // Preview
            ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoMove);
            ImGui::Text("Preview contents");
            ImGui::End();

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
            //SDL_SetRenderScale(m_renderer.get(),
            //                   ImGui::GetIO().DisplayFramebufferScale.x,
            //                   ImGui::GetIO().DisplayFramebufferScale.y);
            SDL_SetRenderDrawColor(
                m_renderer.get(), (Uint8)(20), (Uint8)(20), (Uint8)(20), (Uint8)(255));
            SDL_RenderClear(m_renderer.get());
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(m_renderer.get());
        }
        return 0;
    }

  private:
    void DirectoryTreeViewRecursive(const std::filesystem::path& path)
    {
        ImGuiTreeNodeFlags base_flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
            ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            ImGuiTreeNodeFlags node_flags = base_flags;
            std::string        name = entry.path().string();

            auto last_slash = name.find_last_of("/\\");
            last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
            name = name.substr(last_slash, name.size() - last_slash);

            bool entry_is_file = !std::filesystem::is_directory(entry.path());
            if (entry_is_file)
            {
                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            }
            if (m_selected_path == entry)
            {
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }
            bool node_open = ImGui::TreeNodeEx(name.c_str(), node_flags);
            if (ImGui::IsItemClicked())
            {
                m_selected_path = entry;
            }
            if (!entry_is_file)
            {
                if (node_open)
                {
                    DirectoryTreeViewRecursive(entry.path());
                    ImGui::TreePop();
                }
            }
        }
    }

    void ProcessHierarchy()
    {
        if (!std::filesystem::exists(m_selected_folder))
        {
            return;
        }
        if (!ImGui::CollapsingHeader(m_selected_folder.string().c_str()))
        {
            return;
        }
        auto old_path = m_selected_path;
        DirectoryTreeViewRecursive(m_selected_folder);
        if (old_path != m_selected_path)
        {
            // SDL_Log("Selected path %s\n", selected_path.c_str());
            std::cout << "Selected path: " << m_selected_path << std::endl;
        }
    }

  private:
    WindowPtr             m_window{nullptr};
    RenderPtr             m_renderer{nullptr};
    std::filesystem::path m_selected_folder;
    std::filesystem::path m_selected_path;
};

int run_gui(int, char**)
{
    TexturePackerGuiApplication().Run();

    return 0;
}
} // namespace TexturePackerApp
