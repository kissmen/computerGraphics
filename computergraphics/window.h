#pragma once
#include <Windows.h>
#include <string>
#include <memory.h>
#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

class Window {
public:
    HWND hwnd;
    HINSTANCE hinstance;
    float zoom;
    std::string name;
    int width;
    int height;
    DWORD style;
    bool keys[256];
    int mousex;
    int mousey;
    int window_x;
    int window_y;
    bool mouseButtons[3];

    void init(int window_width, int window_height, const std::string window_name, int window_x = 0, int window_y = 0);
    void updateMouse(int x, int y);
    void processMessages();

    // Methods to handle cursor and mouse capture
    void toggleMouseCapture(bool& mouseCaptured, bool& firstMouseInput);
    void centerCursor();
    void setCursorVisibility(bool visible);
};
