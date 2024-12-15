#pragma once
#include <d3d11.h>
#include <vector>
#include "DXcore.h"
#include "Shader.h"
#include "mesh.h"
#include "texture.h"
#include "mathLibrary.h"
#include <cstdlib>

struct GrassParamsCB {
    float uTime;
    float strength;
    float scaleVal;
    float gradientPower;

    float BaseColorBottom[3]; float pad1;   // 3 floats + 1 float =16 bytes
    float BaseColorTop[3];    float pad2;   // same pattern
    float BaseColorThird[3];  float pad3;

    float WindIntensity;
    float WindWeight;
    float SpecularPower;
    float SpecularIntensity;  //4 floats=16 bytes

    float LightDir[3]; float pad4;  //3 floats+1 pad =16 bytes

    float areaSize;
    float pad5[3];
};

class Grass {
public:
    struct INSTANCE_DATA {
        float x, y, z;    // Instance pos
    };

    ID3D11Buffer* m_instanceBuffer;
    Mesh m_mesh;
    int m_instanceCount;

    GrassParamsCB params;
    ID3D11Buffer* grassCB;

    Grass() : m_instanceBuffer(nullptr), m_instanceCount(0), grassCB(nullptr)
    {
        params.uTime = 0.0f;
        params.strength = 60.0f;
        params.scaleVal = 1.0f;
        params.gradientPower = 1.5f;

        params.BaseColorBottom[0] = 0.5f; params.BaseColorBottom[1] = 0.7f; params.BaseColorBottom[2] = 0.1f; // Bottom
        params.BaseColorTop[0] = 0.2f; params.BaseColorTop[1] = 0.4f; params.BaseColorTop[2] = 0.15f;       // TOP
        params.BaseColorThird[0] = 0.18f; params.BaseColorThird[1] = 0.7f; params.BaseColorThird[2] = 0.5f; // OFFSET Color


        params.WindIntensity = 7.0f;
        params.WindWeight = 0.1f;
        params.SpecularPower = 100.0f;
        params.SpecularIntensity = 1.0f;
        // 45 degree Light
        params.LightDir[0] = 0.707f;  // x
        params.LightDir[1] = 0.707f; // y
        params.LightDir[2] = 0.0f;    // z
        params.areaSize = 400.0f;
    }

    ~Grass() {
        release();
    }

    void init(DXcore& dx, int instanceCount, float areaSize) {
        m_instanceCount = instanceCount;
        params.areaSize = areaSize;

        std::vector<INSTANCE_DATA> instanceData(instanceCount);
        for (int i = 0; i < instanceCount; i++) {
            float x = ((float)rand() / RAND_MAX) * areaSize * 2 - areaSize;
            float z = ((float)rand() / RAND_MAX) * areaSize * 2 - areaSize;
            float y = 0.0f;
            instanceData[i] = { x,y,z };
        }

        // Create instance buffer
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = (UINT)(sizeof(INSTANCE_DATA) * instanceCount);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = instanceData.data();
        HRESULT hr = dx.device->CreateBuffer(&bd, &initData, &m_instanceBuffer);
        if (FAILED(hr)) {
            printf("Failed to create instance buffer.\n");
            exit(1);
        }

        // create CB
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(GrassParamsCB);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = dx.device->CreateBuffer(&cbd, NULL, &grassCB);
        if (FAILED(hr)) {
            printf("Failed to create grassCB buffer.\n");
            exit(1);
        }

        // Three planes of intersecting grass clusters
        float width = 17.0f;
        float height = 24.0f;
        float angle = 60.0f * (PI / 180.0f);
        float c = cos(angle);
        float s = sin(angle);

        STATIC_VERTEX A0 = { {-width,0,0},{0,1,0},{1,0,0},0,1 };
        STATIC_VERTEX A1 = { {width,0,0},{0,1,0},{1,0,0},1,1 };
        STATIC_VERTEX A2 = { {width,height,0},{0,1,0},{1,0,0},1,0 };
        STATIC_VERTEX A3 = { {-width,height,0},{0,1,0},{1,0,0},0,0 };

        STATIC_VERTEX B0 = { {0,0,-width},{0,1,0},{1,0,0},0,1 };
        STATIC_VERTEX B1 = { {0,0,width},{0,1,0},{1,0,0},1,1 };
        STATIC_VERTEX B2 = { {0,height,width},{0,1,0},{1,0,0},1,0 };
        STATIC_VERTEX B3 = { {0,height,-width},{0,1,0},{1,0,0},0,0 };

        STATIC_VERTEX C0 = { {-width * c,0,-width * s},{0,1,0},{1,0,0},0,1 };
        STATIC_VERTEX C1 = { {width * c,0,width * s},{0,1,0},{1,0,0},1,1 };
        STATIC_VERTEX C2 = { {width * c,height,width * s},{0,1,0},{1,0,0},1,0 };
        STATIC_VERTEX C3 = { {-width * c,height,-width * s},{0,1,0},{1,0,0},0,0 };

        std::vector<STATIC_VERTEX> vertices = { A0,A1,A2,A3,B0,B1,B2,B3,C0,C1,C2,C3 };
        std::vector<unsigned int> indices = { 0,1,2,2,3,0,4,5,6,6,7,4,8,9,10,10,11,8 };
        m_mesh.init(vertices, indices, dx.device);
    }

    void update(DXcore& dx, float dt) {
        if (!grassCB) return; // Make sure grassCB created
        params.uTime += dt;
        dx.devicecontext->UpdateSubresource(grassCB, 0, NULL, &params, 0, 0);
    }

    void bindToShader(DXcore& dx) {
        dx.devicecontext->VSSetConstantBuffers(1, 1, &grassCB);
        dx.devicecontext->PSSetConstantBuffers(1, 1, &grassCB);
    }

    void grassBTS(DXcore& dx, Shader& shader, TextureManager& textureManager, const std::string& grassTexName, const std::string& noiseTexName, const std::string& heightmapName) {
        ID3D11ShaderResourceView* grassSRV = textureManager.find(grassTexName);
        if (grassSRV) {
            shader.updateTexturePS(&dx, "grassTex", grassSRV);
        }

        ID3D11ShaderResourceView* noiseSRV = textureManager.find(noiseTexName);
        if (noiseSRV) {
            shader.updateTextureVS(&dx, "noiseTex", noiseSRV);
            shader.updateTexturePS(&dx, "noiseTex", noiseSRV);
        }

        ID3D11ShaderResourceView* heightSRV = textureManager.find(heightmapName);
        if (heightSRV) {
            shader.updateTextureVS(&dx, "heightmapTex", heightSRV);
            shader.updateTexturePS(&dx, "heightmapTex", heightSRV);
        }

        ID3D11SamplerState* samp = textureManager.getSamplerState();
        if (samp) {
            shader.updateSamplerVS(&dx, "samp", samp);
            shader.updateSamplerPS(&dx, "samp", samp);
        }
    }


    void draw(DXcore& dx) {
        ID3D11Buffer* buffers[2] = { m_mesh.vertexBuffer,m_instanceBuffer };
        UINT strides[2] = { m_mesh.strides,(UINT)sizeof(INSTANCE_DATA) };
        UINT offsets[2] = { 0,0 };
        dx.devicecontext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
        dx.devicecontext->IASetIndexBuffer(m_mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        dx.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        dx.devicecontext->DrawIndexedInstanced(m_mesh.indicesSize, m_instanceCount, 0, 0, 0);
    }

    void release() {
        if (m_instanceBuffer) { m_instanceBuffer->Release(); m_instanceBuffer = nullptr; }
        if (grassCB) { grassCB->Release(); grassCB = nullptr; }
    }

};
