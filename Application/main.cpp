#include "raylib.h"
#include "../Interface/MainWindow.h"
#include <vector>

using namespace std;

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 750, "CurveStudio");
    if (!IsWindowReady())
        return 1;

    SetWindowMinSize(1200, 750);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    vector<int> chars;
    for (int i = 32; i <= 126; ++i)
        chars.push_back(i);
    for (int i = 0x400; i <= 0x45F; ++i)
        chars.push_back(i);

    const char* path = "C:/Windows/Fonts/arial.ttf";
    if (!FileExists(path))
        path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

    Font font = LoadFontEx(path, 24, chars.data(), static_cast<int>(chars.size()));
    if (!font.texture.id || font.texture.id == GetFontDefault().texture.id)
    {
        CloseWindow();
        return 1;
    }
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    {
        MainWindow window(font);
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground({21, 23, 28, 255});
            window.Draw();
            EndDrawing();
        }
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
