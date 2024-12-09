#pragma once
#include "geometry.h"
#include "Material.h"

class Render {
public:
    Mesh mesh;
    Material material;

    Render(const Mesh& m, const Material& mat) : mesh(m), material(mat) {}

    void draw(DXcore* dx, Shader& shader) {
        material.apply(dx, shader);
        mesh.draw(dx->devicecontext);
    }
};