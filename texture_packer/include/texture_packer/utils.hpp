#pragma once

#include <texture_packer/atlas.hpp>
#include <texture_packer/image.hpp>
#include <texture_packer/image_info.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace TexturePacker
{

using ImageInfoMap = std::unordered_map<unsigned int, CImageInfo>;

CImage read_image_from_file(const std::string& file_path);

void save_image_to_file(const std::string& file_path, const CImage& image);

CImageInfo read_image_info_from_file(const std::string& file_path);

std::vector<CImageInfo> load_image_infos_from_paths(const std::vector<std::string>& file_paths);

std::vector<CImageInfo> load_image_infos_from_dir(const std::string& dir_path);

void dump_atlas_to_json(const std::string& file_path, const CAtlas& atlas,
                        ImageInfoMap& image_info_map, const std::string& texture_file_name);

void draw_image_in_image(CImage& main_image, const CImage& sub_image, int start_x, int start_y);

CImage dump_atlas_to_image(const CAtlas& atlas, const ImageInfoMap& image_info_map);

ImageInfoMap make_image_info_map(const std::vector<CImageInfo>& image_infos);

} // namespace TexturePacker
