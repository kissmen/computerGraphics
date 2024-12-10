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
    "Textures/SandWithHands_basecolor.png",
    "Textures/bamboo branch.png",
    "Textures/bark09.png",
    "Textures/plant02.png",
    "Textures/sky9.png",
    "Textures/water.png",
    };
    textureManager.init(dx, textureFiles);
    Shader shader;
    shader.init("vertexShader.txt", "pixelShader.txt", dx);
    shader.updateSamplerPS(&dx, "samp", textureManager.getSamplerState());
    shader.apply(&dx);
    Plane plane;
    plane.init(&dx, textureManager, "Textures/SandWithHands_basecolor.png"); 
    Tree oak;
    oak.init(&dx, textureManager, "Models/bamboo.gem");
    SkyDome sky;
    sky.init(dx, textureManager, "Textures/sky9.png");
    WaterParams waterParams;
    waterParams.init(dx);
    Shader waterShader;
    waterShader.init("waterVertexShader.txt", "waterPixelShader.txt", dx);
    waterParams.bindToShader(dx);
    waterParams.bindTextureAndSampler(dx, waterShader, textureManager, "Textures/water.png");
    Mesh waterMesh = WaterParams::createHighResPlane(dx, 10000.0f, 10000.0f, 30, 30);
  //  Matrix44 world;
  //  world.identity();
  //  world = Matrix44::scaling(Vec3(0.2f, 0.2f, 0.2f));
    Matrix44 defaultM;
    Vec4 eye(10.0f, 300.0f, 10.0f, 1.0f);
    Vec4 center(0.0f, 0.0f, 0.0f, 1.0f);
    Vec4 up(0.0f, 1.0f, 0.0f, 1.0f);
    Camera camera(eye, center, up, (float)win.width, (float)win.height);


    // Center mouse to window middle initially
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

        // Cap deltaTime to avoid issues when the application is minimized or paused
        if (dt > 0.1f) {
            dt = 0.1f;
        }
        camera.updateFromKeyboard(win, dt);

        if (win.keys[VK_ESCAPE]) {
            win.keys[VK_ESCAPE] = false;
            win.toggleMouseCapture(mouseCaptured, firstMouseInput);
        }

        camera.updateFromRawInput((float)win.rawMouseDeltaX, (float)win.rawMouseDeltaY, dt);
        win.rawMouseDeltaX = 0;
        win.rawMouseDeltaY = 0;


        // Update matrices
        Matrix44 viewM = camera.transform.viewMatrix;
        Matrix44 projectionM = camera.transform.projectionMatrix;
        Matrix44 vp = viewM * projectionM;

        Matrix44 waterWorld;
        waterWorld.identity();
        waterWorld = Matrix44::translation(Vec3(0.0f, 20.0f, 0.0f));

        // Render
        dx.clear();

        dx.setDepthStateSky(); // Read-only depth state
        sky.draw(&dx, shader, camera);
        dx.setDepthStateDefault(); // Restore of depth state

        waterParams.update(dx, dt);
        waterShader.updateConstantVS("staticMeshBuffer", "W", &waterWorld);
        waterShader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        waterShader.apply(&dx);
        waterMesh.draw(dx.devicecontext);
        // Update WVP Matrix then draw
        shader.updateConstantVS("staticMeshBuffer", "W", &defaultM);
        shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        shader.apply(&dx);

        //plane.draw(&dx, shader);
        oak.draw(&dx, shader);

        dx.present();
    }

    return 0;
}
