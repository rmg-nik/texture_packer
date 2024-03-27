#pragma once

#include <texture_packer/atlas.hpp>
#include <texture_packer/image_info.hpp>
#include <texture_packer/pack_settings.hpp>

#include <vector>

namespace TexturePacker
{
class CTexturePacker
{
public:
  void Pack(const std::vector<CImageInfo>& image_infos, const CPackSettings& settings);

  /*
  void Pack(const std::vector<std::string>& image_paths, const std::string& output_dir,
            const std::string& output_name, const std::string& image_format = "png");
  */

  void Pack(const CPackSettings& settings);

private:
  void AddImageRect(CImageRect image_rect, const CPackSettings& settings);

  void AddImageRects(std::vector<CImageRect> image_rects, const CPackSettings& settings);

  std::vector<CAtlas> m_atlases;
};
} // namespace TexturePacker
