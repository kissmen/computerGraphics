#pragma once
#include "DXcore.h"

class GBuffer {
public:
    ID3D11Texture2D* albedoTex;
    ID3D11Texture2D* normalTex;
    ID3D11Texture2D* depthTex;

    ID3D11RenderTargetView* albedoRTV;
    ID3D11RenderTargetView* normalRTV;
    ID3D11RenderTargetView* depthRTV;

    ID3D11ShaderResourceView* albedoSRV;
    ID3D11ShaderResourceView* normalSRV;
    ID3D11ShaderResourceView* depthSRV;

    void init(DXcore& dx, int width, int height) {
        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        // create Albedo
        HRESULT hr = dx.device->CreateTexture2D(&texDesc, NULL, &albedoTex);
        if (FAILED(hr)) {
            printf("CreateTexture2D for albedoTex failed. HR=0x%08X\n", hr);
            exit(0);
        }
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = texDesc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        hr = dx.device->CreateRenderTargetView(albedoTex, &rtvDesc, &albedoRTV);
        if (FAILED(hr)) { printf("CreateRenderTargetView albedo failed. HR=0x%08X\n", hr); exit(0); }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        hr = dx.device->CreateShaderResourceView(albedoTex, &srvDesc, &albedoSRV);
        if (FAILED(hr)) { printf("CreateShaderResourceView albedo failed. HR=0x%08X\n", hr); exit(0); }

        // Normal
        hr = dx.device->CreateTexture2D(&texDesc, NULL, &normalTex);
        if (FAILED(hr)) { printf("CreateTexture2D normalTex failed. HR=0x%08X\n", hr); exit(0); }
        hr = dx.device->CreateRenderTargetView(normalTex, &rtvDesc, &normalRTV);
        if (FAILED(hr)) { printf("CreateRenderTargetView normal failed. HR=0x%08X\n", hr); exit(0); }
        hr = dx.device->CreateShaderResourceView(normalTex, &srvDesc, &normalSRV);
        if (FAILED(hr)) { printf("CreateShaderResourceView normal failed. HR=0x%08X\n", hr); exit(0); }


        // Depth
        hr = dx.device->CreateTexture2D(&texDesc, NULL, &depthTex);
        if (FAILED(hr)) { printf("CreateTexture2D depthTex failed. HR=0x%08X\n", hr); exit(0); }
        hr = dx.device->CreateRenderTargetView(depthTex, &rtvDesc, &depthRTV);
        if (FAILED(hr)) { printf("CreateRenderTargetView depth failed. HR=0x%08X\n", hr); exit(0); }
        hr = dx.device->CreateShaderResourceView(depthTex, &srvDesc, &depthSRV);
        if (FAILED(hr)) { printf("CreateShaderResourceView depth failed. HR=0x%08X\n", hr); exit(0); }

    }

    void release() {
        if (albedoTex) albedoTex->Release();
        if (normalTex) normalTex->Release();
        if (depthTex) depthTex->Release();
        if (albedoRTV) albedoRTV->Release();
        if (normalRTV) normalRTV->Release();
        if (depthRTV) depthRTV->Release();
        if (albedoSRV) albedoSRV->Release();
        if (normalSRV) normalSRV->Release();
        if (depthSRV) depthSRV->Release();
    }
};
