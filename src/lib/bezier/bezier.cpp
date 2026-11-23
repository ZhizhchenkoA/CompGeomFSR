#include "bezier/bezier.h"

#include <algorithm>

namespace bezier
{

    std::vector<std::vector<geometry::Point>> deCasteljauLevels(
        const std::vector<geometry::Point> &control, double t)
    {
        std::vector<std::vector<geometry::Point>> levels;
        if (control.empty())
        {
            return levels;
        }

        t = std::clamp(t, 0.0, 1.0);
        levels.push_back(control);

        while (levels.back().size() > 1)
        {
            const auto &previous = levels.back();
            std::vector<geometry::Point> next;
            next.reserve(previous.size() - 1);
            for (std::size_t i = 1; i < previous.size(); ++i)
            {
                next.push_back({
                    (1.0 - t) * previous[i - 1].x + t * previous[i].x,
                    (1.0 - t) * previous[i - 1].y + t * previous[i].y,
                });
            }
            levels.push_back(std::move(next));
        }

        return levels;
    }

    geometry::Point deCasteljau(const std::vector<geometry::Point> &control, double t)
    {
        const std::vector<std::vector<geometry::Point>> levels = deCasteljauLevels(control, t);
        return levels.empty() ? geometry::Point{} : levels.back().front();
    }

    std::vector<geometry::Point> sample(const std::vector<geometry::Point> &control, int count)
    {
        std::vector<geometry::Point> result;
        if (control.size() < 2 || count < 1)
        {
            return result;
        }

        result.reserve(static_cast<std::size_t>(count) + 1);
        for (int i = 0; i <= count; ++i)
        {
            result.push_back(deCasteljau(control, static_cast<double>(i) / count));
        }
        return result;
    }

    std::vector<geometry::Point> composite(const std::vector<geometry::Point> &control,
                                           int degree, int countPerSegment)
    {
        std::vector<geometry::Point> result;
        if (degree < 1 || countPerSegment < 1 ||
            control.size() < static_cast<std::size_t>(degree + 1))
        {
            return result;
        }

        for (std::size_t start = 0;
             start + static_cast<std::size_t>(degree) < control.size();
             start += static_cast<std::size_t>(degree))
        {
            const auto first = control.begin() + static_cast<std::ptrdiff_t>(start);
            const auto last = control.begin() + static_cast<std::ptrdiff_t>(start + degree + 1);
            std::vector<geometry::Point> segment(first, last);
            auto samples = sample(segment, countPerSegment);
            if (!result.empty() && !samples.empty())
            {
                samples.erase(samples.begin());
            }
            result.insert(result.end(), samples.begin(), samples.end());
        }

        return result;
    }

} // namespace bezier
