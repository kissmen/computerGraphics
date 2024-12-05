#include <Windows.h>
#include "Adapter.h"
#include "window.h"
#include "DXcore.h"
#include "Shader.h"
#include "Camera.h"
#include "geometry.h"
#include "mathLibrary.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdline, int nCmdshow) {
    Window win;
    win.init(1024, 1024, "My Window");
    DXcore dx;
    dx.init(1024, 1024, win.hwnd, false);
    Shader shader;
    shader.init("vertexShader.txt", "pixelShader.txt", dx);

    Plane plane;
    plane.init(&dx);

    Matrix44 defaultM;
    Vec4 eye(10.0f, 30.0f, 10.0f, 1.0f);
    Vec4 center(0.0f, 0.0f, 0.0f, 1.0f);
    Vec4 up(0.0f, 1.0f, 0.0f, 1.0f);
    Camera camera(eye, center, up);

    // Center mouse to window middle initially
    win.centerCursor();
    bool mouseCaptured = true;
    bool firstMouseInput = true;
    win.setCursorVisibility(false);

    while (true) {
        win.processMessages();

        if (win.keys[VK_ESCAPE]) {
            win.keys[VK_ESCAPE] = false;
            win.toggleMouseCapture(mouseCaptured, firstMouseInput);
        }

        if (mouseCaptured) {
            camera.updateFromKeyboard(win);
            camera.updateFromMouse(win, firstMouseInput);
        }

        // Update matrices
        Matrix44 viewM = camera.transform.viewMatrix;
        Matrix44 projectionM = camera.transform.projectionMatrix;
        Matrix44 vp = viewM * projectionM;

        // Render
        dx.clear();
        shader.updateConstantVS("staticMeshBuffer", "W", &defaultM);
        shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        shader.apply(&dx);
        plane.draw(dx.devicecontext);
        dx.present();
    }

    return 0;
}
