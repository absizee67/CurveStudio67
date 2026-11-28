#include "MainWindow.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cstdio>

using namespace std;

namespace
{
    const Color panel = {30, 33, 40, 255};
    const Color button = {44, 49, 59, 255};
    const Color accent = {58, 105, 169, 255};
    const Color text = {225, 229, 236, 255};
    const Color line = {104, 114, 132, 255};
    const int rows = 3;
    const float listHeight = rows * 26 - 2;
}

void MainWindow::Text(const char* str, float x, float y)
{
    DrawTextEx(font_, str, {x, y}, 16, 0.5f, text);
}

bool MainWindow::Button(const char* str, Rectangle rect, bool selected)
{
    DrawRectangleRec(rect, selected ? accent : button);
    Vector2 size = MeasureTextEx(font_, str, 16, 0.5f);
    Text(str, rect.x + (rect.width - size.x) / 2, rect.y + (rect.height - size.y) / 2);
    return CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

double MainWindow::Input(const char* str, double value, int y)
{
    Rectangle rect = {138, float(y), 136, 24};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(GetMousePosition(), rect))
        {
            edit_ = y;
            input_ = TextFormat("%.10g", value);
            fresh_ = true;
        }
        else if (edit_ == y)
            edit_ = -1;
    }

    if (edit_ == y)
    {
        for (int c = GetCharPressed(); c > 0; c = GetCharPressed())
        {
            bool digit = c >= '0' && c <= '9';
            if (!digit && c != '.' && c != ',' && c != 'e' && c != 'E' && c != '+' && c != '-')
                continue;
            if (fresh_) input_.clear();
            fresh_ = false;
            if (input_.size() < 24) input_ += char(c == ',' ? '.' : c);
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (fresh_) input_.clear();
            else if (!input_.empty()) input_.pop_back();
            fresh_ = false;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        {
            char* end;
            double n = strtod(input_.c_str(), &end);
            bool valid = end != input_.c_str() && *end == '\0' &&
                         isfinite(n) && n >= Curve::kMinPositiveValue;
            if (valid && !fresh_ && value != n)
            {
                value = n;
                changed_ = true;
            }
            if (valid) edit_ = -1;
        }
        if (IsKeyPressed(KEY_ESCAPE)) edit_ = -1;
    }

    Text(str, 28, rect.y + 3);
    DrawRectangleRec(rect, edit_ == y ? accent : button);
    const char* number = edit_ == y ? input_.c_str() : TextFormat("%.10g", value);
    while (edit_ == y && *number && MeasureTextEx(font_, number, 16, 0.5f).x > rect.width - 12)
        ++number;
    BeginScissorMode(int(rect.x), int(rect.y), int(rect.width), int(rect.height));
    Text(number, rect.x + 5, rect.y + 3);
    EndScissorMode();
    return value;
}

void MainWindow::Scroll(Rectangle rect, int count, int visible, int& offset, int id)
{
    int limit = max(0, count - visible);
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, rect))
        offset -= int(GetMouseWheelMove());
    offset = clamp(offset, 0, limit);
    if (limit == 0)
    {
        if (scrollId_ == id) scrollId_ = 0;
        return;
    }

    Rectangle bar = {rect.x + rect.width - 10, rect.y, 10, rect.height};
    float h = max(16.0f, bar.height * visible / count);
    float range = bar.height - h;
    Rectangle thumb = {bar.x, bar.y + range * offset / limit, bar.width, h};
    if (CheckCollisionPointRec(mouse, bar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        scrollId_ = id;
        scrollGrab_ = CheckCollisionPointRec(mouse, thumb) ? mouse.y - thumb.y : h / 2;
    }
    if (scrollId_ == id)
    {
        float part = clamp((mouse.y - bar.y - scrollGrab_) / range, 0.0f, 1.0f);
        offset = int(part * limit + 0.5f);
        thumb.y = bar.y + range * offset / limit;
    }
    DrawRectangleRec(bar, button);
    DrawRectangleRec(thumb, accent);
}

void MainWindow::DrawList()
{
    Text("Кривые", 28, 68);
    int count = int(curves_.Size());
    Scroll({28, 90, 246, listHeight}, count, rows, listScroll_, 1);
    float w = count > rows ? 230 : 246;
    for (int i = listScroll_; i < count && i < listScroll_ + rows; ++i)
    {
        const char* name = TextFormat("%s #%d", curves_.At(i)->GetDefaultName().c_str(), i + 1);
        if (Button(name, {28, 90.0f + (i - listScroll_) * 26, w, 24}, selected_ == i))
            selected_ = i;
    }

    DrawLine(28, 174, 274, 174, line);
    Text("Добавить", 28, 182);
    if (Button("Окружность", {28, 202, 246, 22}))
        curves_.AddCircle(1.0);
    if (Button("Эллипс", {28, 228, 246, 22}))
        curves_.AddEllipse(2.0, 1.0);
    if (Button("Спираль", {28, 254, 246, 22}))
        curves_.AddHelix(1.0, 1.0);
    if (Button("Случайные", {28, 280, 246, 22}))
        curves_.GenerateRandom(5);
    if (int(curves_.Size()) != count)
    {
        changed_ = true;
        selected_ = int(curves_.Size()) - 1;
        listScroll_ = max(0, selected_ - rows + 1);
    }
}

void MainWindow::DrawParams()
{
    Text("Параметры", 28, 322);
    Curve* curve = selected_ >= 0 ? curves_.At(selected_) : nullptr;
    if (!curve)
    {
        Text("Кривая не выбрана", 28, 346);
        return;
    }
    Text(curve->GetDefaultName().c_str(), 28, 346);
    CurveType type = curve->GetType();
    if (type == CurveType::Circle)
    {
        auto* c = static_cast<Circle*>(curve);
        c->SetRadius(Input("Радиус", c->GetRadius(), 366));
    }
    else if (type == CurveType::Ellipse)
    {
        auto* e = static_cast<Ellipse*>(curve);
        e->SetRadiusX(Input("Радиус X", e->GetRadiusX(), 366));
        e->SetRadiusY(Input("Радиус Y", e->GetRadiusY(), 402));
    }
    else if (type == CurveType::Helix)
    {
        auto* h = static_cast<Helix*>(curve);
        h->SetRadius(Input("Радиус", h->GetRadius(), 366));
        h->SetStep(Input("Шаг", h->GetStep(), 402));
    }

    int y = type == CurveType::Circle ? 410 : 446;
    if (Button("Удалить", {28, float(y), 246, 26}))
    {
        scene_.Remove(selected_);
        curves_.RemoveAt(selected_);
        changed_ = true;
        selected_ = -1;
        edit_ = -1;
    }
}

void MainWindow::PrintResults(const vector<const Circle*>& circles, double sum)
{
    printf("\nResults at t = pi/4\n");
    for (int i = 0; i < int(curves_.Size()); ++i)
    {
        const Curve* c = curves_.At(i);
        Point3D p = c->GetPoint(Curve::kPiOverFour);
        Point3D d = c->GetDerivative(Curve::kPiOverFour);
        printf("Curve #%d: point (%.10g, %.10g, %.10g), derivative (%.10g, %.10g, %.10g)\n",
            i + 1, p.x, p.y, p.z, d.x, d.y, d.z);
    }

    printf("Sorted circle radii:\n");
    for (const Circle* c : circles)
        printf("R = %.10g\n", c->GetRadius());
    printf("Total radius: %.10g\n", sum);
}

void MainWindow::DrawResults()
{
    DrawLine(28, 484, 274, 484, line);
    Text("Расчёты при t = pi/4", 28, 492);
    auto circles = curves_.GetSortedCircles();
    double sum = curves_.GetCirclesRadiusSum();
    Text(TextFormat("Окружности: %d", int(circles.size())), 28, 514);
    Text(TextFormat("Сумма радиусов: %.6g", sum), 28, 536);

    if (changed_)
    {
        PrintResults(circles, sum);
        changed_ = false;
    }

    vector<string> lines;
    const Curve* c = selected_ >= 0 ? curves_.At(selected_) : nullptr;
    if (c)
    {
        Point3D p = c->GetPoint(Curve::kPiOverFour);
        Point3D d = c->GetDerivative(Curve::kPiOverFour);
        lines.push_back(TextFormat("%s #%d", c->GetDefaultName().c_str(), selected_ + 1));
        lines.push_back(TextFormat("Точка: (%.4g, %.4g, %.4g)", p.x, p.y, p.z));
        lines.push_back(TextFormat("Произв.: (%.4g, %.4g, %.4g)", d.x, d.y, d.z));
    }

    lines.push_back("Радиусы по возрастанию:");
    for (const Circle* circle : circles)
        lines.push_back(TextFormat("R = %.6g", circle->GetRadius()));
    if (circles.empty()) lines.push_back("Нет окружностей");

    int visible = (GetScreenHeight() - 580) / 20;
    if (visible <= 0) return;
    Rectangle rect = {28, 562, 246, float(visible * 20)};
    int count = int(lines.size());
    Scroll(rect, count, visible, resultScroll_, 2);
    float w = count > visible ? 230 : 246;
    for (int i = resultScroll_; i < count && i < resultScroll_ + visible; ++i)
    {
        const char* str = lines[i].c_str();
        float size = 14;
        while (MeasureTextEx(font_, str, size, 0.5f).x > w && size > 9) --size;
        DrawTextEx(font_, str, {28, rect.y + (i - resultScroll_) * 20}, size, 0.5f, text);
    }
}

void MainWindow::Draw()
{
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        scrollId_ = 0;

    Text("CurveStudio / Редактор 3D-кривых", 16, 20);
    DrawRectangle(16, 58, 270, GetScreenHeight() - 74, panel);
    DrawLine(28, 314, 274, 314, line);
    int before = selected_;
    DrawList();
    if (selected_ != before) edit_ = -1;
    DrawParams();
    if (selected_ != before) resultScroll_ = 0;
    DrawResults();

    scene_.Draw({298, 58, float(GetScreenWidth() - 314), float(GetScreenHeight() - 74)}, curves_, selected_);
}
