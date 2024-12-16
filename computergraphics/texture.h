#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Adapter.h"
#include "DXcore.h"
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Texture {
    ID3D11Texture2D* texture = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;

public:
    bool loadFromMemory(DXcore* dx, const unsigned char* data, size_t size) {
        int width, height, channels;
        unsigned char* texels = stbi_load_from_memory(data, (int)size, &width, &height, &channels, 0);
        if (!texels) {
            return false;
        }
        
        if (channels == 3) {
            unsigned char* texelsWithAlpha = new unsigned char[width * height * 4];
            for (int i = 0; i < width * height; i++) {
                texelsWithAlpha[i * 4 + 0] = texels[i * 3 + 0];
                texelsWithAlpha[i * 4 + 1] = texels[i * 3 + 1];
                texelsWithAlpha[i * 4 + 2] = texels[i * 3 + 2];
                texelsWithAlpha[i * 4 + 3] = 255;
            }
            stbi_image_free(texels);
            texels = texelsWithAlpha;
            channels = 4;
        }

        D3D11_TEXTURE2D_DESC texDesc;
        ZeroMemory(&texDesc, sizeof(texDesc));
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = texels;
        initData.SysMemPitch = width * channels;

        HRESULT hr = dx->device->CreateTexture2D(&texDesc, &initData, &texture);
        stbi_image_free(texels);
        if (FAILED(hr)) {
            printf("CreateTexture2D (texture) failed. HR=0x%08X\n", hr);
            exit(0);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        hr = dx->device->CreateShaderResourceView(texture, &srvDesc, &srv);
        if (FAILED(hr)) {
            printf("CreateShaderResourceView failed. HR=0x%08X\n", hr);
            exit(0);
        }

        return true;
    }

    ID3D11ShaderResourceView* getSRV() { return srv; }

    void free() {
        if (srv) { srv->Release(); srv = nullptr; }
        if (texture) { texture->Release(); texture = nullptr; }
    }

    ~Texture() {
        free();
    }
};

class Sampler {
    ID3D11SamplerState* state;
public:
    bool init(DXcore& dx) {
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = dx.device->CreateSamplerState(&samplerDesc, &state);
        dx.devicecontext->PSSetSamplers(0, 1, &state);
        return SUCCEEDED(hr);
    }

    ID3D11SamplerState* getState() { return state; }

    void free() {
        if (state) {
            state->Release();
            state = nullptr;
        }
    }

    ~Sampler() {
        free();
    }
};

class TextureManager {
public:
    map<string, Texture*> textures;
    Sampler sampler;

    bool init(DXcore& dx, const vector<string>& filenames) {
        // Initialize sampler once
        if (!sampler.init(dx)) return false;

        for (auto& filename : filenames) {
            
            ifstream file(filename, ios::binary);
            if (!file) {
                cout << "Failed to open texture file: " << filename << endl;
                continue;
            }

            ostringstream oss;
            oss << file.rdbuf();
            string fileContent = oss.str();

            Texture* texture = new Texture();
            if (!texture->loadFromMemory(&dx, (const unsigned char*)fileContent.data(), fileContent.size())) {
                printf("Failed to load texture %s from memory.\n", filename.c_str());
                exit(0);
                cout << "Failed to load texture from memory: " << filename << endl;
                delete texture;
                continue;
            }

            textures.insert({ filename, texture });
        }

        return true;
    }

    bool loadFromFile(DXcore* dx, const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cout << "Failed to open texture file: " << filename << endl;
            return false;
        }
        ostringstream oss;
        oss << file.rdbuf();
        string fileContent = oss.str();

        Texture* texture = new Texture();
        if (!texture->loadFromMemory(dx, (const unsigned char*)fileContent.data(), fileContent.size())) {
            cout << "Failed to load texture from " << filename << endl;
            delete texture;
            return false;
        }
        textures.insert({ filename, texture });
        return true;
    }

    ID3D11ShaderResourceView* find(const string& name) {
        auto it = textures.find(name);
        if (it != textures.end()) {
            return it->second->getSRV();
        }
        return nullptr;
    }

    ID3D11SamplerState* getSamplerState() {
        return sampler.getState();
    }

    void unload(const string& name) {
        if (textures.find(name) != textures.end()) {
            textures[name]->free();
            delete textures[name];
            textures.erase(name);
        }
    }

    ~TextureManager() {
        for (auto it = textures.begin(); it != textures.end(); ++it) {
            it->second->free();
            delete it->second;
        }
        textures.clear();
        sampler.free();
    }
};
