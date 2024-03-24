#pragma once

#include <vector>
#include <utility>

namespace TexturePacker
{
struct Size
{
    int w{};
    int h{};
};

struct CRect
{
    /*
    (x, y)
    (left, top)
            +----+
            |    |
            +----+
                (x + width, y + height)
                (right, bottom)
    */

    int x, y, width, height;

    [[nodiscard]]
    int get_left() const
    {
        return x;
    }

    void set_left(int l)
    {
        x = l;
    }

    [[nodiscard]]
    int get_right() const
    {
        return x + width;
    }

    void set_right(int r)
    {
        x = r - width;
    }

    [[nodiscard]]
    int get_top() const
    {
        return y;
    }

    void set_top(int t)
    {
        y = t;
    }

    [[nodiscard]]
    int get_bottom() const
    {
        return y + height;
    }

    void set_bottom(int b)
    {
        height = b - y;
    }

    void enlarge_left_to(int l)
    {
        width = get_right() - l;
        x = l;
    }

    void enlarge_right_to(int r)
    {
        width = r - get_left();
    }

    void enlarge_top_to(int t)
    {
        height = get_bottom() - t;
        y = t;
    }

    void enlarge_bottom_to(int b)
    {
        height = b - get_top();
    }

    [[nodiscard]]
    int get_area() const
    {
        return width * height;
    }

    void rotate()
    {
        std::swap(width, height);
    }

    [[nodiscard]]
    bool is_overlapped(CRect rect) const
    {
        return !(x >= rect.x + rect.width || y >= rect.y + rect.height || x + width <= rect.x ||
                 y + height <= rect.y);
    }

    [[nodiscard]]
    bool contains(CRect rect) const
    {
        return x <= rect.x && y <= rect.y && x + width >= rect.x + rect.width &&
               y + height >= rect.y + rect.height;
    }

    [[nodiscard]]
    bool same(CRect rect) const
    {
        return x == rect.x && y == rect.y && width == rect.width && height == rect.height;
    }

    [[nodiscard]]
    std::vector<CRect> cut(const CRect& rect) const
    {
        std::vector<CRect> rects;
        if (is_overlapped(rect))
        {
            CRect tmp_rect{};

            if (get_left() < rect.get_left())
            {
                tmp_rect = *this;
                tmp_rect.enlarge_right_to(rect.get_left());
                if (tmp_rect.get_area() > 0)
                {
                    rects.emplace_back(tmp_rect);
                }
            }
            if (get_top() < rect.get_top())
            {
                tmp_rect = *this;
                tmp_rect.enlarge_bottom_to(rect.get_top());
                if (tmp_rect.get_area() > 0)
                {
                    rects.emplace_back(tmp_rect);
                }
            }
            if (get_right() > rect.get_right())
            {
                tmp_rect = *this;
                tmp_rect.enlarge_left_to(rect.get_right());
                if (tmp_rect.get_area() > 0)
                {
                    rects.emplace_back(tmp_rect);
                }
            }
            if (get_bottom() > rect.get_bottom())
            {
                tmp_rect = *this;
                tmp_rect.enlarge_top_to(rect.get_bottom());
                if (tmp_rect.get_area() > 0)
                {
                    rects.emplace_back(tmp_rect);
                }
            }
        }
        else
        {
            rects.emplace_back(*this);
        }

        return rects;
    }
};

}; // namespace TexturePacker
