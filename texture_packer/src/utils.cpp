#include <texture_packer/utils.hpp>

#include <nlohmann/json.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>

// template <class K, class V, class dummy_compare, class A>
// using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
// using json = nlohmann::basic_json<my_workaround_fifo_map>;

namespace TexturePacker
{
std::vector<CImageInfo> load_image_infos_from_dir(const std::string& dir_path)
{
    std::vector<std::string> file_paths;

    for (auto& fe : std::filesystem::directory_iterator(dir_path))
    {
        auto file_path = fe.path().string();
        auto suffix = fe.path().extension().string();
        if (suffix.compare(".bmp") == 0 || suffix.compare(".jpg") == 0 ||
            suffix.compare(".jpeg") == 0 || suffix.compare(".webp") == 0 ||
            suffix.compare(".png") == 0)
        {
            file_paths.emplace_back(file_path);
        }
    }

    return load_image_infos_from_paths(file_paths);
}

std::vector<CImageInfo> load_image_infos_from_paths(const std::vector<std::string>& file_paths)
{
    std::vector<CImageInfo> image_infos;

    for (auto file_path : file_paths)
    {
        image_infos.emplace_back(CImageInfo(read_image_from_file(file_path), file_path));
    }

    return image_infos;
}

CImageInfo read_image_info_from_file(const std::string& file_path)
{
    return CImageInfo(read_image_from_file(file_path), file_path);
}

void save_image_to_file(const std::string& file_path, const CImage& image)
{
    std::filesystem::path tmp(file_path);
    if (!std::filesystem::exists(tmp.parent_path()))
        std::filesystem::create_directories(tmp.parent_path());

    auto suffix = file_path.substr(file_path.find_last_of("."));
    assert(!suffix.empty());
    if (suffix.compare(".jpg") == 0)
    {
        image.SaveAsJPEG(file_path.c_str());
    }
    else if (suffix.compare(".png") == 0)
    {
        image.SaveAsPNG(file_path.c_str());
    }
}

CImage read_image_from_file(const std::string& file_path)
{
    auto img = CImage(file_path.c_str());
    return img;
}

void draw_image_in_image(CImage& main_image, const CImage& sub_image, int start_x, int start_y)
{
    main_image.Composite(sub_image, start_x, start_y);
}

void dump_atlas_to_json(const std::string& file_path, const CAtlas& atlas,
                        ImageInfoMap& image_info_map, const std::string& texture_file_name)
{
    std::filesystem::path tmp(file_path);
    if (!std::filesystem::exists(tmp.parent_path()))
        std::filesystem::create_directories(tmp.parent_path());

    nlohmann::json root_json;
    nlohmann::json frames_json;

    for (const CImageRect& image_rect : atlas.GetPlacedImageRect())
    {
        const auto& image_info = image_info_map.at(image_rect.m_ex_key);

        const auto source_bbox = image_info.GetSourceBbox();
        const auto source_rect = image_info.GetSourceRect();

        const auto source_bbox_x = source_bbox.x - image_info.GetExtruded();
        const auto source_bbox_y = source_bbox.y - image_info.GetExtruded();
        // TODO
        // const auto source_bbox_w = source_bbox.width - image_info.GetExtruded();
        // const auto source_bbox_h = source_bbox.height - image_info.GetExtruded();

        std::filesystem::path image_path = image_info.GetImagePath();
        image_path = image_path.filename();

        nlohmann::json frame_data;
        frame_data["filename"] = image_path.string();
        frame_data["frame"]["x"] = image_rect.x + image_info.GetExtruded();
        frame_data["frame"]["y"] = image_rect.y + image_info.GetExtruded();
        frame_data["frame"]["w"] = source_bbox.width;
        frame_data["frame"]["h"] = source_bbox.height;
        frame_data["rotated"] = false;
        frame_data["padding"]["left"] = source_bbox_x;
        frame_data["padding"]["top"] = source_bbox_y;
        frame_data["padding"]["right"] = source_rect.width - source_bbox.width - source_bbox_x;
        frame_data["padding"]["bottom"] = source_rect.height - source_bbox.height - source_bbox_y;
        frame_data["sourceSize"]["w"] = image_info.GetSourceSize().w;
        frame_data["sourceSize"]["h"] = image_info.GetSourceSize().h;
        frames_json.push_back(frame_data);
    }

    root_json["frames"] = frames_json;

    nlohmann::json metadata;
    metadata["textureFileName"] = std::filesystem::path(texture_file_name).filename().string();
    metadata["size"]["w"] = atlas.GetWidth();
    metadata["size"]["h"] = atlas.GetHeight();

    root_json["metadata"] = metadata;

    std::ofstream fs(file_path);
    fs << root_json.dump(4);
}

ImageInfoMap make_image_info_map(const std::vector<CImageInfo>& image_infos)
{
    ImageInfoMap image_info_map;
    for (const CImageInfo& image_info : image_infos)
    {
        image_info_map.emplace(image_info.GetExKey(), image_info);
    }
    return image_info_map;
}

CImage dump_atlas_to_image(const CAtlas& atlas, const ImageInfoMap& image_info_map)
{
    CImage image(atlas.GetWidth(), atlas.GetHeight());
    for (auto image_rect : atlas.GetPlacedImageRect())
    {
        const auto& image_info = image_info_map.at(image_rect.m_ex_key);
        draw_image_in_image(image, image_info.GetImage(), image_rect.x, image_rect.y);
    }
    return image;
}
} // namespace TexturePacker
