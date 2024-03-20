#pragma once

#include <texture_packer/abstract_image.hpp>

#include <memory>
#include <mutex>
#include <stdexcept>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace TexturePacker
{
class SdlImage : public CAbstractImage
{
  public:
    SdlImage(const std::string& path)
    {
        static std::once_flag init_image_magic;
        std::call_once(init_image_magic, []() {
            if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
            {
                throw std::runtime_error(SDL_GetError());
            }
        });
        m_surface = IMG_Load(path.c_str());
        if (m_surface == nullptr)
        {
            throw std::runtime_error(SDL_GetError());
        }
        ConvertToRGBA();
    }

    SdlImage(int w, int h)
        : m_surface(SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32))
    {
    }

    SdlImage(const SdlImage& other)
        : m_surface(SDL_CreateSurface(other.m_surface->w, other.m_surface->w,
                                      other.m_surface->format->format))
    {

        if (SDL_BlitSurface(other.m_surface, nullptr, m_surface, nullptr) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    ~SdlImage() override
    {
        SDL_DestroySurface(m_surface);
    }

    [[nodiscard]]
    int Width() const override
    {
        return m_surface->w;
    }

    [[nodiscard]]
    int Height() const override
    {
        return m_surface->h;
    }

    [[nodiscard]]
    int Channels() const override
    {
        return m_surface->format->bytes_per_pixel;
    }

    void SaveAsJPEG(const std::string& path) const override
    {
        constexpr int quality = 100;
        if (IMG_SaveJPG(m_surface, path.c_str(), quality) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void SaveAsPNG(const std::string& path) const override
    {
        if (IMG_SavePNG(m_surface, path.c_str()) < 0)
        {
            throw std::runtime_error(SDL_GetError());
        }
    }

    void Crop(int left, int top, int right, int bottom) override
    {
        auto old_sfc = m_surface;
        int  new_w = m_surface->w - right - left;
        int  new_h = m_surface->h - top - bottom;
        m_surface = SDL_CreateSurface(new_w, new_h, m_surface->format->format);
        SDL_Rect src_rect;
        src_rect.x = left;
        src_rect.y = top;
        src_rect.w = new_w;
        src_rect.h = new_h;
        SDL_BlitSurface(old_sfc, &src_rect, m_surface, nullptr);
        SDL_DestroySurface(old_sfc);
    }

    void Scale(double scale) override
    {
        int  new_w = std::max(1, static_cast<int>(round(Width() * scale)));
        int  new_h = std::max(1, static_cast<int>(round(Height() * scale)));
        auto old_sfc = m_surface;
        m_surface = SDL_CreateSurface(new_w, new_h, m_surface->format->format);
        SDL_BlitSurface(old_sfc, nullptr, m_surface, nullptr);
        SDL_DestroySurface(old_sfc);
    }

    [[nodiscard]]
    Color GetColor(int x, int y) const override
    {
        Color color;
        auto  pixel = (Uint32*)((Uint8*)m_surface->pixels + y * m_surface->pitch +
                               x * m_surface->format->bytes_per_pixel);

        SDL_GetRGBA(*pixel, m_surface->format, &color.r, &color.g, &color.b, &color.a);
        return color;
    }

    void SetColor(int x, int y, Color color) override
    {
        auto pixel = (Uint32*)((Uint8*)m_surface->pixels + y * m_surface->pitch +
                               x * m_surface->format->bytes_per_pixel);
        *pixel = SDL_MapRGBA(m_surface->format, color.r, color.g, color.b, color.a);
    }

    void Composite(const CAbstractImage& src, int xOffset, int yOffset) override
    {
        const auto& sdl_src = dynamic_cast<const SdlImage&>(src);
        SDL_Rect    dst;
        dst.x = xOffset;
        dst.y = yOffset;
        dst.w = src.Width();
        dst.h = src.Height();
        SDL_BlitSurface(sdl_src.m_surface, nullptr, m_surface, &dst);
    }

    void ConvertToRGBA() override
    {
        if (m_surface->format->format == SDL_PIXELFORMAT_RGBA32)
        {
            return;
        }
        auto old_sfc = m_surface;
        m_surface = SDL_ConvertSurfaceFormat(m_surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(old_sfc);
    }

    [[nodiscard]]
    std::unique_ptr<CAbstractImage> Clone() const noexcept override
    {
        return std::make_unique<SdlImage>(*this);
    }

  private:
    SDL_Surface* m_surface{nullptr};
};
} // namespace TexturePacker
