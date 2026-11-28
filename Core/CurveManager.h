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

using namespace std;

class CurveManager
{
public:
    size_t Size() const
    {
        return curves_.size();
    }

    bool Empty() const
    {
        return curves_.empty();
    }

    const Curve* At(size_t index) const
    {
        if (index >= curves_.size())
            return nullptr;
        return curves_[index].get();
    }

    Curve* At(size_t index)
    {
        if (index >= curves_.size())
            return nullptr;
        return curves_[index].get();
    }

    size_t AddCircle(double radius = 1.0)
    {
        curves_.push_back(make_unique<Circle>(radius));
        return curves_.size() - 1;
    }

    size_t AddEllipse(double radiusX = 1.0, double radiusY = 1.0)
    {
        curves_.push_back(make_unique<Ellipse>(radiusX, radiusY));
        return curves_.size() - 1;
    }

    size_t AddHelix(double radius = 1.0, double step = 1.0)
    {
        curves_.push_back(make_unique<Helix>(radius, step));
        return curves_.size() - 1;
    }

    bool RemoveAt(size_t index)
    {
        if (index >= curves_.size())
            return false;
        curves_.erase(curves_.begin() + static_cast<ptrdiff_t>(index));
        return true;
    }

    void GenerateRandom(size_t count = 5)
    {
        static mt19937 rng(random_device{}());
        uniform_int_distribution<int> typeDist(0, 2);
        uniform_real_distribution<double> radiusDist(0.5, 6.0);
        uniform_real_distribution<double> stepDist(0.5, 4.0);

        for (size_t i = 0; i < count; ++i)
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
    vector<const Circle*> GetSortedCircles() const
    {
        vector<const Circle*> circles = GetCircles();

        sort(circles.begin(), circles.end(),
            [](const Circle* a, const Circle* b) {
                return a->GetRadius() < b->GetRadius();
            });

        return circles;
    }

    // Сумма радиусов всех окружностей
    double GetCirclesRadiusSum() const
    {
        const vector<const Circle*> circles = GetCircles();
        return accumulate(circles.begin(), circles.end(), 0.0,
            [](double sum, const Circle* circle) {
                return sum + circle->GetRadius();
            });
    }

private:
    vector<const Circle*> GetCircles() const
    {
        vector<const Circle*> circles;
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

    vector<unique_ptr<Curve>> curves_;
};
