#pragma once

#include <texture_packer/rect.hpp>

namespace TexturePacker
{
struct CImageRect : public CRect
{
  CImageRect()
      : CRect({0, 0, 0, 0})
  {
  }

  unsigned int m_ex_key{0};
};

}; // namespace TexturePacker
