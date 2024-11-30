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

using namespace std;

class Shader {
public:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* layout;
    vector<ConstantBuffer> psConstantBuffers;
    vector<ConstantBuffer> vsConstantBuffers;
    map<string, int> textureBindPointsVS;
    map<string, int> textureBindPointsPS;
    bool hasLayout;


    void init(string vs, string ps, DXcore &dx) {
        
            // Read the vertex shader and pixel shader .txt files
            string vsShader = readFile(vs);
            string psShader= readFile(ps);

            // Compile & create vertex shader and pixel shader
            compileVS(vsShader, dx.device);
            compilePS(psShader, dx.device);

            apply(&dx);
        }

    // map<string, Shader> shader;
    string readFile(string filename) {
        ifstream file(filename);
        stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    void loadVS(DXcore* dx, string vertexShaderHLSL){
        ID3DBlob* shader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(vertexShaderHLSL.c_str(), strlen(vertexShaderHLSL.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &shader, &status);
        if (FAILED(hr))
        {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }
        dx->device->CreateVertexShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &vertexShader);
        ConstantBufferReflection reflection;
        reflection.build(dx, shader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
    }

    void loadPS(DXcore* dx, string pixelShaderHLSL)
    {
        ID3DBlob* shader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(pixelShaderHLSL.c_str(), strlen(pixelShaderHLSL.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &shader, &status);
        if (FAILED(hr))
        {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }
        dx->device->CreatePixelShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &pixelShader);
        ConstantBufferReflection reflection;
        reflection.build(dx, shader, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
    }

    void compileVS(string vertexShaderHLSL, ID3D11Device* &device) {
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
            { "COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 

        };

        device->CreateInputLayout(layoutDesc, 2, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
       

    }

    void compilePS(string pixelShaderHLSL, ID3D11Device* &device) {
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

    

    void apply(DXcore* dx) {
        if (hasLayout == 1) {
            dx->devicecontext->IASetInputLayout(layout);
        }
        else {
            dx->devicecontext->IASetInputLayout(NULL);
        }
        dx->devicecontext->VSSetShader(vertexShader, NULL, 0);
        dx->devicecontext->PSSetShader(pixelShader, NULL, 0);
        for (int i = 0; i < vsConstantBuffers.size(); i++)
        {
            vsConstantBuffers[i].upload(dx);
        }
        for (int i = 0; i < psConstantBuffers.size(); i++)
        {
            psConstantBuffers[i].upload(dx);
        }
    }


};


