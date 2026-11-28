#pragma once

#include <cmath>
#include <string>
#include "Point3D.h"

using namespace std;

enum class CurveType
{
    Circle,
    Ellipse,
    Helix
};

class Curve
{
public:
    static constexpr double kMinPositiveValue = 1e-4;
    static constexpr double kPi = 3.14159265358979323846;
    static constexpr double kPiOverFour = kPi / 4.0;

    virtual ~Curve() = default;

    virtual CurveType GetType() const = 0;
    virtual string GetDefaultName() const = 0;

    virtual Point3D GetPoint(double t) const = 0;
    virtual Point3D GetDerivative(double t) const = 0;

    static double SanitizePositive(double value, double fallback = 1.0)
    {
        if (isfinite(value) && value >= kMinPositiveValue)
            return value;
        if (isfinite(fallback) && fallback >= kMinPositiveValue)
            return fallback;
        return kMinPositiveValue;
    }
};
