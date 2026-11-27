#pragma once

#include <cmath>
#include "Curve.h"

class Circle : public Curve
{
public:
    explicit Circle(double radius = 1.0)
        : radius_(SanitizePositive(radius, 1.0))
    {
    }

    CurveType GetType() const override
    {
        return CurveType::Circle;
    }

    std::string GetDefaultName() const override
    {
        return "Окружность";
    }

    double GetRadius() const
    {
        return radius_;
    }

    void SetRadius(double radius)
    {
        radius_ = SanitizePositive(radius, radius_);
    }

    Point3D GetPoint(double t) const override
    {
        return Point3D(
            radius_ * std::cos(t),
            radius_ * std::sin(t),
            0.0
        );
    }

    Point3D GetDerivative(double t) const override
    {
        return Point3D(
            -radius_ * std::sin(t),
            radius_ * std::cos(t),
            0.0
        );
    }

private:
    double radius_ = 1.0;
};