#pragma once
#include "DXcore.h"
#include "Mesh.h"
#include "Shader.h"
#include "texture.h"
#include "material.h"
#include "mathLibrary.h"
#include "Camera.h"

class SkyDome {
public:
    Mesh domeMesh;
    Material domeMaterial;
    float radius;
    int slices;
    int stacks;

    SkyDome(float r = 50000.0f, int sl = 64, int st = 64) : radius(r), slices(sl), stacks(st) {}

    void createSphereMesh(ID3D11Device* device) {
        std::vector<STATIC_VERTEX> vertices;
        std::vector<unsigned int> indices;

        for (int stack = 0; stack <= stacks; stack++) {
            float phi = (float)stack / (float)stacks * PI;
            for (int slice = 0; slice <= slices; slice++) {
                float theta = (float)slice / (float)slices * 2.0f * PI;
                STATIC_VERTEX v;
                v.pos.x = radius * sinf(phi) * cosf(theta);
                v.pos.y = radius * cosf(phi);
                v.pos.z = radius * sinf(phi) * sinf(theta);

                // Normal and tangent
                v.normal = Vec3(0, 1, 0);
                v.tangent = Vec3(1, 0, 0);
                // UV
                v.tu = theta / (2.0f * PI);
                v.tv = phi / PI;
                vertices.push_back(v);
            }
        }

        for (int stack = 0; stack < stacks; stack++) {
            for (int slice = 0; slice < slices; slice++) {
                int first = stack * (slices + 1) + slice;
                int second = first + slices + 1;
                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);
                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        domeMesh.init(vertices, indices, device);
    }

    void init(DXcore& dx, TextureManager& textureManager, const std::string& skyTextureFile) {
        createSphereMesh(dx.device);

        domeMaterial = Material(&textureManager, skyTextureFile);
    }

    void draw(DXcore* dx, Shader& shader, Camera& camera) {
        //// sky dome at camera pos always around camera
        Matrix44 translation = Matrix44::translation(Vec3(camera.eye.x, camera.eye.y, camera.eye.z));
        shader.updateConstantVS("staticMeshBuffer", "W", &translation);

        // DXcore for set Depth of sky
        dx->setDepthStateSky();

        domeMaterial.apply(dx, shader);
        domeMesh.draw(dx->devicecontext);

        // Restore default depth state
        dx->setDepthStateDefault();
    }
};
