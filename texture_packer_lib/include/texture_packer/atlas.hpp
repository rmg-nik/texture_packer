#pragma once

#include <texture_packer/image_rect.hpp>

#include <tuple>
#include <vector>

namespace TexturePacker
{
enum class ExpandStrategy : unsigned char
{
  ExpandBoth = 0,
  ExpandWidth = 1,
  ExpandHeight = 2,
  ExpandShortSide = 3,
  ExpandLongSide = 4,
};

enum class RankStrategy : unsigned char
{
  RankBSSF = 0,
  RankBLSF = 1,
  RankBAF = 2,
};

constexpr unsigned int MAX_RANK = -1;
constexpr int          DEFAULT_ATLAS_MAX_WIDTH = 2048;
constexpr int          DEFAULT_ATLAS_MAX_HEIGHT = 2048;

class CAtlas
{
public:
  CAtlas(int _max_width = DEFAULT_ATLAS_MAX_WIDTH, int _max_height = DEFAULT_ATLAS_MAX_HEIGHT,
         bool _force_square = false, bool _force_pot = false, int _border_padding = 0,
         int _shape_padding = 0, ExpandStrategy _expand_strategy = ExpandStrategy::ExpandShortSide,
         RankStrategy _rank_strategy = RankStrategy::RankBAF);

  [[nodiscard]]
  const std::vector<CImageRect>& GetPlacedImageRect() const;

  [[nodiscard]]
  int GetWidth() const;

  [[nodiscard]]
  int GetHeight() const;

  bool TryExpand();

  void PlaceImageRectInFreeRect(unsigned int free_rect_idx, CImageRect& image_rect);

  [[nodiscard]]
  unsigned int Rank(CRect free_rect, const CImageRect& image_rect,
                    RankStrategy rank_strategy = RankStrategy::RankBAF) const;

  [[nodiscard]]
  std::tuple<unsigned int, unsigned int, bool> FindBestRankWithoutRotate(
      const CImageRect& image_rect) const;

  [[nodiscard]]
  std::tuple<unsigned int, unsigned int, bool> FindBestRankWithRotate(
      const CImageRect& image_rect) const;

  [[nodiscard]]
  std::tuple<unsigned int, unsigned int, bool> FindBestRank(const CImageRect& image_rect,
                                                            bool enable_rotate = false) const;

  void Shrink();

private:
  [[nodiscard]]
  bool IsInMaxSize(int new_width, int new_height) const;

  void PruneFreeRects();

private:
  int                     m_width;
  int                     m_height;
  int                     m_max_width;
  int                     m_max_height;
  int                     m_border_padding;
  int                     m_shape_padding;
  bool                    m_force_square;
  bool                    m_force_pot;
  ExpandStrategy          m_expand_strategy;
  RankStrategy            m_rank_strategy;
  std::vector<CImageRect> m_image_rects;
  std::vector<CRect>      m_free_rects;
};

} // namespace TexturePacker
