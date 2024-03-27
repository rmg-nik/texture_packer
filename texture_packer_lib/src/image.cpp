#include <texture_packer/image.hpp>

#include <array>
#include <memory>

// #include "magic_image.hpp"
#include "sdl_image.hpp"

namespace TexturePacker
{
CImage::CImage(const std::string& path)
{
  m_impl = std::make_unique<SdlImage>(path);
  // m_impl = std::make_unique<MagicImage>(path);
}

CImage::CImage(int w, int h)
{
  m_impl = std::make_unique<SdlImage>(w, h);
  // m_impl = std::make_unique<MagicImage>(w, h);
}

CImage::CImage(const CImage& other)
    : m_impl(other.m_impl->Clone())
{
}

CImage::CImage(CImage&& other) noexcept
    : m_impl(std::move(other.m_impl))
{
}

CImage& CImage::operator=(const CImage& other)
{
  m_impl = other.m_impl->Clone();
  return *this;
}

CImage& CImage::operator=(CImage&& other) noexcept
{
  m_impl = std::move(other.m_impl);
  return *this;
}

CImage::~CImage() = default;

[[nodiscard]]
int CImage::Width() const
{
  return m_impl->Width();
}

[[nodiscard]]
int CImage::Height() const
{
  return m_impl->Height();
}

[[nodiscard]]
int CImage::Channels() const
{
  return m_impl->Channels();
}

void CImage::SaveAsJPEG(const std::string& path) const
{
  m_impl->SaveAsJPEG(path);
}

void CImage::SaveAsPNG(const std::string& path) const
{
  m_impl->SaveAsPNG(path);
}

void CImage::Crop(int left, int top, int right, int bottom)
{
  m_impl->Crop(left, top, right, bottom);
}

void CImage::Scale(double scale)
{
  m_impl->Scale(scale);
}

void CImage::Composite(const CImage& src, int xOffset, int yOffset)
{
  m_impl->Composite(*src.m_impl, xOffset, yOffset);
}

Color CImage::GetColor(int x, int y) const
{
  return m_impl->GetColor(x, y);
}

void CImage::SetColor(int x, int y, Color value)
{
  m_impl->SetColor(x, y, value);
}

void CImage::EnlargeBorder(int size, bool repeat_border)
{
  CImage new_image(Width() + size * 2, Height() + size * 2);
  new_image.Composite(*this, size, size);

  std::swap(*this, new_image);

  if (!repeat_border)
  {
    return;
  }

  const auto w = Width();
  const auto h = Height();

  for (int offset_y = 0; offset_y < h; ++offset_y)
  {
    for (int offset_x = 0; offset_x < size; ++offset_x)
    {
      SetColor(offset_x, offset_y, GetColor(size, offset_y));
      SetColor(w - size + offset_x, offset_y, GetColor(w - size - 1, offset_y));
    }
  }

  for (int offset_x = 0; offset_x < w; ++offset_x)
  {
    for (int offset_y = 0; offset_y < size; ++offset_y)
    {
      SetColor(offset_x, offset_y, GetColor(offset_x, size));
      SetColor(offset_x, h - size + offset_y, GetColor(offset_x, h - size - 1));
    }
  }
}

CRect CImage::GetBoundingBox() const
{
  const int width = Width();
  const int height = Height();
  int       l = width;
  int       t = height;
  int       r = 0;
  int       b = 0;

  for (int x = 0; x < width; ++x)
  {
    for (int y = 0; y < height; ++y)
    {
      if (GetColor(x, y).a != 0)
      {
        if (x < l)
        {
          l = x;
        }
        if (y < t)
        {
          t = y;
        }
        if (x > r)
        {
          r = x;
        }

        if (y > b)
        {
          b = y;
        }
      }
    }
  }
  if (l > r)
  {
    l = r;
  }
  if (t > b)
  {
    t = b;
  }
  return CRect{l, t, r - l + 1, b - t + 1};
}

void CImage::CleanPixelAlphaBelow(const Channel alpha)
{
  const auto width = Width();
  const auto height = Height();
  for (int w = 0; w < width; ++w)
  {
    for (int h = 0; h < height; ++h)
    {
      if (GetColor(w, h).a < alpha)
      {
        SetColor(w, h, {0, 0, 0, 0});
      }
    }
  }
}

bool CImage::IsBorderPixel(int x, int y) const
{
  const auto width = Width();
  const auto height = Height();

  if (GetColor(x, y).a == 0)
  {
    return false;
  }

  const std::array<int, 3> offsets = {-1, 0, 1};
  for (const int offset_x : offsets)
  {
    const int nx = x + offset_x;
    for (const int offset_y : offsets)
    {
      const int ny = y + offset_y;
      if (nx >= 0 && nx < width && ny >= 0 && ny < height && GetColor(nx, ny).a == 0)
      {
        return true;
      }
    }
  }

  return false;
}

void CImage::AlphaBleeding(std::uint32_t bleeding_pixel)
{
  using Vec2 = std::tuple<int, int>;

  std::vector<Vec2> borders0;
  std::vector<Vec2> borders1;
  const auto        width = Width();
  const auto        height = Height();

  for (int x = 0; x < width; ++x)
  {
    for (int y = 0; y < height; ++y)
    {
      if (IsBorderPixel(x, y))
      {
        borders0.emplace_back(x, y);
      }
    }
  }

  const std::array<int, 3> offsets = {-1, 0, 1};
  std::vector<Vec2>*       borders = &borders0;
  std::vector<Vec2>*       new_borders = &borders1;

  for (std::uint32_t i = 0; i < bleeding_pixel; ++i)
  {
    for (auto border : *borders)
    {
      const int x = std::get<0>(border);
      const int y = std::get<1>(border);

      for (const int offset_x : offsets)
      {
        const int nx = x + offset_x;
        for (const int offset_y : offsets)
        {
          const int ny = y + offset_y;
          if (nx >= 0 && nx < width && ny >= 0 && ny < height && GetColor(nx, ny).a == 0)
          {
            auto color = GetColor(x, y);
            color.a = 1;
            SetColor(nx, ny, color);

            if (IsBorderPixel(nx, ny))
            {
              new_borders->emplace_back(Vec2{nx, ny});
            }
          }
        }
      }
    }

    auto tmp = borders;
    borders = new_borders;
    new_borders = tmp;
    new_borders->clear();
  }
}

SDL_Texture* CImage::GetTexture(SDL_Renderer* renderer)
{
  return m_impl->GetTexture(renderer);
}
} // namespace TexturePacker
