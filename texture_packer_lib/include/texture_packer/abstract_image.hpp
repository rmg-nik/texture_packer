#pragma once

#include <cstdint>
#include <memory>
#include <string>

struct SDL_Texture;
struct SDL_Renderer;

namespace TexturePacker
{
using Channel = std::uint8_t;

struct Color
{
  Channel r{};
  Channel g{};
  Channel b{};
  Channel a{};
};

class CAbstractImage
{
public:
  virtual ~CAbstractImage() = default;

  [[nodiscard]]
  virtual int Width() const = 0;

  [[nodiscard]]
  virtual int Height() const = 0;

  [[nodiscard]]
  virtual int Channels() const = 0;

  virtual void SaveAsJPEG(const std::string& path) const = 0;

  virtual void SaveAsPNG(const std::string& path) const = 0;

  virtual void Crop(int left, int top, int right, int bottom) = 0;

  virtual void Scale(double scale) = 0;

  virtual void Composite(const CAbstractImage& src, int xOffset, int yOffset) = 0;

  [[nodiscard]]
  virtual Color GetColor(int x, int y) const = 0;

  virtual void SetColor(int x, int y, Color value) = 0;

  [[nodiscard]]
  virtual std::unique_ptr<CAbstractImage> Clone() const noexcept = 0;

  [[nodiscard]]
  virtual SDL_Texture* GetTexture(SDL_Renderer* renderer) = 0;
};
} // namespace TexturePacker
