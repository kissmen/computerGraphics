#pragma once
#include <d3d11.h>
#include <vector>
#include "DXcore.h"
#include "Shader.h"
#include "mesh.h"
#include "texture.h" // TextureManager
#include "mathLibrary.h"
#include <cstdlib> // for rand()

class Grass {
public:
    ID3D11Buffer* m_instanceBuffer;
    ID3D11ShaderResourceView* m_grassSRV;
    ID3D11ShaderResourceView* m_noiseSRV;
    Mesh m_mesh;
    int m_instanceCount;

    struct INSTANCE_DATA {
        float x, y, z;
    };

    struct GrassParamsCB {
        float uTime;
        float strength;
        float scaleVal;
        float pad;
        float uColor[3];
        float pad2;
    };
    // for constant buffer
    GrassParamsCB params;
    ID3D11Buffer* grassCB;

    Grass() : m_instanceBuffer(nullptr), m_instanceCount(0), grassCB(nullptr),
        m_grassSRV(nullptr), m_noiseSRV(nullptr)
    {
        params.uTime = 0.0f;
        params.strength = 40.0f;
        params.scaleVal = 1.0f;
        params.uColor[0] = 0.2f;
        params.uColor[1] = 1.0f;
        params.uColor[2] = 0.5f;
    }

    ~Grass() {
        release();
    }

    void init(DXcore& dx, int instanceCount, float areaSize) {
        m_instanceCount = instanceCount;
        std::vector<INSTANCE_DATA> instanceData(instanceCount);
        for (int i = 0; i < instanceCount; i++) {
            float x = ((float)rand() / RAND_MAX) * areaSize * 2 - areaSize;
            float z = ((float)rand() / RAND_MAX) * areaSize * 2 - areaSize;
            float y = 0.0f;
            instanceData[i].x = x; instanceData[i].y = y; instanceData[i].z = z;
        }

        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(INSTANCE_DATA) * instanceCount;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = instanceData.data();
        dx.device->CreateBuffer(&bd, &initData, &m_instanceBuffer);

        // create CB
        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(GrassParamsCB);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = 0;
        cbd.MiscFlags = 0;
        dx.device->CreateBuffer(&cbd, NULL, &grassCB);

        // X-shaped mesh, two intersecting planes
        float width = 12.0f;
        float height = 20.0f;

        // bottom v=0, top v=1
        STATIC_VERTEX A0 = { {-width,0,0},{0,1,0},{1,0,0},0,1 };   // bottom tv=0
        STATIC_VERTEX A1 = { {width,0,0},{0,1,0},{1,0,0},1,1 };   // bottom tv=0
        STATIC_VERTEX A2 = { {width,height,0},{0,1,0},{1,0,0},1,0 }; // top tv=0
        STATIC_VERTEX A3 = { {-width,height,0},{0,1,0},{1,0,0},0,0 }; // top tv=0

        // same for b
        STATIC_VERTEX B0 = { {0,0,-width},{0,1,0},{1,0,0},0,1 };
        STATIC_VERTEX B1 = { {0,0,width},{0,1,0},{1,0,0},1,1 };
        STATIC_VERTEX B2 = { {0,height,width},{0,1,0},{1,0,0},1,0 };
        STATIC_VERTEX B3 = { {0,height,-width},{0,1,0},{1,0,0},0,0 };

        std::vector<STATIC_VERTEX> vertices = { A0,A1,A2,A3,B0,B1,B2,B3 };
        std::vector<unsigned int> indices = { 0,1,2,2,3,0,4,5,6,6,7,4 };
        m_mesh.init(vertices, indices, dx.device);
    }

    void update(DXcore& dx, float dt) {
        params.uTime += dt;
        dx.devicecontext->UpdateSubresource(grassCB, 0, NULL, &params, 0, 0);
    }

    void setStrength(float s) { params.strength = s; }
    void setScale(float sc) { params.scaleVal = sc; }
    void setColor(float r, float g, float b) { params.uColor[0] = r; params.uColor[1] = g; params.uColor[2] = b; }

    // Make CB bind to b1 of VS and PS
    void bindToShader(DXcore& dx) {
        dx.devicecontext->VSSetConstantBuffers(1, 1, &grassCB);
        dx.devicecontext->PSSetConstantBuffers(1, 1, &grassCB);
    }

    // textureManager.find to get SRV then use shader.updateTexturePS bind
    void grassBTS(DXcore& dx, Shader& shader, TextureManager& textureManager,
        const std::string& grassTexName, const std::string& noiseTexName)
    {
        ID3D11ShaderResourceView* grassSRV = textureManager.find(grassTexName);
        if (grassSRV) {
            shader.updateTexturePS(&dx, "grassTex", grassSRV);
        }

        ID3D11ShaderResourceView* noiseSRV = textureManager.find(noiseTexName);
        if (noiseSRV) {
            shader.updateTexturePS(&dx, "noiseTex", noiseSRV);
        }

        ID3D11SamplerState* samp = textureManager.getSamplerState();
        if (samp) {
            shader.updateSamplerPS(&dx, "samp", samp);
        }
    }

    void draw(DXcore& dx) {
        // Set VB and InstanceBuffer
        ID3D11Buffer* buffers[2] = { m_mesh.vertexBuffer,m_instanceBuffer };
        UINT strides[2] = { m_mesh.strides,sizeof(INSTANCE_DATA) };
        UINT offsets[2] = { 0,0 };
        dx.devicecontext->IASetVertexBuffers(0, 2, buffers, strides, offsets);
        dx.devicecontext->IASetIndexBuffer(m_mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        dx.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        dx.devicecontext->DrawIndexedInstanced(m_mesh.indicesSize, m_instanceCount, 0, 0, 0);
    }

    void release() {
        if (m_instanceBuffer) {
            m_instanceBuffer->Release();
            m_instanceBuffer = nullptr;
        }
        if (grassCB) {
            grassCB->Release();
            grassCB = nullptr;
        }
    }

};