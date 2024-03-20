#include "cli.hpp"

#include <cxxopts.hpp>
#include <texture_packer/texture_packer.hpp>

namespace TexturePackerApp
{
int run_cli(int argc, char** argv)
{
    cxxopts::Options options("Texture Packer CLI", "A CLI interface to my texture packer");

    // clang-format off
    options.add_options()
        ("input_dir", "input dir", cxxopts::value<std::string>())
        ("output_dir", "output folder", cxxopts::value<std::string>()->default_value("./"))
        ("output_name", "output atlas name (with placeholder '%d')", cxxopts::value<std::string>())
        ("image_format", "output image format {png, jpg}", cxxopts::value<std::string>()->default_value("png"))
        ("max_width", "max atlas Width", cxxopts::value<int>()->default_value("4096"))
        ("max_height", "max atlas Height", cxxopts::value<int>()->default_value("4096"))
        ("force_square", "force square", cxxopts::value<bool>()->default_value("false"))
        ("force_pot", "force power of 2", cxxopts::value<bool>()->default_value("false"))
        ("border_padding", "border padding", cxxopts::value<int>()->default_value("0"))
        ("shape_padding", "shape padding", cxxopts::value<int>()->default_value("0"))
        ("reduce_border_artifacts", "reduce border artifacts", cxxopts::value<bool>()->default_value("false"))
        ("trim_mode", "Trim pixel alpha less than input value", cxxopts::value<int>()->default_value("0"))
        ("extrude", "extrude", cxxopts::value<int>()->default_value("0"))
        ("scale", "scale", cxxopts::value<double>()->default_value("1.0"))
        ;
    // clang-format on
    auto result = options.parse(argc, argv);

    TexturePacker::CPackSettingsBuilder settings_builder;
    settings_builder.WithAtlasesOutputDir(result["output_dir"].as<std::string>())
        .WithAtlasesOutputFormat(result["image_format"].as<std::string>())
        .WithImagesInputDir(result["input_dir"].as<std::string>())
        .WithAtlasesPatternName(result["output_name"].as<std::string>())
        .WithMaxWidth(result["max_width"].as<int>())
        .WithMaxHeight(result["max_height"].as<int>())
        .WithForceSquare(result["force_square"].as<bool>())
        .WithForcePOT(result["force_pot"].as<bool>())
        .WithBorderPadding(result["border_padding"].as<int>())
        .WithShapePadding(result["shape_padding"].as<int>())
        .WithReduceBorderArtifacts(result["reduce_border_artifacts"].as<bool>())
        .WithTrimMode(result["trim_mode"].as<int>())
        .WithExtrude(result["extrude"].as<int>())
        .WithScale(result["scale"].as<double>());
    TexturePacker::CTexturePacker packer;
    packer.Pack(settings_builder.Build());

    return 0;
}
} // namespace TexturePackerApp
