#pragma once
#pragma comment(lib, "dxguid.lib")
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include "DXcore.h"
#include "d3dcompiler.h"
#include "d3d11shader.h"
#include "shaderreflection.h"
#include "mathLibrary.h"
#include "Adapter.h"

using namespace std;

class Shader {
public:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11GeometryShader* geometryShader;
    ID3D11InputLayout* layout;
    ID3D11Buffer* cb;
    ID3D11ShaderResourceView* srv;
    ID3D11SamplerState* state;
    vector<ConstantBuffer> psConstantBuffers;
    vector<ConstantBuffer> vsConstantBuffers;
    map<string, int> textureBindPointsVS;
    map<string, int> textureBindPointsPS;
    map<string, ID3D11ShaderResourceView*> vsTextures;
    map<string, ID3D11ShaderResourceView*> psTextures;
    map<string, ID3D11SamplerState*> vsSamplers;
    map<string, ID3D11SamplerState*> psSamplers;
    bool hasLayout = 1;


   

    // map<string, Shader> shader;
    string readFile(string filename) {
        ifstream file(filename);
        stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    void init(string vs, string ps, DXcore& dx) {

        // Read the vertex shader and pixel shader .txt files
        string vsShader = readFile(vs);
        string psShader = readFile(ps);

        // Compile & create vertex shader and pixel shader
        loadVS(&dx, vsShader);
        loadPS(&dx, psShader);
        compileVS(dx.device, vsShader);
        compilePS(dx.device, psShader);

        apply(&dx);
    }


    void loadVS(DXcore* dx, string vertexShaderHLSL){
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(vertexShaderHLSL.c_str(), strlen(vertexShaderHLSL.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }
        dx->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        ConstantBufferReflection reflection;
        reflection.build(dx, compiledVertexShader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
    }

    void compileVS(ID3D11Device*& device, string vertexShaderHLSL) {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(vertexShaderHLSL.c_str(), strlen(vertexShaderHLSL.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }
        device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            // New add for instance
            { "INSTANCEPOSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1},
        };
        UINT numElements = _countof(layoutDesc);

        device->CreateInputLayout(layoutDesc, numElements, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
       
        compiledVertexShader->Release();
    }

    void loadPS(DXcore* dx, string pixelShaderHLSL)
    {
        ID3DBlob* compiledPixelShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(pixelShaderHLSL.c_str(), strlen(pixelShaderHLSL.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPixelShader, &status);
        if (FAILED(hr))
        {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }
        dx->device->CreatePixelShader(compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &pixelShader);
        ConstantBufferReflection reflection;
        reflection.build(dx, compiledPixelShader, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
    }

    void compilePS(ID3D11Device*& device, string pixelShaderHLSL) {
        ID3DBlob* compiledPixelShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(pixelShaderHLSL.c_str(), strlen(pixelShaderHLSL.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPixelShader, &status);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Pixel Shader Error", 0);
            exit(0);
        }
        device->CreatePixelShader(compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &pixelShader);
    }


    void updateConstant(string constantBufferName, string variableName, void* data, vector<ConstantBuffer>& buffers)
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            if (buffers[i].name == constantBufferName)
            {
                buffers[i].update(variableName, data);
                return;
            }
        }
    }

    void updateConstantVS(string constantBufferName, string variableName, void* data)
    {
        updateConstant(constantBufferName, variableName, data, vsConstantBuffers);
    }
    void updateConstantPS(string constantBufferName, string variableName, void* data)
    {
        updateConstant(constantBufferName, variableName, data, psConstantBuffers);
    }

    // Update the texture for the vertex shader
    void updateTextureVS(DXcore* dx, string name, ID3D11ShaderResourceView* textureSRV) {
        if (textureBindPointsVS.find(name) != textureBindPointsVS.end()) {
            vsTextures[name] = textureSRV;
        }
    }

    // Update the texture for the pixel shader and store it for applying later
    void updateTexturePS(DXcore* dx, string name, ID3D11ShaderResourceView* textureSRV) {
        // Ensure the texture name is known by reflection
        if (textureBindPointsPS.find(name) != textureBindPointsPS.end()) {
            psTextures[name] = textureSRV;
        }
    }

    // Update the sampler for the vertex shader
    void updateSamplerVS(DXcore* dx, string name, ID3D11SamplerState* samplerState) {
        if (textureBindPointsVS.find(name) != textureBindPointsVS.end()) {
            vsSamplers[name] = samplerState;
        }
    }
    
    // Update the sampler for the pixel shader and store it for applying later
    void updateSamplerPS(DXcore* dx, string name, ID3D11SamplerState* samplerState) {
        if (textureBindPointsPS.find(name) != textureBindPointsPS.end()) {
            psSamplers[name] = samplerState;
        }
    }

    void apply(DXcore* dx) {
        if (hasLayout == 1) {
            dx->devicecontext->IASetInputLayout(layout);
        }
        else {
            dx->devicecontext->IASetInputLayout(NULL);
        }
        dx->devicecontext->VSSetShader(vertexShader, NULL, 0);
        dx->devicecontext->PSSetShader(pixelShader, NULL, 0);

        // Upload constants
        for (int i = 0; i < vsConstantBuffers.size(); i++)
        {
            vsConstantBuffers[i].upload(dx);
        }
        for (int i = 0; i < psConstantBuffers.size(); i++)
        {
            psConstantBuffers[i].upload(dx);
        }
        // Set VS textures
        for (auto& tex : vsTextures) {
            int slot = textureBindPointsVS[tex.first];
            dx->devicecontext->VSSetShaderResources(slot, 1, &tex.second);
        }

        // Set VS samplers
        for (auto& samp : vsSamplers) {
            int slot = textureBindPointsVS[samp.first];
            dx->devicecontext->VSSetSamplers(slot, 1, &samp.second);
        }

        // Set PS textures
        for (auto& tex : psTextures) {
            int slot = textureBindPointsPS[tex.first];
            dx->devicecontext->PSSetShaderResources(slot, 1, &tex.second);
        }

        // Set PS samplers
        for (auto& samp : psSamplers) {
            int slot = textureBindPointsPS[samp.first];
            dx->devicecontext->PSSetSamplers(slot, 1, &samp.second);
        }
    }
};


