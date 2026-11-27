#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <numeric>
#include <random>
#include <vector>
#include "Circle.h"
#include "Curve.h"
#include "Ellipse.h"
#include "Helix.h"

class CurveManager
{
public:
    std::size_t Size() const
    {
        return curves_.size();
    }

    bool Empty() const
    {
        return curves_.empty();
    }

    const Curve* At(std::size_t index) const
    {
        if (index >= curves_.size())
            return nullptr;
        return curves_[index].get();
    }

    Curve* At(std::size_t index)
    {
        if (index >= curves_.size())
            return nullptr;
        return curves_[index].get();
    }

    std::size_t AddCircle(double radius = 1.0)
    {
        curves_.push_back(std::make_unique<Circle>(radius));
        return curves_.size() - 1;
    }

    std::size_t AddEllipse(double radiusX = 1.0, double radiusY = 1.0)
    {
        curves_.push_back(std::make_unique<Ellipse>(radiusX, radiusY));
        return curves_.size() - 1;
    }

    std::size_t AddHelix(double radius = 1.0, double step = 1.0)
    {
        curves_.push_back(std::make_unique<Helix>(radius, step));
        return curves_.size() - 1;
    }

    bool RemoveAt(std::size_t index)
    {
        if (index >= curves_.size())
            return false;
        curves_.erase(curves_.begin() + static_cast<std::ptrdiff_t>(index));
        return true;
    }

    void GenerateRandom(std::size_t count = 5)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> typeDist(0, 2);
        std::uniform_real_distribution<double> radiusDist(0.5, 6.0);
        std::uniform_real_distribution<double> stepDist(0.5, 4.0);

        for (std::size_t i = 0; i < count; ++i)
        {
            const int rawType = typeDist(rng);
            if (rawType == 0)
                AddCircle(radiusDist(rng));
            else if (rawType == 1)
                AddEllipse(radiusDist(rng), radiusDist(rng));
            else
                AddHelix(radiusDist(rng), stepDist(rng));
        }
    }

    // Отдельный контейнер только для окружностей, отсортированный по радиусу
    std::vector<const Circle*> GetSortedCircles() const
    {
        std::vector<const Circle*> circles = GetCircles();

        std::sort(circles.begin(), circles.end(),
            [](const Circle* a, const Circle* b) {
                return a->GetRadius() < b->GetRadius();
            });

        return circles;
    }

    // Сумма радиусов всех окружностей
    double GetCirclesRadiusSum() const
    {
        const std::vector<const Circle*> circles = GetCircles();
        return std::accumulate(circles.begin(), circles.end(), 0.0,
            [](double sum, const Circle* circle) {
                return sum + circle->GetRadius();
            });
    }

private:
    std::vector<const Circle*> GetCircles() const
    {
        std::vector<const Circle*> circles;
        circles.reserve(curves_.size());

        for (const auto& curvePtr : curves_)
        {
            if (curvePtr && curvePtr->GetType() == CurveType::Circle)
            {
                circles.push_back(static_cast<const Circle*>(curvePtr.get()));
            }
        }

        return circles;
    }

    std::vector<std::unique_ptr<Curve>> curves_;
};
