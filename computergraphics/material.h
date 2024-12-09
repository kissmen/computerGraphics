#pragma once
#include <string>
#include "DXcore.h"
#include "Shader.h"
#include "texture.h" // For TextureManager
#include "geometry.h"

class Material {
public:
    std::string diffuseTextureName;
    TextureManager* textureManager;

    Material() : textureManager(nullptr) {}
    Material(TextureManager* tm, const std::string& diffuse = "")
        : textureManager(tm), diffuseTextureName(diffuse) {}

    void apply(DXcore* dx, Shader& shader) {
        if (textureManager && !diffuseTextureName.empty()) {
            ID3D11ShaderResourceView* srv = textureManager->find(diffuseTextureName);
            if (srv) {
                shader.updateTexturePS(dx, "tex", srv);
            }
        }
        shader.apply(dx);
    }
};


