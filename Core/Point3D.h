#pragma once

struct Point3D
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Point3D() = default;
    Point3D(double xVal, double yVal, double zVal)
        : x(xVal), y(yVal), z(zVal)
    {
    }
};