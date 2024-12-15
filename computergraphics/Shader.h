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
  //  bool isInstanced = false; 

   
    
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

    // For Deferred Shading init 
    //void init(string vs, string ps, DXcore& dx) {
    //    string vsShader = readFile(vs);
    //    if (vsShader.empty()) {
    //        printf("Failed to read vertex shader file: %s\n", vs.c_str());
    //        MessageBoxA(NULL, "Vertex shader file not found or empty.", "Error", MB_OK);
    //        exit(0);
    //    }
    //    printf("Shader source for %s:\n%s\n", vs.c_str(), vsShader.c_str());
    //    string psShader = readFile(ps);

    //    ID3DBlob* compiledVS = nullptr;
    //    ID3DBlob* vsStatus = nullptr;
    //    HRESULT hr = D3DCompile(vsShader.c_str(), vsShader.size(), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVS, &vsStatus);
    //    if (FAILED(hr)) {
    //        if (vsStatus) {
    //            printf("VS Error: %s\n", (char*)vsStatus->GetBufferPointer());
    //        }else {
    //            printf("VS compile failed, no error message available.\n");
    //        }
    //        MessageBoxA(NULL, "Vertex shader compilation failed. Check console output.", "Error", MB_OK);
    //        exit(0);
    //    }
    //    hr = dx.device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), NULL, &vertexShader);
    //    if (FAILED(hr)) {
    //        printf("CreateVertexShader failed. HR=0x%08X\n", hr);
    //        exit(0);
    //    }
    //    // VS Reflection
    //    ConstantBufferReflection reflection;
    //    reflection.build(&dx, compiledVS, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);

    //    if (vs.find("NoInst") != std::string::npos) {
    //        isInstanced = false;
    //        // ??????????
    //        D3D11_INPUT_ELEMENT_DESC layoutDescNoInst[] = {
    //            { "POS",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
    //        };
    //        hr = dx.device->CreateInputLayout(layoutDescNoInst, _countof(layoutDescNoInst),
    //            compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &layout);
    //        if (FAILED(hr)) {
    //            printf("CreateInputLayout for NoInst failed. HR=0x%08X\n", hr);
    //            exit(0);
    //        }
    //    }
    //    else if (vs.find("Inst") != std::string::npos) {
    //        isInstanced = true;
    //        // ?????????
    //        D3D11_INPUT_ELEMENT_DESC layoutDescInst[] = {
    //            { "POS",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "INSTANCEPOSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1}
    //        };
    //        hr = dx.device->CreateInputLayout(layoutDescInst, _countof(layoutDescInst),
    //            compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &layout);
    //        if (FAILED(hr)) {
    //            printf("CreateInputLayout for Inst failed. HR=0x%08X\n", hr);
    //            exit(0);
    //        }
    //    }
    //    else {
    //        // ????? NoInst ??? Inst????????
    //        isInstanced = false;
    //        D3D11_INPUT_ELEMENT_DESC layoutDescNoInst[] = {
    //            { "POS",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
    //            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
    //        };
    //        hr = dx.device->CreateInputLayout(layoutDescNoInst, _countof(layoutDescNoInst),
    //            compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &layout);
    //        if (FAILED(hr)) {
    //            printf("CreateInputLayout default(NoInst) failed. HR=0x%08X\n", hr);
    //            exit(0);
    //        }
    //    }

    //    compiledVS->Release();

    //    ID3DBlob* compiledPS = nullptr;
    //    ID3DBlob* psStatus = nullptr;
    //    hr = D3DCompile(psShader.c_str(), psShader.size(), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPS, &psStatus);
    //    if (FAILED(hr)) {
    //        printf("PS Error: %s\n", (char*)psStatus->GetBufferPointer());
    //        exit(0);
    //    }
    //    hr = dx.device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), NULL, &pixelShader);
    //    if (FAILED(hr)) {
    //        printf("CreatePixelShader failed. HR=0x%08X\n", hr);
    //        exit(0);
    //    }
    //    // PS reflection
    //    reflection.build(&dx, compiledPS, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
    //    compiledPS->Release();

    //    apply(&dx);
    //}

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
    // For For Deferred Shading compile
    /*void compileVSNoInst(ID3D11Device* device, const std::string& vsShader) {
        ID3DBlob* compiledVS = nullptr;
        ID3DBlob* status = nullptr;
        HRESULT hr = D3DCompile(vsShader.c_str(), vsShader.size(), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVS, &status);
        if (FAILED(hr)) {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }

        device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), NULL, &vertexShader);

        D3D11_INPUT_ELEMENT_DESC layoutDescNoInst[] = {
            { "POS",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        };
        UINT numElements = _countof(layoutDescNoInst);
        device->CreateInputLayout(layoutDescNoInst, numElements, compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &layout);

        compiledVS->Release();
    }

    void compileVSInst(ID3D11Device* device, const std::string& vsShader) {
        ID3DBlob* compiledVS = nullptr;
        ID3DBlob* status = nullptr;
        HRESULT hr = D3DCompile(vsShader.c_str(), vsShader.size(), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVS, &status);
        if (FAILED(hr)) {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }

        device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), NULL, &vertexShader);

        D3D11_INPUT_ELEMENT_DESC layoutDescInst[] = {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
            { "INSTANCEPOSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,1,0,D3D11_INPUT_PER_INSTANCE_DATA,1}
        };
        UINT numElements = _countof(layoutDescInst);
        device->CreateInputLayout(layoutDescInst, _countof(layoutDescInst), compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &layout);

        compiledVS->Release();
    }*/

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


