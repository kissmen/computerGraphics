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
    DXcore dx;
    Shader shader;
    Triangle triangle;
    Plane plane;
    Transform transform;

    win.init(1024, 1024, "My Window");
    dx.init(1024, 1024, win.hwnd, false);
    triangle.init(dx.device);
    triangle.createBuffer(dx.device);
    shader.init("vertexShader.txt", "pixelShader.txt", dx);
    plane.init(&dx);

    Matrix44 defaultM;

    Vec4 eye(100.0f, 80.0f, 100.0f, 1.0f);
    Vec4 center(0.0f, 0.0f, 0.0f, 1.0f);
    Vec4 up(0.0f, 1.0f, 0.0f, 1.0f);

    transform.setViewMatrix(eye, center, up);
    transform.setProjectionMatrix(60.0f, 1.0f, 0.1f, 1000.0f);

    
        while (true)
        {

            win.processMessages();

            Matrix44 viewM = transform.viewMatrix;
            Matrix44 projectionM = transform.projectionMatrix;
            Matrix44 vp = viewM * projectionM;


            dx.clear();

            shader.updateConstantVS("staticMeshBuffer", "W", &defaultM);
            shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
            shader.apply(&dx);
           // triangle.draw(dx.devicecontext);
            plane.draw(dx.devicecontext);

             dx.present();
        }
       

}