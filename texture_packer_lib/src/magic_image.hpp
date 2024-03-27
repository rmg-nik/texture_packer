#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#endif
#include <Magick++.h>
#include <MagickCore/MagickCore.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <texture_packer/abstract_image.hpp>

#include <cassert>
#include <iostream>
#include <memory>
#include <mutex>

namespace TexturePacker
{
class MagicImage : public CAbstractImage
{
public:
  MagicImage(const std::string& path)
  {
    static std::once_flag init_image_magic;
    std::call_once(init_image_magic, []() { Magick::InitializeMagick(nullptr); });
    try
    {
      m_image = std::make_shared<Magick::Image>(path);
    }
    catch (...)
    {
      std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
      std::terminate();
    }

    m_image->type(MagickCore::ImageType::TrueColorAlphaType);
    m_view = std::make_shared<Magick::Pixels>(*m_image);
    m_pixels = m_view->get(0, 0, Width(), Height());
  }

  MagicImage(int w, int h)
  {
    auto c = Magick::Color(0, 0, 0, 0);
    auto size = Magick::Geometry(w, h);
    m_image = std::make_shared<Magick::Image>(size, c);
    m_image->type(MagickCore::ImageType::TrueColorAlphaType);
    m_view = std::make_shared<Magick::Pixels>(*m_image);
    m_pixels = m_view->get(0, 0, Width(), Height());
  }

  MagicImage(const MagicImage& other)
  {
    m_image = std::make_shared<Magick::Image>(*other.m_image);
    m_image->type(MagickCore::ImageType::TrueColorAlphaType);
    m_view = std::make_shared<Magick::Pixels>(*m_image);
    m_pixels = m_view->get(0, 0, Width(), Height());
  }

  ~MagicImage() override
  {
    m_view->sync();
  }

  [[nodiscard]]
  int Width() const override
  {
    return static_cast<int>(m_image->size().width());
  }

  [[nodiscard]]
  int Height() const override
  {
    return static_cast<int>(m_image->size().height());
  }

  [[nodiscard]]
  int Channels() const override
  {
    return static_cast<int>(m_image->channels());
  }

  void SaveAsJPEG(const std::string& path) const override
  {
    m_image->write(path);
  }

  void SaveAsPNG(const std::string& path) const override
  {
    m_image->quality(100);
    m_image->write("png32:" + path);
  }

  void Crop(int left, int top, int right, int bottom) override
  {
    auto geom = Magick::Geometry(right - left, bottom - top, left, top);
    m_image->crop(geom);
    m_view = std::make_shared<Magick::Pixels>(*m_image);
    m_pixels = m_view->get(0, 0, Width(), Height());
  }

  void Scale(double scale) override
  {
    int w = std::max(1, static_cast<int>(round(Width() * scale)));
    int h = std::max(1, static_cast<int>(round(Height() * scale)));
    m_image->resize(Magick::Geometry(w, h));
    m_view = std::make_shared<Magick::Pixels>(*m_image);
    m_pixels = m_view->get(0, 0, Width(), Height());
  }

  Color GetColor(int x, int y) const override
  {
    auto  w = Width();
    auto  channels = Channels();
    int   offset = channels * (w * y + x);
    Color color;
    color.r = m_pixels[offset + 0];
    color.g = m_pixels[offset + 1];
    color.b = m_pixels[offset + 2];
    color.a = m_pixels[offset + 3];
  }

  void SetColor(int x, int y, Color value) override
  {
    auto w = Width();
    auto channels = Channels();
    int  offset = channels * (w * y + x);
    m_pixels[offset + 0] = value.r;
    m_pixels[offset + 1] = value.g;
    m_pixels[offset + 2] = value.b;
    m_pixels[offset + 3] = value.a;
  }

  void Composite(const CAbstractImage& src, int xOffset, int yOffset) override
  {
    const auto& magic_src = dynamic_cast<const MagicImage&>(src);
    m_image->composite(
        *magic_src.m_image, xOffset, yOffset, Magick::CompositeOperator::BlendCompositeOp);
    m_view = std::make_shared<Magick::Pixels>(*m_image);
    m_pixels = m_view->get(0, 0, Width(), Height());
  }

  void ConvertToRGBA() override
  {
    if (Channels() == 4)
    {
      return;
    }
    assert(false); // TODO
    auto new_image = MagicImage(Width(), Height());
    new_image.Composite(*this, 0, 0);
    std::swap(*this, new_image);
  }

  [[nodiscard]]
  std::unique_ptr<CAbstractImage> Clone() const noexcept override
  {
    return std::make_unique<MagicImage>(*this);
  }

private:
  std::shared_ptr<Magick::Image>  m_image;
  std::shared_ptr<Magick::Pixels> m_view;
  Magick::Quantum*                m_pixels{nullptr};
};
} // namespace TexturePacker
