#pragma once
#include "Mesh.h"
#include "render.h"
#include "GEMLoader.h"
#include "texture.h"
using namespace std;

class Plane {
public:
    Mesh plane;
    Material mt;

    Plane() : mt() {}

    STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv) {
        STATIC_VERTEX v;
        v.pos = p;
        v.normal = n;
        v.tangent = Vec3(0, 0, 0);
        v.tu = tu;
        v.tv = tv;
        return v;
    }

    void init(DXcore* dx, TextureManager& textureManager, const string& diffuseTex) {
        vector<STATIC_VERTEX> vertices;
        vertices.push_back(addVertex(Vec3(-400, 0, -400), Vec3(0, 1, 0), 0, 0));
        vertices.push_back(addVertex(Vec3(400, 0, -400), Vec3(0, 1, 0), 1, 0));
        vertices.push_back(addVertex(Vec3(-400, 0, 400), Vec3(0, 1, 0), 0, 1));
        vertices.push_back(addVertex(Vec3(400, 0, 400), Vec3(0, 1, 0), 1, 1));

        vector<unsigned int> indices = { 2, 1, 0, 1, 2, 3 };
        plane.init(vertices, indices, dx->device);
        mt = Material(&textureManager, diffuseTex);
    }

    void draw(DXcore* dx, Shader& shader) {
        mt.apply(dx, shader);
        plane.draw(dx->devicecontext);
    }
};

class Tree {
public:
    vector<Render> mtpart;

    void init(DXcore* dx, TextureManager& textureManager, const string& filename) {
        vector<GEMLoader::GEMMesh> oak;
        GEMLoader::GEMModelLoader loader;
        loader.load(filename, oak);

        for (size_t i = 0; i < oak.size(); i++) {
            vector<STATIC_VERTEX> vertices;
            for (auto& ov : oak[i].verticesStatic) {
                STATIC_VERTEX v;
                v.pos = Vec3(ov.position.x, ov.position.y, ov.position.z);
                v.normal = Vec3(ov.normal.x, ov.normal.y, ov.normal.z);
                v.tangent = Vec3(ov.tangent.x, ov.tangent.y, ov.tangent.z);
                v.tu = ov.u;
                v.tv = ov.v;
                vertices.push_back(v);
            }

            Mesh m;
            m.init(vertices, oak[i].indices, dx->device);

            string diffuseTexture = oak[i].material.find("diffuse").getValue();
            Material mat(&textureManager, diffuseTexture);
            mtpart.push_back(Render(m, mat));
        }
    }


    void draw(DXcore* dx, Shader& shader) {
        for (auto& part : mtpart) {
            part.draw(dx, shader);
        }
    }
};
