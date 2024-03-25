#pragma once

#include <string>

namespace TexturePacker
{
struct CPackSettings
{
    constexpr static int kDefaultAtlasSize{4096};

    bool        reduce_border_artifacts{false};
    bool        force_square{false};
    bool        force_pot{false};
    int         trim_mode{0};
    int         extrude{0};
    int         max_width{kDefaultAtlasSize};
    int         max_height{kDefaultAtlasSize};
    int         border_padding{0};
    int         shape_padding{2};
    double      scale{1.0};
    std::string images_input_dir;
    std::string atlases_output_dir;
    std::string atlases_pattern_name{"atlas_%02d"};
    std::string atlases_output_format{"png"};
};

class CPackSettingsBuilder
{
  public:
    CPackSettingsBuilder& WithReduceBorderArtifacts(bool reduce_border_artifacts)
    {
        m_settings.reduce_border_artifacts = reduce_border_artifacts;
        return *this;
    }

    CPackSettingsBuilder& WithForceSquare(bool force_square)
    {
        m_settings.force_square = force_square;
        return *this;
    }

    CPackSettingsBuilder& WithForcePOT(bool force_pot)
    {
        m_settings.force_pot = force_pot;
        return *this;
    }

    CPackSettingsBuilder& WithTrimMode(int trim_mode)
    {
        m_settings.trim_mode = trim_mode;
        return *this;
    }

    CPackSettingsBuilder& WithExtrude(int extrude)
    {
        m_settings.extrude = extrude;
        return *this;
    }

    CPackSettingsBuilder& WithMaxWidth(int max_width)
    {
        m_settings.max_width = max_width;
        return *this;
    }

    CPackSettingsBuilder& WithMaxHeight(int max_height)
    {
        m_settings.max_height = max_height;
        return *this;
    }

    CPackSettingsBuilder& WithBorderPadding(int border_padding)
    {
        m_settings.border_padding = border_padding;
        return *this;
    }

    CPackSettingsBuilder& WithShapePadding(int shape_padding)
    {
        m_settings.shape_padding = shape_padding;
        return *this;
    }

    CPackSettingsBuilder& WithScale(double scale)
    {
        m_settings.scale = scale;
        return *this;
    }

    CPackSettingsBuilder& WithImagesInputDir(std::string images_input_dir)
    {
        m_settings.images_input_dir = std::move(images_input_dir);
        return *this;
    }

    CPackSettingsBuilder& WithAtlasesOutputDir(std::string atlases_output_dir)
    {
        m_settings.atlases_output_dir = std::move(atlases_output_dir);
        return *this;
    }

    CPackSettingsBuilder& WithAtlasesPatternName(std::string atlases_pattern_name)
    {
        m_settings.atlases_pattern_name = atlases_pattern_name;
        return *this;
    }

    CPackSettingsBuilder& WithAtlasesOutputFormat(std::string atlases_output_format)
    {
        m_settings.atlases_output_format = std::move(atlases_output_format);
        return *this;
    }

    CPackSettings Build()
    {
        return m_settings;
    }

  private:
    CPackSettings m_settings;
};
} // namespace TexturePacker
