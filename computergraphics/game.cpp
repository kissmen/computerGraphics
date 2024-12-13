#include <Windows.h>
#include "Adapter.h"
#include "window.h"
#include "DXcore.h"
#include "Shader.h"
#include "Camera.h"
#include "geometry.h"
#include "texture.h"
#include "material.h"
#include "render.h"
#include "skydome.h"
#include "waterparams.h"
#include "grass.h"
#include "mathLibrary.h"
#include <chrono>
using namespace std;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdline, int nCmdshow) {
    Window win;
    win.init(1920, 1080, "My Window");
    win.useRawInput = true;
    win.registerRawInput();

    DXcore dx;
    dx.init(1920, 1080, win.hwnd, false);

    TextureManager textureManager;
    vector<std::string> textureFiles = {
        "Textures/plane.jpg",
        "Textures/bamboo branch.png",
        "Textures/acacia branch.png",
        "Textures/juniper branch.png",
        "Textures/palm07.png",
        "Textures/pine branch.png",
        "Textures/bark02.png",
        "Textures/bark09.png",
        "Textures/mossybark02.png",
        "Textures/plant02.png",
        "Textures/sky5.png",
        "Textures/sky.hdr",
        "Textures/night.png",
        "Textures/water.png",
        "Textures/grass.png",
        "Textures/grass1.png",
        "Textures/grassss.png",
        "Textures/grassnoise.png",
        "Textures/grassheight.png",
    };
    textureManager.init(dx, textureFiles);

    Shader shader;
    shader.init("vertexShader.txt", "pixelShader.txt", dx);
    shader.updateSamplerPS(&dx, "samp", textureManager.getSamplerState());
    shader.apply(&dx);

    Plane plane;
    plane.init(&dx, textureManager, "Textures/plane.jpg");
    Tree oak;
    oak.init(&dx, textureManager, "Models/juniper.gem");

    SkyDome sky;
    sky.init(dx, textureManager, "Textures/sky5.png");

    WaterParams waterParams;
    waterParams.init(dx);
    Shader waterShader;
    waterShader.init("waterVertexShader.txt", "waterPixelShader.txt", dx);
    waterParams.bindToShader(dx);
    waterParams.waterBTS(dx, waterShader, textureManager, "Textures/water.png");
    Mesh waterMesh = WaterParams::createHighResPlane(dx, 10000.0f, 10000.0f, 35, 35);

    Grass grass;
    grass.init(dx, 50000, 400.0f);

    Shader grassShader;
    grassShader.init("grassVertexShader.txt", "grassPixelShader.txt", dx);
    grass.bindToShader(dx);
    grass.grassBTS(dx, grassShader, textureManager, "Textures/grassss.png", "Textures/grassnoise.png", "Textures/grassheight.png");

    Matrix44 defaultM;
    Vec4 eye(10.0f, 40.0f, 10.0f, 1.0f);
    Vec4 center(10.0f, 0.0f, 10.0f, 1.0f);
    Vec4 up(0.0f, 1.0f, 0.0f, 1.0f);
    Camera camera(eye, center, up, (float)win.width, (float)win.height);

    // Center mouse
    win.centerCursor();
    bool mouseCaptured = true;
    bool firstMouseInput = true;
    win.setCursorVisibility(false);

    auto lt = chrono::high_resolution_clock::now();

    while (true) {
        win.processMessages();

        auto currentTime = chrono::high_resolution_clock::now();
        chrono::duration<float> dtDuration = currentTime - lt;
        float dt = dtDuration.count();
        lt = currentTime;

        if (dt > 0.1f) {
            dt = 0.1f;
        }

        camera.updateFromKeyboard(win, dt);
        if (win.keys[VK_ESCAPE]) {
            win.keys[VK_ESCAPE] = false;
            break;
        }

        camera.updateFromRawInput((float)win.rawMouseDeltaX, (float)win.rawMouseDeltaY, dt);
        win.rawMouseDeltaX = 0;
        win.rawMouseDeltaY = 0;

        Matrix44 viewM = camera.transform.viewMatrix;
        Matrix44 projectionM = camera.transform.projectionMatrix;
        Matrix44 vp = viewM * projectionM;

        Matrix44 waterWorld;
        waterWorld.identity();
        waterWorld = Matrix44::translation(Vec3(0.0f, -20.0f, 0.0f));

        Matrix44 grassWorld;
        grassWorld.identity();

        dx.clear();

        dx.setDepthStateSky();
        sky.draw(&dx, shader, camera);
        dx.setDepthStateDefault();

        waterParams.update(dx, dt);
        waterShader.updateConstantVS("staticMeshBuffer", "W", &waterWorld);
        waterShader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        waterShader.apply(&dx);
        // waterMesh.draw(dx.devicecontext);

        grass.update(dx, dt);
        grassShader.updateConstantVS("staticMeshBuffer", "W", &grassWorld);
        grassShader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        grassShader.apply(&dx);
        grass.draw(dx);

        shader.updateConstantVS("staticMeshBuffer", "W", &defaultM);
        shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        shader.apply(&dx);
         plane.draw(&dx, shader);
        // oak.draw(&dx, shader);

        dx.present();
    }

    return 0;
}
