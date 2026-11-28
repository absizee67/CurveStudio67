#pragma once

#include <cmath>
#include "Curve.h"

using namespace std;

class Ellipse : public Curve
{
public:
    Ellipse(double radiusX = 1.0, double radiusY = 1.0)
        : radiusX_(SanitizePositive(radiusX, 1.0))
        , radiusY_(SanitizePositive(radiusY, 1.0))
    {
    }

    CurveType GetType() const override
    {
        return CurveType::Ellipse;
    }

    string GetDefaultName() const override
    {
        return "Эллипс";
    }

    double GetRadiusX() const
    {
        return radiusX_;
    }

    double GetRadiusY() const
    {
        return radiusY_;
    }

    void SetRadiusX(double radiusX)
    {
        radiusX_ = SanitizePositive(radiusX, radiusX_);
    }

    void SetRadiusY(double radiusY)
    {
        radiusY_ = SanitizePositive(radiusY, radiusY_);
    }

    Point3D GetPoint(double t) const override
    {
        return Point3D(
            radiusX_ * cos(t),
            radiusY_ * sin(t),
            0.0
        );
    }

    Point3D GetDerivative(double t) const override
    {
        return Point3D(
            -radiusX_ * sin(t),
            radiusY_ * cos(t),
            0.0
        );
    }

private:
    double radiusX_ = 1.0;
    double radiusY_ = 1.0;
};
