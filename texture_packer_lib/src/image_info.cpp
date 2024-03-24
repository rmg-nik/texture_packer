#include <texture_packer/image_info.hpp>
#include <texture_packer/utils.hpp>

namespace TexturePacker
{
CImageInfo::CImageInfo(CImage _image, std::string _image_path)
    : m_image(std::move(_image))
    , m_image_path(std::move(_image_path))
    , m_source_rect({0, 0, m_image.Width(), m_image.Height()})
    , m_source_bbox({0, 0, m_image.Width(), m_image.Height()})
    , m_source_size{m_image.Width(), m_image.Height()}
{
    static std::uint32_t ex_key_counter = 0;
    m_ex_key = ex_key_counter++;
}

/*
   ImageInfo:: ImageInfo(const ImageInfo& image_info)
        : m_image(image_info.m_image)
        , m_image_path(image_info.m_image_path)
        , m_source_rect(image_info.m_source_rect)
        , m_source_bbox(image_info.m_source_bbox)
        , m_source_size(image_info.m_source_size)
        , m_trimmed(image_info.m_trimmed)
        , m_extruded(image_info.m_extruded)
        , m_ex_key(image_info.m_ex_key)
    {
    }
*/
Size CImageInfo::GetSourceSize() const
{
    return m_source_size;
}

const std::string& CImageInfo::GetImagePath() const
{
    return m_image_path;
}

CRect CImageInfo::GetSourceBbox() const
{
    return m_source_bbox;
}

CRect CImageInfo::GetSourceRect() const
{
    return m_source_rect;
}

std::uint32_t CImageInfo::GetExKey() const
{
    return m_ex_key;
}

const CImage& CImageInfo::GetImage() const
{
    return m_image;
}

CImageRect CImageInfo::GetImageRect() const
{
    CImageRect image_rect;
    image_rect.x = 0;
    image_rect.y = 0;
    image_rect.width = m_image.Width();
    image_rect.height = m_image.Height();
    image_rect.m_ex_key = m_ex_key;
    return image_rect;
}

unsigned char CImageInfo::GetExtruded() const
{
    return m_extruded;
}

void CImageInfo::Extrude(int size)
{
    if (m_extruded)
    {
        return;
    }

    m_extruded = size;
    m_image.EnlargeBorder(m_extruded, true);

    m_source_rect.x += m_extruded;
    m_source_rect.y += m_extruded;

    m_source_bbox.x += m_extruded;
    m_source_bbox.y += m_extruded;
}

bool CImageInfo::IsTrimmed() const
{
    return m_trimmed;
}

void CImageInfo::Scale(double scale)
{
    m_image.Scale(scale);
    m_source_rect = {0, 0, m_image.Width(), m_image.Height()};
    m_source_bbox = {0, 0, m_image.Width(), m_image.Height()};
}

void CImageInfo::Trim(unsigned char alpha_threshold)
{
    m_trimmed = true;
    m_image.CleanPixelAlphaBelow(alpha_threshold);
    m_source_bbox = m_image.GetBoundingBox();
    m_image.Crop(m_source_bbox.get_left(),
                 m_source_bbox.get_top(),
                 m_source_bbox.get_right(),
                 m_source_bbox.get_bottom());
}
} // namespace TexturePacker
