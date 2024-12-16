#pragma once
#include <Windows.h>
#include <string>
#include <memory.h>
#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
using namespace std;

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

    // Store raw input data
    LONG rawMouseDeltaX = 0;
    LONG rawMouseDeltaY = 0;
    bool useRawInput = false;

    void init(int window_width, int window_height, const string window_name, int window_x = 0, int window_y = 0);
    void updateMouse(int x, int y);
    void processMessages();

    // Methods to handle cursor and mouse capture
    void toggleMouseCapture(bool& mouseCaptured, bool& firstMouseInput);
    void centerCursor();
    void setCursorVisibility(bool visible);

    void registerRawInput();
    //Solve WM_INPUT message in WndProc
    void handleRawInput(LPARAM lParam);
};
