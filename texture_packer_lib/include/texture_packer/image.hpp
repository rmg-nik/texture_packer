#pragma once

#include <texture_packer/abstract_image.hpp>
#include <texture_packer/rect.hpp>

#include <memory>

namespace TexturePacker
{
class CImage
{
  public:
    CImage(const std::string& path);

    CImage(int w, int h);

    CImage(const CImage& other);

    CImage(CImage&& other) noexcept;

    CImage& operator=(const CImage& other);

    CImage& operator=(CImage&& other) noexcept;

    ~CImage();

    [[nodiscard]]
    int Width() const;

    [[nodiscard]]
    int Height() const;

    [[nodiscard]]
    int Channels() const;

    void SaveAsJPEG(const std::string& path) const;

    void SaveAsPNG(const std::string& path) const;

    void Crop(int left, int top, int right, int bottom);

    void Scale(double scale);

    void Composite(const CImage& src, int xOffset, int yOffset);

    [[nodiscard]]
    Color GetColor(int x, int y) const;

    void SetColor(int x, int y, Color value);

    void EnlargeBorder(int size, bool repeat_border);

    [[nodiscard]]
    CRect GetBoundingBox() const;

    void CleanPixelAlphaBelow(const Channel alpha);

    [[nodiscard]]
    bool IsBorderPixel(int x, int y) const;

    void AlphaBleeding(std::uint32_t bleeding_pixel = 4);

  private:
    std::unique_ptr<CAbstractImage> m_impl;
};
} // namespace TexturePacker
