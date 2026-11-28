#include "Scene.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

using namespace std;

namespace
{
    const float gridSize = 14;
    const int segments = 100;

    const Vector3 ends[] = {{gridSize + 1, 0, 0}, {0, gridSize + 1, 0}, {0, 0, gridSize + 1}};
    const Color colors[] = {RED, GREEN, BLUE};
    const char* const names[] = {"X", "Y", "Z"};
}

Scene::~Scene()
{
    if (view_.id)
        UnloadRenderTexture(view_);
}

void Scene::Remove(int index)
{
    if (index >= 0 && index < int(pos_.size()))
        pos_.erase(pos_.begin() + index);
    drag_ = -1;
}

void Scene::MoveCamera()
{
    Vector3 forward = Vector3Subtract(cam_.target, cam_.position);
    forward.z = 0;
    forward = Vector3Normalize(forward);
    Vector3 right = Vector3CrossProduct(forward, cam_.up);
    Vector3 move = {};
    if (IsKeyDown(KEY_W)) move = Vector3Add(move, forward);
    if (IsKeyDown(KEY_S)) move = Vector3Subtract(move, forward);
    if (IsKeyDown(KEY_D)) move = Vector3Add(move, right);
    if (IsKeyDown(KEY_A)) move = Vector3Subtract(move, right);
    move = Vector3Scale(Vector3Normalize(move), 6 * GetFrameTime());
    cam_.position = Vector3Add(cam_.position, move);
    cam_.target = Vector3Add(cam_.target, move);

    bool rotate = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    float wheel = GetMouseWheelMove();
    if (!rotate && wheel == 0)
        return;

    Vector3 offset = Vector3Subtract(cam_.position, cam_.target);
    float dist = Vector3Length(offset);
    float yaw = atan2(offset.y, offset.x);
    float pitch = asin(clamp(offset.z / dist, -1.0f, 1.0f));

    if (rotate)
    {
        Vector2 mouse = GetMouseDelta();
        yaw -= mouse.x * 0.005f;
        pitch = clamp(pitch + mouse.y * 0.005f, -1.4f, 1.4f);
    }

    dist = clamp(dist * pow(0.9f, wheel), 2.0f, 100.0f);
    offset = {dist * cos(pitch) * cos(yaw),
              dist * cos(pitch) * sin(yaw), dist * sin(pitch)};
    cam_.position = Vector3Add(cam_.target, offset);
}

void Scene::Grid()
{
    Color gray = {45, 45, 45, 255};
    for (float i = 1; i <= gridSize; ++i)
    {
        DrawLine3D({i, 0, 0}, {i, gridSize, 0}, gray);
        DrawLine3D({0, i, 0}, {gridSize, i, 0}, gray);
        DrawLine3D({i, 0, 0}, {i, 0, gridSize}, gray);
        DrawLine3D({0, 0, i}, {gridSize, 0, i}, gray);
        DrawLine3D({0, i, 0}, {0, i, gridSize}, gray);
        DrawLine3D({0, 0, i}, {0, gridSize, i}, gray);
    }
    for (int i = 0; i < 3; ++i)
        DrawLine3D({}, ends[i], colors[i]);
}

Vector3 Scene::Point(const Curve& curve, int i, Vector3 pos)
{
    double t = 2 * Curve::kPi * i / segments;
    Point3D p = curve.GetPoint(t);
    return Vector3Add({float(p.x), float(p.y), float(p.z)}, pos);
}

void Scene::MoveCurve()
{
    // Движение мыши вправо и вверх в плоскости экрана.
    Vector3 forward = Vector3Normalize(Vector3Subtract(cam_.target, cam_.position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, cam_.up));
    Vector3 up = Vector3CrossProduct(right, forward);
    Vector2 mouse = GetMouseDelta();

    // Чем дальше камера, тем больше перемещение на один пиксель.
    float depth = Vector3DotProduct(Vector3Subtract(pos_[drag_], cam_.position), forward);
    float scale = max(depth, 1.0f) * 2 * tan(cam_.fovy * float(Curve::kPi) / 360) / view_.texture.height;
    Vector3 move = Vector3Add(Vector3Scale(right, mouse.x), Vector3Scale(up, -mouse.y));
    pos_[drag_] = Vector3Add(pos_[drag_], Vector3Scale(move, scale));
}

void Scene::Draw(Rectangle rect, const CurveManager& curves, int selected)
{
    int w = int(rect.width), h = int(rect.height);
    if (w <= 0 || h <= 0)
        return;
    if (!view_.id || view_.texture.width != w || view_.texture.height != h)
    {
        if (view_.id) UnloadRenderTexture(view_);
        view_ = LoadRenderTexture(w, h);
    }
    if (!view_.id)
    {
        DrawRectangleRec(rect, BLACK);
        return;
    }

    pos_.resize(curves.Size(), {gridSize / 2, gridSize / 2, 1});
    bool inside = CheckCollisionPointRec(GetMousePosition(), rect);
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT) || drag_ != selected)
        drag_ = -1;

    if (inside && drag_ < 0)
    {
        MoveCamera();
        if (selected >= 0 && selected < int(pos_.size()) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            drag_ = selected;
    }
    else if (drag_ >= 0)
        MoveCurve();

    BeginTextureMode(view_);
    ClearBackground(BLACK);
    BeginMode3D(cam_);
    Grid();
    for (int i = 0; i < int(curves.Size()); ++i)
    {
        const Curve& curve = *curves.At(i);
        int n = curve.GetType() == CurveType::Helix ? 3 * segments : segments;
        Color color = i == selected ? YELLOW : WHITE;
        Vector3 a = Point(curve, 0, pos_[i]);
        for (int j = 1; j <= n; ++j)
        {
            Vector3 b = Point(curve, j, pos_[i]);
            DrawLine3D(a, b, color);
            a = b;
        }
    }
    EndMode3D();
    Vector3 forward = Vector3Subtract(cam_.target, cam_.position);
    for (int i = 0; i < 3; ++i)
    {
        if (Vector3DotProduct(Vector3Subtract(ends[i], cam_.position), forward) <= 0)
            continue;
        Vector2 p = GetWorldToScreenEx(ends[i], cam_, w, h);
        if (p.x >= 0 && p.x < w - 20 && p.y >= 0 && p.y < h - 20)
            DrawText(names[i], int(p.x), int(p.y), 20, colors[i]);
    }
    EndTextureMode();
    DrawTexturePro(view_.texture, {0, 0, float(w), -float(h)}, rect, {0, 0}, 0, WHITE);
}
