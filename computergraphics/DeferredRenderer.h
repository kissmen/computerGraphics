#pragma once
#include "DXcore.h"
#include "GBuffer.h"
#include "Shader.h"
#include "texture.h"

class DeferredRenderer {
public:
    GBuffer gbuffer;
    Shader geometryShaderInst;
    Shader geometryShaderNoInst;
    Shader lightingShader;
    Shader fullscreenVS;

    void init(DXcore& dx, int width, int height, TextureManager& tm) {
        // create GBuffer
        gbuffer.init(dx, width, height);

        // Noinstance geometry pass shader
        geometryShaderNoInst.init("GeometryPassNoInstVS.txt", "GeometryPassPS.txt", dx);
        geometryShaderNoInst.updateSamplerPS(&dx, "samp", tm.getSamplerState());

        // Instance geometry pass shader
        geometryShaderInst.init("GeometryPassInstVS.txt", "GeometryPassPS.txt", dx);
        geometryShaderInst.updateSamplerPS(&dx, "samp", tm.getSamplerState());

        // fullscreen VS & lighting PS
        fullscreenVS.init("FullScreenVS.txt", "FullScreenPS.txt", dx);
        fullscreenVS.updateSamplerPS(&dx, "samp", tm.getSamplerState());
    }

    void beginGeometryPass(DXcore& dx) {
        ID3D11RenderTargetView* rtvs[3] = { gbuffer.albedoRTV, gbuffer.normalRTV, gbuffer.depthRTV };
        dx.devicecontext->OMSetRenderTargets(3, rtvs, dx.depthStencilView);

        float clearColor[4] = { 0,0,0,1 };
        for (int i = 0; i < 3; i++) {
            dx.devicecontext->ClearRenderTargetView(rtvs[i], clearColor);
        }
        dx.devicecontext->ClearDepthStencilView(dx.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }



    void endGeometryPass(DXcore& dx) {
        // lighting pass
    }

    void doLightingPass(DXcore& dx) {
        dx.devicecontext->OMSetRenderTargets(1, &dx.backbufferRenderTargetView, dx.depthStencilView);
        float clearColor[4] = { 0.1f,0.1f,0.1f,1.0f };
        dx.devicecontext->ClearRenderTargetView(dx.backbufferRenderTargetView, clearColor);

        // GBuffer SRV fullscreenVS/PS
        fullscreenVS.updateTexturePS(&dx, "gAlbedo", gbuffer.albedoSRV);
        fullscreenVS.updateTexturePS(&dx, "gNormal", gbuffer.normalSRV);
        fullscreenVS.updateTexturePS(&dx, "gDepth", gbuffer.depthSRV);

        fullscreenVS.apply(&dx);

        dx.devicecontext->IASetInputLayout(NULL);
        dx.devicecontext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
        dx.devicecontext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
        dx.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        dx.devicecontext->Draw(3, 0);
    }
};
