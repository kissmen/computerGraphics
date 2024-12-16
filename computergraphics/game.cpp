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
#include "GBuffer.h"
#include "DeferredRenderer.h"
#include "animationObj.h"
#include "mathLibrary.h"
#include <chrono>
using namespace std;

class Getfps
{
public:
    float FPS;
    float Deltatime = 0.f;
    Getfps() {}
    void print(float dt)
    {
        Deltatime += dt;
        FPS = 1.0f / dt;
        if (Deltatime >= 1.0f)
        {
            cout << "FPS is " << FPS << endl;
            Deltatime = 0.f;
        }
    }
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdline, int nCmdshow) {
    Window win;
    win.init(1920, 1080, "My Window");
    win.useRawInput = true;
    win.registerRawInput();

    Getfps getfps;


    DXcore dx;
    dx.init(1920, 1080, win.hwnd, false);

    TextureManager textureManager;
    vector<std::string> textureFiles = {
        "Textures/plane.jpg",
        "Textures/oak leaf.png",
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
        "Textures/T-rex_Base_Color.png",
        "Textures/T-rex_Normal_OpenGL.png",
    };
    textureManager.init(dx, textureFiles);

    // For Deferred Shading init
    /*DeferredRenderer deferred;
    deferred.init(dx, win.width, win.height, textureManager);*/

    Shader shader;
    shader.init("vertexShader.txt", "pixelShader.txt", dx);
    shader.updateSamplerPS(&dx, "samp", textureManager.getSamplerState());
    shader.apply(&dx);

    Plane plane;
    plane.init(&dx, textureManager, "Textures/plane.jpg");
    Tree oak;
    oak.init(&dx, textureManager, "Models/oak.gem");
    Dinosaur dino(1000.0f, 1000.0f);
    dino.position = Vec3(10000.0f, -500.0f, 10000.0f);
    dino.loadMesh(&dx, "Models/TRex.gem", &textureManager);
    Shader animShader;
    animShader.initAnimated("aniVS.txt", "aniPS.txt", dx);
    animShader.updateSamplerPS(&dx, "samp", textureManager.getSamplerState());
    animShader.apply(&dx);
    SkyDome sky;
    sky.init(dx, textureManager, "Textures/sky5.png");

    WaterParams waterParams;
    waterParams.init(dx);
    Shader waterShader;
    waterShader.init("waterVertexShader.txt", "waterPixelShader.txt", dx);
    waterParams.bindToShader(dx);
    waterParams.waterBTS(dx, waterShader, textureManager, "Textures/water.png");
    Mesh waterMesh = WaterParams::createHighResPlane(dx, 40000.0f, 40000.0f, 50, 50);

    Grass grass;
    grass.init(dx, 30000, 600.0f);

    Shader grassShader;
    grassShader.init("grassVertexShader.txt", "grassPixelShader.txt", dx);
    grass.bindToShader(dx);
    grass.grassBTS(dx, grassShader, textureManager, "Textures/grassss.png", "Textures/grassnoise.png", "Textures/grassheight.png");

    Matrix44 defaultM;
    Vec4 eye(10.0f, 180.0f, 200.0f, 1.0f);
    Vec4 center(10.0f, 0.0f, 10.0f, 1.0f);
    Vec4 up(0.0f, 1.0f, 0.0f, 1.0f);
    Camera camera(eye, center, up, (float)win.width, (float)win.height);
    Vec3 cameraPos(camera.eye.x, camera.eye.y, camera.eye.z);
    dino.generateRandomTarget(cameraPos); 
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

        

        dx.clear();

        sky.rotateAngle += dt * 0.01f;
        dx.setDepthStateSky();
        sky.draw(&dx, shader, camera);
        dx.setDepthStateDefault();

        Matrix44 waterWorld;
        waterWorld.identity();
        waterWorld = Matrix44::translation(Vec3(0.0f, 0.0f, 0.0f));
        waterParams.update(dx, dt);
        waterShader.updateConstantVS("staticMeshBuffer", "W", &waterWorld);
        waterShader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        waterShader.apply(&dx);
        waterMesh.draw(dx.devicecontext);

        Matrix44 grassWorld;
        grassWorld.identity();
        grassWorld = Matrix44::translation(Vec3(0.0f, 100.0f, 0.0f));
        grass.update(dx, dt);
        grassShader.updateConstantVS("staticMeshBuffer", "W", &grassWorld);
        grassShader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        grassShader.apply(&dx);
        grass.draw(dx);

       
        shader.updateConstantVS("staticMeshBuffer", "W", &defaultM);
        shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        shader.apply(&dx);
        Matrix44 planeWorld;
        planeWorld.identity();
        planeWorld = Matrix44::scaling(Vec3(1.0f, 1.0f, 1.0f)) * Matrix44::translation(Vec3(0.0f, 100.0f, 0.0f));

        shader.updateConstantVS("staticMeshBuffer", "W", &planeWorld);
        shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        shader.apply(&dx);

        plane.draw(&dx, shader);
        Matrix44 treeWorld;
        treeWorld.identity();
        
        treeWorld = Matrix44::scaling(Vec3(0.7f, 0.7f, 0.7f)) * Matrix44::translation(Vec3(20.0f, 100.0f, 20.0f));

        shader.updateConstantVS("staticMeshBuffer", "W", &treeWorld);
        shader.updateConstantVS("staticMeshBuffer", "VP", &vp);
        shader.apply(&dx);

        oak.draw(&dx, shader);
      // 
        
        
        dino.draw(&animShader, &dx, dt, textureManager, vp);

        getfps.print(dt);
        dx.present();

    }

    return 0;
}

// deferred.beginGeometryPass(dx);

       // Matrix44 vp = viewM * projectionM;

        // For Deferred Shading init 
        /*deferred.geometryShaderNoInst.updateConstantVS("staticMeshBuffer", "W", &defaultM);
        deferred.geometryShaderNoInst.updateConstantVS("staticMeshBuffer", "VP", &vp);
        ID3D11ShaderResourceView* planeTexSRV = textureManager.find("Textures/plane.jpg");
        deferred.geometryShaderNoInst.updateTexturePS(&dx, "tex", planeTexSRV);
        deferred.geometryShaderNoInst.updateSamplerPS(&dx, "samp", textureManager.getSamplerState());
        deferred.geometryShaderNoInst.apply(&dx);*/


        /* dx.setDepthStateSky();
         sky.draw(&dx, camera);
         dx.setDepthStateDefault();*/
         // For Deferred Shading init
          /*waterParams.update(dx, dt);
          deferred.geometryShaderNoInst.updateConstantVS("staticMeshBuffer", "W", &waterWorld);
          deferred.geometryShaderNoInst.updateConstantVS("staticMeshBuffer", "VP", &vp);
          deferred.geometryShaderNoInst.apply(&dx);*/
          // waterMesh.draw(dx.devicecontext);

            // For Deferred Shading init 
            /*deferred.geometryShaderInst.updateConstantVS("staticMeshBuffer", "W", &grassWorld);
            deferred.geometryShaderInst.updateConstantVS("staticMeshBuffer", "VP", &vp);
            deferred.geometryShaderInst.apply(&dx);*/

            /*grass.update(dx, dt);
            grass.draw(dx);*/


            //  deferred.endGeometryPass(dx);


            //  deferred.doLightingPass(dx);
            //  dx.present();