#pragma once

#include "raylib.h"
#include <vector>
#include "../Core/CurveManager.h"

class Scene
{
public:
    ~Scene();
    void Draw(Rectangle rect, const CurveManager& curves, int selected);
    void Remove(int index);

private:
    Camera3D cam_ = {{21, -17, 17}, {5, 5, 5}, {0, 0, 1}, 45, CAMERA_PERSPECTIVE};
    RenderTexture2D view_ = {};
    std::vector<Vector3> pos_;
    int drag_ = -1;

    void MoveCamera();
    void Grid();
    Vector3 Point(const Curve& curve, int i, Vector3 pos);
    void MoveCurve();
};
