#include <texture_packer/atlas.hpp>

#include <cassert>

namespace TexturePacker
{
CAtlas::CAtlas(int _max_width, int _max_height, bool _force_square, bool _force_pot,
               int _border_padding, int _shape_padding, ExpandStrategy _expand_strategy,
               RankStrategy _rank_strategy)
    : m_width(0)
    , m_height(0)
    , m_max_width(_max_width)
    , m_max_height(_max_height)
    , m_border_padding(_border_padding)
    , m_shape_padding(_shape_padding)
    , m_force_square(_force_square)
    , m_force_pot(_force_pot)
    , m_expand_strategy(_expand_strategy)
    , m_rank_strategy(_rank_strategy)
{
    (void)m_rank_strategy; // TODO
    if (m_force_square)
    {
        m_expand_strategy = ExpandStrategy::ExpandBoth;
    }

    assert(2 * m_border_padding <= m_max_width && 2 * m_border_padding <= m_max_height);

    while (2 * m_border_padding >= m_width || 2 * m_border_padding >= m_height)
    {
        m_width += 2;
        m_height += 2;
    }

    m_free_rects.emplace_back(
        CRect({static_cast<int>(m_border_padding),
               static_cast<int>(m_border_padding),
               static_cast<int>(m_width) - 2 * static_cast<int>(m_border_padding),
               static_cast<int>(m_height) - 2 * static_cast<int>(m_border_padding)}));
}

void CAtlas::PruneFreeRects()
{
    std::vector<CRect> new_free_rects;
    for (std::size_t i = 0; i < m_free_rects.size(); ++i)
    {
        auto rect0 = m_free_rects[i];
        bool pruned = false;
        for (std::size_t j = 0; j < m_free_rects.size(); ++j)
        {
            auto rect1 = m_free_rects[j];
            if (i != j && rect1.contains(rect0))
            {
                pruned = true;
                break;
            }
        }

        if (!pruned)
        {
            new_free_rects.emplace_back(rect0);
        }
    }

    m_free_rects = new_free_rects;
}

bool CAtlas::IsInMaxSize(int new_width, int new_height) const
{
    return new_width <= m_max_width && new_height <= m_max_height;
}

void CAtlas::Shrink()
{
    int max_x = -1;
    int max_y = -1;
    for (const auto& rect : m_image_rects)
    {
        max_x = std::max(max_x, rect.get_right());
        max_y = std::max(max_y, rect.get_bottom());
    }
    m_width = max_x + m_border_padding;
    m_height = max_y + m_border_padding;
    // TODO fit free rects
    m_free_rects.clear();
}

std::tuple<unsigned int, unsigned int, bool> CAtlas::FindBestRank(const CImageRect& image_rect,
                                                                  bool enable_rotate) const
{
    if (enable_rotate)
        return FindBestRankWithRotate(image_rect);
    return FindBestRankWithoutRotate(image_rect);
}

std::tuple<unsigned int, unsigned int, bool> CAtlas::FindBestRankWithRotate(
    const CImageRect& image_rect) const
{
    unsigned int best_rank, best_rank_r;
    unsigned int best_free_rect_index, best_free_rect_index_r;
    bool         rotate, rotate_r;

    CImageRect image_rect_rotated = image_rect;
    image_rect_rotated.rotate();

    std::tie(best_rank, best_free_rect_index, rotate) = FindBestRankWithoutRotate(image_rect);
    std::tie(best_rank_r, best_free_rect_index_r, rotate_r) =
        FindBestRankWithoutRotate(image_rect_rotated);

    if (best_rank <= best_rank_r)
    {
        return std::make_tuple(best_rank, best_free_rect_index, false);
    }
    else
    {
        return std::make_tuple(best_rank_r, best_free_rect_index_r, true);
    }
}

std::tuple<unsigned int, unsigned int, bool> CAtlas::FindBestRankWithoutRotate(
    const CImageRect& image_rect) const
{
    unsigned int best_rank = MAX_RANK;
    unsigned int best_free_rect_index = -1;

    for (std::size_t index = 0; index < m_free_rects.size(); ++index)
    {
        auto r = Rank(m_free_rects[index], image_rect);
        if (r < best_rank)
        {
            best_rank = r;
            best_free_rect_index = index;
        }
    }

    return std::make_tuple(best_rank, best_free_rect_index, false);
}

unsigned int CAtlas::Rank(CRect free_rect, const CImageRect& image_rect,
                          RankStrategy rank_strategy) const
{
    unsigned int r = MAX_RANK;
    switch (rank_strategy)
    {
    case RankStrategy::RankBSSF:
        r = image_rect.width >= image_rect.height ? free_rect.width - image_rect.width
                                                  : free_rect.height - image_rect.height;
        break;
    case RankStrategy::RankBLSF:
        r = image_rect.width <= image_rect.height ? free_rect.width - image_rect.width
                                                  : free_rect.height - image_rect.height;
        break;
    case RankStrategy::RankBAF:
        r = free_rect.get_area() - image_rect.get_area();
        break;
    }

    int sp_x = free_rect.x == m_border_padding ? 0 : m_shape_padding;
    int sp_y = free_rect.y == m_border_padding ? 0 : m_shape_padding;

    if (r < 0 || free_rect.width - image_rect.width < sp_x ||
        free_rect.height - image_rect.height < sp_y)
    {
        return MAX_RANK;
    }

    return r;
}

void CAtlas::PlaceImageRectInFreeRect(unsigned int free_rect_idx, CImageRect& image_rect)
{
    auto free_rect = m_free_rects[free_rect_idx];

    int sp_x = free_rect.x == m_border_padding ? 0 : m_shape_padding;
    int sp_y = free_rect.y == m_border_padding ? 0 : m_shape_padding;

    image_rect.x = free_rect.x + sp_x;
    image_rect.y = free_rect.y + sp_y;

    CImageRect tmp_rect = image_rect;
    tmp_rect.enlarge_left_to(image_rect.get_left() - sp_x);
    tmp_rect.enlarge_top_to(image_rect.get_top() - sp_y);

    std::vector<CRect> non_overlapped_free_rects;
    std::vector<CRect> new_free_rects;

    for (auto rect : m_free_rects)
    {
        if (tmp_rect.is_overlapped(rect))
        {
            for (auto new_rect : rect.cut(static_cast<CRect>(tmp_rect)))
            {
                new_free_rects.emplace_back(new_rect);
            }
        }
        else
        {
            non_overlapped_free_rects.emplace_back(rect);
        }
    }

    m_free_rects = new_free_rects;
    PruneFreeRects();

    for (auto rect : non_overlapped_free_rects)
    {
        m_free_rects.emplace_back(rect);
    }

    m_image_rects.emplace_back(image_rect);
}

bool CAtlas::TryExpand()
{
    int new_width = m_width, new_height = m_height;
    switch (m_expand_strategy)
    {

    case ExpandStrategy::ExpandWidth:
        new_width = m_force_pot ? m_width * 2 : (m_width + 4);
        break;

    case ExpandStrategy::ExpandHeight:
        new_height = m_force_pot ? m_height * 2 : (m_height + 4);
        break;

    case ExpandStrategy::ExpandShortSide:
        if (m_width < m_height)
        {
            new_width = m_force_pot ? m_width * 2 : (m_width + 4);
        }
        else
        {
            new_height = m_force_pot ? m_height * 2 : (m_height + 4);
        }
        break;

    case ExpandStrategy::ExpandLongSide:
        if (m_width >= m_height)
        {
            new_width = m_force_pot ? m_width * 2 : (m_width + 4);
        }
        else
        {
            new_height = m_force_pot ? m_height * 2 : (m_height + 4);
        }
        break;

    default:
    case ExpandStrategy::ExpandBoth:
        new_width = m_force_pot ? m_width * 2 : (m_width + 4);
        new_height = m_force_pot ? m_height * 2 : (m_height + 4);
        break;
    }

    if (!IsInMaxSize(new_width, new_height))
    {
        return false;
    }

    int old_right = m_width - m_border_padding;
    int old_bottom = m_height - m_border_padding;
    int new_right = new_width - m_border_padding;
    int new_bottom = new_height - m_border_padding;

    for (CRect& rect : m_free_rects)
    {
        if (rect.get_right() == old_right)
        {
            rect.enlarge_right_to(new_right);
        }
        if (rect.get_bottom() == old_bottom)
        {
            rect.enlarge_bottom_to(new_bottom);
        }
    }

    if (m_width != new_width)
    {
        m_free_rects.emplace_back(
            CRect({old_right,
                   static_cast<int>(m_border_padding),
                   static_cast<int>(new_width) - static_cast<int>(m_width),
                   static_cast<int>(new_height) - 2 * static_cast<int>(m_border_padding)}));
    }

    if (m_height != new_height)
    {
        m_free_rects.emplace_back(
            CRect({static_cast<int>(m_border_padding),
                   static_cast<int>(old_bottom),
                   static_cast<int>(new_width) - 2 * static_cast<int>(m_border_padding),
                   static_cast<int>(new_height) - static_cast<int>(m_height)}));
    }

    m_width = new_width;
    m_height = new_height;

    PruneFreeRects();

    return true;
}

int CAtlas::GetHeight() const
{
    return m_height;
}

int CAtlas::GetWidth() const
{
    return m_width;
}

const std::vector<CImageRect>& CAtlas::GetPlacedImageRect() const
{
    return m_image_rects;
}
} // namespace TexturePacker
