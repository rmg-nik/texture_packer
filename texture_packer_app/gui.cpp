#include "gui.hpp"

#include <texture_packer/image.hpp>
#include <texture_packer/pack_settings.hpp>

#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#include <fmt/core.h>
#include <portable-file-dialogs.h>

#include <filesystem>
#include <iostream>
#include <set>
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

        m_open_folder_ico.release();
        m_image_to_preview.release();
        m_renderer.release();
        m_window.release();
        SDL_Quit();
    }

    int Run() &&
    {
        while (m_is_running)
        {
            ProcessEvents();

            // Start the Dear ImGui frame
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            // ImGui::ShowDemoWindow(nullptr);

            ProcessDocking();

            // Menu Bar
            ProcessMenuBar();

            // Left Hierarchy view
            ProcessHierarchy();

            // Preview
            ProcessPreview();

            // Right Property view
            ProcessProperties();

            // Viewport
            ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove);
            ImGui::Text("Viewport contents");
            ImGui::End();

            // Rendering
            ImGui::Render();
            // SDL_SetRenderScale(m_renderer.get(),
            //                    ImGui::GetIO().DisplayFramebufferScale.x,
            //                    ImGui::GetIO().DisplayFramebufferScale.y);
            SDL_SetRenderDrawColor(
                m_renderer.get(), (Uint8)(20), (Uint8)(20), (Uint8)(20), (Uint8)(255));
            SDL_RenderClear(m_renderer.get());
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(m_renderer.get());
        }
        return 0;
    }

  private:
    void ProcessEvents()
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
            {
                m_is_running = false;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(m_window.get()))
            {
                m_is_running = false;
            }
        }
    }

    void ProcessDocking()
    {
        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                                                            ImGuiDockNodeFlags_PassthruCentralNode);

        static bool initialized = false;
        if (initialized)
        {
            return;
        }
        initialized = true;

        ImGuiID dock_id_0{};
        ImGuiID dock_id_1{};
        ImGuiID dock_id_2{};
        ImGuiID dock_id_3{};
        ImGuiID dock_id_4{};
        ImGuiID dock_id_5{};
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, &dock_id_0, &dock_id_1);
        ImGui::DockBuilderSplitNode(dock_id_1, ImGuiDir_Left, 0.75f, &dock_id_2, &dock_id_3);
        ImGui::DockBuilderSplitNode(dock_id_0, ImGuiDir_Up, 0.65f, &dock_id_4, &dock_id_5);
        ImGui::DockBuilderDockWindow("Hierarchy", dock_id_4);
        ImGui::DockBuilderDockWindow("Preview", dock_id_5);
        ImGui::DockBuilderDockWindow("Viewport", dock_id_2);
        ImGui::DockBuilderDockWindow("Properties", dock_id_3);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    void ProcessMenuBar()
    {
        // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {10.f, 10.f});
        if (ImGui::BeginMainMenuBar())
        {

            // if (m_open_folder_ico == nullptr)
            //{
            //     m_open_folder_ico =
            //         std::make_unique<TexturePacker::CImage>("./assets/open_folder_ico.png");
            // }
            // ImGui::ImageButton(m_open_folder_ico->GetTexture(m_renderer.get()), {10.f, 10.f});

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    m_selected_folder = pfd::select_folder("Select directory").result();
                    std::cout << "Selected dir: " << m_selected_folder << "\n";
                }
                if (ImGui::MenuItem("Exit"))
                {
                    m_is_running = false;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        // ImGui::PopStyleVar();
    }

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

            bool entry_is_file = std::filesystem::is_regular_file(entry.path());
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
        ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove);
        if (std::filesystem::exists(m_selected_folder) &&
            ImGui::CollapsingHeader(m_selected_folder.string().c_str()))
        {

            auto old_path = m_selected_path;
            DirectoryTreeViewRecursive(m_selected_folder);
            if (old_path != m_selected_path)
            {
                // SDL_Log("Selected path %s\n", selected_path.c_str());
                std::cout << "Selected path: " << m_selected_path << std::endl;
                static const std::set<std::string> kSupportedFormats = {
                    ".png", ".jpg", ".jpeg", ".bmp"};
                if (kSupportedFormats.contains(m_selected_path.extension().string()))
                {
                    m_image_to_preview =
                        std::make_unique<TexturePacker::CImage>(m_selected_path.string());
                }
                else
                {
                    m_image_to_preview.release();
                }
            }
        }
        ImGui::End();
    }

    void ProcessPreview()
    {
        ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoMove);
        if (m_image_to_preview != nullptr)
        {
            auto       cursor_pos = ImGui::GetCursorPos();
            const auto windows_size = ImGui::GetWindowSize();
            auto       texture = m_image_to_preview->GetTexture(m_renderer.get());
            ImVec2     texture_size{static_cast<float>(m_image_to_preview->Width()),
                                static_cast<float>(m_image_to_preview->Height())};

            float scale =
                std::min(windows_size.x / texture_size.x, windows_size.y / texture_size.y);
            if (scale > 1.f)
            {
                scale = 1.f;
            }
            texture_size.x *= scale;
            texture_size.y *= scale;
            cursor_pos = ImVec2{(windows_size.x - texture_size.x) / 2.f,
                                (windows_size.y - texture_size.y) / 2.f};

            ImGui::SetCursorPos(cursor_pos);
            ImGui::Image(static_cast<ImTextureID>(texture), texture_size);
        }
        ImGui::End();
    }

    void ProcessProperties()
    {
        ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoMove);
        ImGui::Text("Properties");
        ImGui::InputText("output folder", &m_pack_settings.atlases_output_dir);
        ImGui::SameLine();
        ImGui::Button("...");

        ImGui::InputText("output name", &m_pack_settings.atlases_pattern_name);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Sets the output name pattern. Should contain %% placeholder.");
        }

        ImGui::InputInt("max width", &m_pack_settings.max_width);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Sets the maximum width for the output texture.");
        }
        ImGui::InputInt("max height", &m_pack_settings.max_width);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Sets the maximum height for the output texture.");
        }

        ImGui::InputInt("extrude", &m_pack_settings.extrude);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip(
                "Extrude repeats the sprite's pixels at the border. Sprite's size is not changed.");
        }

        ImGui::InputInt("shape padding", &m_pack_settings.shape_padding);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Shape padding is the space between sprites. Value adds transparent\n"
                              "pixels between sprites to avoid artifacts from neighbor sprites.");
        }

        ImGui::InputInt("border padding", &m_pack_settings.border_padding);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip(
                "Border padding is the space between the sprites and the border of the sprite\n"
                "sheet. Value adds transparent pixels around the borders of the sprite sheet.");
        }

        ImGui::InputInt("trim below alpha", &m_pack_settings.trim_mode);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Pixels with an alpha value below this value will be considered\n"
                              "transparent when trimming the sprite.");
        }

        ImGui::InputDouble("scale", &m_pack_settings.scale, 0.01);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip(
                "Scaling factor of sprites before packing into a sprite sheet. The original\n"
                "dimensions of the sprite will be preserved in the atlas information.");
        }

        ImGui::Checkbox("force pot", &m_pack_settings.force_pot);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Forces the output texture to have a power of 2 size.");
        }

        ImGui::Checkbox("force square", &m_pack_settings.force_square);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Forces the output texture to have a squared size.");
        }

        ImGui::Checkbox("reduce border artifacts", &m_pack_settings.reduce_border_artifacts);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip(
                "Adds color to transparent pixels by repeating a sprite's outer color values.\n"
                "These color values can reduce artifacts around sprites and removes dark\n"
                "halos at transparent borders. This feature is also known as \"Alpha bleeding\"");
        }

        ImGui::End();
    }

  private:
    bool                                   m_is_running{true};
    WindowPtr                              m_window{nullptr};
    RenderPtr                              m_renderer{nullptr};
    std::unique_ptr<TexturePacker::CImage> m_image_to_preview{nullptr};
    std::unique_ptr<TexturePacker::CImage> m_open_folder_ico{nullptr};
    std::filesystem::path                  m_selected_folder;
    std::filesystem::path                  m_selected_path;
    TexturePacker::CPackSettings           m_pack_settings;
};

int run_gui(int, char**)
{
    TexturePackerGuiApplication().Run();

    return 0;
}
} // namespace TexturePackerApp
