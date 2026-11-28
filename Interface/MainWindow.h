#pragma once

#include "Scene.h"
#include <string>

using namespace std;

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
    int resultScroll_ = 0;
    int scrollId_ = 0;
    bool changed_ = true;
    float scrollGrab_ = 0;
    int edit_ = -1;
    bool fresh_ = false;
    string input_;

    void Text(const char* str, float x, float y);
    bool Button(const char* str, Rectangle rect, bool selected = false);
    double Input(const char* str, double value, int y);
    void Scroll(Rectangle rect, int count, int visible, int& offset, int id);
    void DrawList();
    void DrawParams();
    void DrawResults();
    void PrintResults(const vector<const Circle*>& circles, double sum);
};
