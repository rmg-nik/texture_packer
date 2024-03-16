#pragma once

#include <texture_packer/image.hpp>
#include <texture_packer/image_rect.hpp>

#include <string>

namespace TexturePacker
{
class CImageInfo
{
  public:
    CImageInfo(CImage _image, std::string _image_path);

    // ImageInfo(const ImageInfo& image_info);

    void Trim(unsigned char alpha_threshold);

    void Scale(double scale);

    [[nodiscard]]
    bool IsTrimmed() const;

    void Extrude(int size);

    [[nodiscard]]
    unsigned char GetExtruded() const;

    [[nodiscard]]
    CImageRect GetImageRect() const;

    [[nodiscard]]
    const CImage& GetImage() const;

    [[nodiscard]]
    std::uint32_t GetExKey() const;

    [[nodiscard]]
    CRect GetSourceRect() const;

    [[nodiscard]]
    CRect GetSourceBbox() const;

    [[nodiscard]]
    const std::string& GetImagePath() const;

    [[nodiscard]]
    Size GetSourceSize() const;

  private:
    CImage        m_image;
    std::string   m_image_path;
    CRect         m_source_rect;
    CRect         m_source_bbox;
    Size          m_source_size{};
    bool          m_trimmed{false};
    int           m_extruded{0};
    std::uint32_t m_ex_key{0};
};

}; // namespace TexturePacker
