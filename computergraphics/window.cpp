#include "window.h"

// Window* window

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Retrieve the Window* associated with the HWND
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_CREATE:
    {
        // Store the Window* in GWLP_USERDATA
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
        return 0;
    }
    case WM_DESTROY:
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        exit(0);
        return 0;
    }
    case WM_KEYDOWN:
    {
        if (window && wParam < 256) {
            window->keys[(unsigned int)wParam] = true;
        }
        return 0;
    }
    case WM_KEYUP:
    {
        if (window && wParam < 256) {
            window->keys[(unsigned int)wParam] = false;
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if (window) {
            window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
            window->mouseButtons[0] = true;
        }
        return 0;
    }
    case WM_LBUTTONUP:
    {
        if (window) {
            window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
            window->mouseButtons[0] = false;
        }
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (window) {
            window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
        }
        return 0;
    }
    default:
    {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    }
}

void Window::init(int window_width, int window_height, const std::string window_name, int window_x, int window_y) {
    WNDCLASSEX wc = {};
    hinstance = GetModuleHandle(NULL);
    name = window_name;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    std::wstring wname = std::wstring(name.begin(), name.end());
    wc.lpszClassName = wname.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);

    width = window_width;
    height = window_height;

    style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        wname.c_str(),
        wname.c_str(),
        style,
        window_x,
        window_y,
        width,
        height,
        NULL,
        NULL,
        hinstance,
        this // Pass 'this' here
    );

    memset(keys, 0, sizeof(keys));
    memset(mouseButtons, 0, sizeof(mouseButtons));
    mousex = 0;
    mousey = 0;
}

void Window::updateMouse(int x, int y) {
    mousex = x;
    mousey = y;
}

void Window::processMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::toggleMouseCapture(bool& mouseCaptured, bool& firstMouseInput) {
    mouseCaptured = !mouseCaptured;

    if (mouseCaptured) {
        setCursorVisibility(false);
        centerCursor();
        firstMouseInput = true;
    }
    else {
        setCursorVisibility(true);
    }
}

void Window::centerCursor() {
    POINT centerPos = { width / 2, height / 2 };
    ClientToScreen(hwnd, &centerPos);
    SetCursorPos(centerPos.x, centerPos.y);
}

void Window::setCursorVisibility(bool visible) {
    ShowCursor(visible);
}
