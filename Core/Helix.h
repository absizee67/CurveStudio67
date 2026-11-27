#pragma once

#include <cmath>
#include "Curve.h"

class Helix : public Curve
{
public:
    static constexpr double kTwoPi = 2.0 * Curve::kPi;

    Helix(double radius = 1.0, double step = 1.0)
        : radius_(SanitizePositive(radius, 1.0))
        , step_(SanitizePositive(step, 1.0))
    {
    }

    CurveType GetType() const override
    {
        return CurveType::Helix;
    }

    std::string GetDefaultName() const override
    {
        return "Спираль";
    }

    double GetRadius() const
    {
        return radius_;
    }

    double GetStep() const
    {
        return step_;
    }

    void SetRadius(double radius)
    {
        radius_ = SanitizePositive(radius, radius_);
    }

    void SetStep(double step)
    {
        step_ = SanitizePositive(step, step_);
    }

    Point3D GetPoint(double t) const override
    {
        const double z = (step_ / kTwoPi) * t;
        return Point3D(
            radius_ * std::cos(t),
            radius_ * std::sin(t),
            z
        );
    }

    Point3D GetDerivative(double t) const override
    {
        const double dzDt = step_ / kTwoPi;
        return Point3D(
            -radius_ * std::sin(t),
            radius_ * std::cos(t),
            dzDt
        );
    }

private:
    double radius_ = 1.0;
    double step_ = 1.0;
};