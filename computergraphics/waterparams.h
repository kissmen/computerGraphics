#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "DXcore.h"
#include "Shader.h"
#include "mesh.h"
#include "texture.h" // TextureManager

using namespace std;

class WaterParams {
public:
    struct WaterParamsCB {
        float uTime;
        float strength;
        float scaleVal;
        float pad;
        float uColor[3];
        float pad2;
    };

    WaterParamsCB params;

    ID3D11Buffer* waterCB = nullptr;

    void init(DXcore& dx) {

        params.uTime = 0.0f;
        params.strength = 80.0f;
        params.scaleVal = 18.0f;
        params.uColor[0] = 0.25f;
        params.uColor[1] = 0.45f;
        params.uColor[2] = 1.0f;

        // b1
        D3D11_BUFFER_DESC bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(WaterParamsCB);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        HRESULT hr = dx.device->CreateBuffer(&bd, NULL, &waterCB);
        if (FAILED(hr)) {
            
        }
    }

    void update(DXcore& dx, float dt) {
        params.uTime += dt;
        dx.devicecontext->UpdateSubresource(waterCB, 0, NULL, &params, 0, 0);
    }

    void setStrength(float s) { params.strength = s; }
    void setScale(float sc) { params.scaleVal = sc; }
    void setColor(float r, float g, float b) { params.uColor[0] = r; params.uColor[1] = g; params.uColor[2] = b; }

    // waterCB
    void bindToShader(DXcore& dx) {
        dx.devicecontext->VSSetConstantBuffers(1, 1, &waterCB);
        dx.devicecontext->PSSetConstantBuffers(1, 1, &waterCB);
    }

    void waterBTS(DXcore& dx, Shader& shader, TextureManager& textureManager, const string& textureName) {
        ID3D11ShaderResourceView* srv = textureManager.find(textureName);
        if (srv) {
            shader.updateTexturePS(&dx, "tex", srv);
        }

        ID3D11SamplerState* samp = textureManager.getSamplerState();
        if (samp) {
            shader.updateSamplerPS(&dx, "samp", samp);
        }
    }

    // Water plane
    static Mesh createHighResPlane(DXcore& dx, float width = 50.0f, float depth = 200.0f, int xSubdiv = 100, int zSubdiv = 100) {
        vector<STATIC_VERTEX> vertices;
        vector<unsigned int> indices;

        float halfWidth = width * 0.5f;
        float halfDepth = depth * 0.5f;

        float dxStep = width / xSubdiv;
        float dzStep = depth / zSubdiv;

        for (int z = 0; z <= zSubdiv; z++) {
            float zPos = -halfDepth + z * dzStep;
            for (int x = 0; x <= xSubdiv; x++) {
                float xPos = -halfWidth + x * dxStep;
                STATIC_VERTEX v;
                v.pos = { xPos, 0.0f, zPos };
                v.normal = { 0.0f, 1.0f, 0.0f };
                v.tangent = { 1.0f, 0.0f, 0.0f };
                v.tu = (float)x / xSubdiv;
                v.tv = (float)z / zSubdiv;
                vertices.push_back(v);
            }
        }

        for (int z = 0; z < zSubdiv; z++) {
            for (int x = 0; x < xSubdiv; x++) {
                int start = z * (xSubdiv + 1) + x;
                indices.push_back(start);
                indices.push_back(start + xSubdiv + 1);
                indices.push_back(start + 1);

                indices.push_back(start + 1);
                indices.push_back(start + xSubdiv + 1);
                indices.push_back(start + xSubdiv + 2);
            }
        }

        Mesh planeMesh;
        planeMesh.init(vertices, indices, dx.device);
        return planeMesh;
    }
};


