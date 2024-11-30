#include <Windows.h>
#include "window.h"
#include "DXcore.h"
#include "Shader.h"
#include "Camera.h"
#include "geometry.h"
#include "mathLibrary.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdline, int nCmdshow) {
    Window win;
    DXcore dx;
    Shader shader;
    Triangle triangle;

    win.init(1024, 1024, "My Window");
    dx.init(1024, 1024, win.hwnd, false);
    triangle.init(dx.device);
    triangle.createBuffer(dx.device);
    shader.init("vertexShader.txt", "pixelShader.txt", dx);



        while (true)
        {
            
            dx.clear();
            win.processMessages();
           // triangle.draw(dx.devicecontext);
            shader.apply(&dx);
            dx.present();
        }
        return 0;

}