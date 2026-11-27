#pragma once

#include "Scene.h"
#include <string>

class MainWindow
{
public:
    explicit MainWindow(Font font) : font_(font) {}
    void Draw();

private:
    Font font_;
    CurveManager curves_;
    Scene scene_;
    int selected_ = -1;
    int listScroll_ = 0;
    bool scrolling_ = false;
    float grab_ = 0;
    int edit_ = -1;
    bool fresh_ = false;
    std::string input_;

    void Text(const char* str, float x, float y);
    bool Button(const char* str, Rectangle rect, bool selected = false);
    double Input(const char* str, double value, int y);
    void Scroll(int count);
    void DrawList();
    void DrawParams();
};
