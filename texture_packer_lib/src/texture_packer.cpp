#include <texture_packer/texture_packer.hpp>
#include <texture_packer/utils.hpp>

#include <fmt/printf.h>

#include <algorithm>
#include <cassert>
#include <filesystem>

namespace TexturePacker
{

/*
void CTexturePacker::Pack(const std::vector<std::string>& image_paths,
                          const std::string& output_dir, const std::string& output_name,
                          const std::string& image_format)
{
    auto image_infos = load_image_infos_from_paths(image_paths);
    return Pack(image_infos, output_dir, output_name, image_format);
}
*/

void CTexturePacker::Pack(const std::vector<CImageInfo>& image_infos, const CPackSettings& settings)
{
    m_atlases.clear();
    m_atlases.emplace_back(settings.max_width,
                           settings.max_height,
                           settings.force_square,
                           settings.force_pot,
                           settings.border_padding,
                           settings.shape_padding);

    auto image_infos_copy = image_infos;

    if (settings.scale != 1.0)
    {
        for (CImageInfo& image_info : image_infos_copy)
        {
            image_info.Scale(settings.scale);
        }
    }
    if (settings.trim_mode > 0)
    {
        for (CImageInfo& image_info : image_infos_copy)
        {
            image_info.Trim(settings.trim_mode);
        }
    }

    if (settings.extrude > 0)
    {
        for (CImageInfo& image_info : image_infos_copy)
        {
            image_info.Extrude(settings.extrude);
        }
    }

    std::vector<CImageRect> image_rects;
    for (const auto& image_info : image_infos_copy)
    {
        image_rects.emplace_back(image_info.GetImageRect());
    }

    AddImageRects(image_rects, settings);
    for (auto& atlas : m_atlases)
    {
        atlas.Shrink();
    }
    auto image_info_map = make_image_info_map(image_infos_copy);

    for (std::size_t i = 0; i < m_atlases.size(); ++i)
    {
        const auto&       atlas = m_atlases[i];
        const std::string atlas_name = fmt::sprintf(settings.atlases_pattern_name, i);
        const std::string image_file_name = atlas_name + "." + settings.atlases_output_format;
        const std::string json_file_name = atlas_name + ".json";
        const std::filesystem::path image_path =
            settings.atlases_output_dir + "/" + image_file_name;
        const std::filesystem::path json_path = settings.atlases_output_dir + "/" + json_file_name;
        auto                        image = dump_atlas_to_image(atlas, image_info_map);
        if (settings.reduce_border_artifacts)
        {
            image.AlphaBleeding();
        }
        save_image_to_file(image_path.string(), image);
        dump_atlas_to_json(json_path.string(), atlas, image_info_map, image_file_name);
    }
}

void CTexturePacker::Pack(const CPackSettings& settings)
{
    auto image_infos = load_image_infos_from_dir(settings.images_input_dir);
    return Pack(image_infos, settings);
}

void CTexturePacker::AddImageRects(std::vector<CImageRect> image_rects,
                                   const CPackSettings&    settings)
{
    std::sort(image_rects.begin(),
              image_rects.end(),
              [](const CImageRect& a, const CImageRect& b)
              { return std::max(a.width, a.height) > std::max(b.width, b.height); });

    for (auto image_rect : image_rects)
    {
        AddImageRect(image_rect, settings);
    }
}

void CTexturePacker::AddImageRect(CImageRect image_rect, const CPackSettings& settings)
{
    unsigned int best_atlas_index = -1;
    unsigned int best_free_rect_index = -1;
    unsigned int best_rank = MAX_RANK;
    bool         best_rotated = false;

    unsigned int rank{};
    unsigned int free_rect_index{};
    bool         rotated{};

    for (std::size_t atlas_index = 0; atlas_index < m_atlases.size(); ++atlas_index)
    {
        std::tie(rank, free_rect_index, rotated) = m_atlases[atlas_index].FindBestRank(image_rect);

        if (rank < best_rank)
        {
            best_atlas_index = atlas_index;
            best_rank = rank;
            best_free_rect_index = free_rect_index;
            best_rotated = rotated;
        }
    }

    if (best_rank == MAX_RANK)
    {
        for (std::size_t atlas_index = 0; atlas_index < m_atlases.size(); ++atlas_index)
        {
            while (MAX_RANK == best_rank)
            {
                if (m_atlases[atlas_index].TryExpand())
                {
                    best_atlas_index = atlas_index;
                    std::tie(best_rank, best_free_rect_index, best_rotated) =
                        m_atlases[atlas_index].FindBestRank(image_rect);
                }
                else
                {
                    break;
                }
            }

            if (MAX_RANK != best_rank)
            {
                break;
            }
        }

        if (best_rank == MAX_RANK)
        {
            m_atlases.emplace_back(settings.max_width,
                                   settings.max_height,
                                   settings.force_square,
                                   settings.force_pot,
                                   settings.border_padding,
                                   settings.shape_padding);

            best_atlas_index = (unsigned int)m_atlases.size() - 1;
            std::tie(best_rank, best_free_rect_index, best_rotated) =
                m_atlases[best_atlas_index].FindBestRank(image_rect);

            while (MAX_RANK == best_rank)
            {
                if (!m_atlases[best_atlas_index].TryExpand())
                {
                    assert(false); // can not place image in max size
                }

                std::tie(best_rank, best_free_rect_index, best_rotated) =
                    m_atlases[best_atlas_index].FindBestRank(image_rect);
            }
        }
    }

    if (best_rotated)
    {
        image_rect.rotate();
    }

    m_atlases[best_atlas_index].PlaceImageRectInFreeRect(best_free_rect_index, image_rect);
}
} // namespace TexturePacker
