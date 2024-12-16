#pragma once
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include "mathLibrary.h"
#include "GEMLoader.h"
#include "mesh.h"
#include "DXcore.h"
#include "texture.h"
#include "Shader.h"

using namespace std;

struct Bone
{
    string name;
    Matrix44 offset;
    int parentIndex;
};

struct Skeleton
{
    vector<Bone> bones;
    Matrix44 globalInverse;
};

struct AnimationFrame
{
    vector<Vec3> positions;
    vector<Quaternion> rotations;
    vector<Vec3> scales;
};

class aniSequence
{
public:
    vector<AnimationFrame> frames;
    float ticksPerSecond;

    Vec3 interpolate(const Vec3& p1, const Vec3& p2, float t) {
        return (p1 * (1.0f - t) + p2 * t);
    }

    Quaternion interpolate(const Quaternion& q1, const Quaternion& q2, float t) {
        return Quaternion::slerp(q1, q2, t);
    }

    float duration() {
        return (frames.empty() ? 0.0f : (float)frames.size() / ticksPerSecond);
    }

    void calcFrame(float t, int& frame, float& interpolationFact)
    {
        interpolationFact = t * ticksPerSecond;
        frame = (int)floorf(interpolationFact);
        interpolationFact -= frame;
        frame = min(frame, (int)frames.size() - 1);
    }

    int nextFrame(int frame)
    {
        return min(frame + 1, (int)frames.size() - 1);
    }

    Matrix44 interpolateBoneToGlobal(Matrix44* matrices, int baseFrame, float interpolationFact, Skeleton* skeleton, int boneIndex) {
        int nextFrameIndex = nextFrame(baseFrame);

        // Interpolation based on data from current frame and the next frame
        Vec3 p = interpolate(frames[baseFrame].positions[boneIndex], frames[nextFrameIndex].positions[boneIndex], interpolationFact);
        Quaternion r = interpolate(frames[baseFrame].rotations[boneIndex], frames[nextFrameIndex].rotations[boneIndex], interpolationFact);
        Vec3 s = interpolate(frames[baseFrame].scales[boneIndex], frames[nextFrameIndex].scales[boneIndex], interpolationFact);

        Matrix44 scale = Matrix44::scaling(s);
        Matrix44 rotation = r.toMatrix();
        Matrix44 translation = Matrix44::translation(p);
        Matrix44 local = scale * rotation * translation;

        int parentIndex = skeleton->bones[boneIndex].parentIndex;
        if (parentIndex >= 0) {
            Matrix44 global = local * matrices[parentIndex];
            return global;
        }
        return local;
    }
};

class Animation
{
public:
    map<string, aniSequence> animations;
    Skeleton skeleton;

    int bonesSize() {
        return (int)skeleton.bones.size();
    }

    void calcFrame(string name, float t, int& frame, float& interpolationFact) {
        animations[name].calcFrame(t, frame, interpolationFact);
    }

    Matrix44 interpolateBoneToGlobal(string name, Matrix44* matrices, int baseFrame, float interpolationFact, int boneIndex) {
        return animations[name].interpolateBoneToGlobal(matrices, baseFrame, interpolationFact, &skeleton, boneIndex);
    }

    void calcFinalTransforms(Matrix44* matrices)
    {
        int count = bonesSize();
        for (int i = 0; i < count; i++)
        {
            matrices[i] = skeleton.bones[i].offset * matrices[i] * skeleton.globalInverse;
        }
    }
};

class aniInstance
{
public:
    Animation* animation;
    string currentAnimation;
    Matrix44 matrices[256];
    float t;

    aniInstance() : animation(nullptr), t(0.0f) {}

    void resetAnimationTime() {
        t = 0.0f;
    }

    bool animationFinished() {
        if (!animation || currentAnimation.empty()) return true;
        float dur = animation->animations[currentAnimation].duration();
        return (t > dur);
    }

    void update(string name, float dt) {
        if (!animation) return;

        if (name == currentAnimation) {
            t += dt;
        }
        else {
            currentAnimation = name;
            t = 0;
        }

        if (animationFinished()) {
            resetAnimationTime();
        }

        int frame = 0;
        float interpolationFact = 0;
        animation->calcFrame(name, t, frame, interpolationFact);

        int count = animation->bonesSize();
        for (int i = 0; i < count; i++)
        {
            matrices[i] = animation->interpolateBoneToGlobal(name, matrices, frame, interpolationFact, i);
        }

        animation->calcFinalTransforms(matrices);
    }
};

class aniMesh
{
public:
    vector<Mesh> geoset;
    Animation animation1;
    aniInstance instance;
    vector<string> textureFilenames;
    vector<string> normalFilenames;

    void loadMesh(DXcore* dx, const string& filename, TextureManager* textures)
    {
        GEMLoader::GEMModelLoader loader;
        vector<GEMLoader::GEMMesh> gemmeshes;
        GEMLoader::GEMAnimation gemanimation;
        loader.load(filename, gemmeshes, gemanimation);
        for (size_t i = 0; i < gemmeshes.size(); i++) {
            vector<ANIMATED_VERTEX> vertices;
            vertices.reserve(gemmeshes[i].verticesAnimated.size());
            for (auto& gv : gemmeshes[i].verticesAnimated) {
                ANIMATED_VERTEX v;
                memcpy(&v, &gv, sizeof(ANIMATED_VERTEX));
                vertices.push_back(v);
            }
            Mesh geo;
            geo.aniinit(dx->device, vertices, gemmeshes[i].indices);

            string diffuse = gemmeshes[i].material.find("diffuse").getValue();
            textureFilenames.push_back(diffuse);
            textures->loadFromFile(dx, diffuse);

            string normal = gemmeshes[i].material.find("normal").getValue();
            normalFilenames.push_back(normal);
            textures->loadFromFile(dx, normal);
            geoset.push_back(geo);
        }

        // Loading Skeletal Data
        memcpy(&animation1.skeleton.globalInverse, &gemanimation.globalInverse, sizeof(Matrix44));
        for (auto& b : gemanimation.bones) {
            Bone bone;
            bone.name = b.name;
            memcpy(&bone.offset, &b.offset, sizeof(Matrix44));
            bone.parentIndex = b.parentIndex;
            animation1.skeleton.bones.push_back(bone);
        }

        // Load animation data
        for (auto& anim : gemanimation.animations) {
            aniSequence aseq;
            aseq.ticksPerSecond = anim.ticksPerSecond;
            for (auto& f : anim.frames) {
                AnimationFrame frame;
                for (size_t index = 0; index < f.positions.size(); index++) {
                    frame.positions.push_back(*(Vec3*)&f.positions[index]);
                    frame.rotations.push_back(*(Quaternion*)&f.rotations[index]);
                    frame.scales.push_back(*(Vec3*)&f.scales[index]);
                }
                aseq.frames.push_back(frame);
            }
            animation1.animations.insert({ anim.name, aseq });
        }

        instance.animation = &animation1;
    }

    void draw(Shader* shader, DXcore* dx, float dt, TextureManager& textures, const string& animename, const Matrix44& Worldpos, const Matrix44& Transform)
    {
        instance.update(animename, dt);
        shader->updateConstantVS("animatedMeshBuffer", "W", (void*)&Worldpos);
        shader->updateConstantVS("animatedMeshBuffer", "VP", (void*)&Transform);
        shader->updateConstantVS("animatedMeshBuffer", "bones", instance.matrices);
        shader->apply(dx);

        for (size_t i = 0; i < geoset.size(); i++)
        {
            ID3D11ShaderResourceView* diffuseSRV = textures.find(textureFilenames[i]);
            ID3D11ShaderResourceView* normalSRV = textures.find(normalFilenames[i]);
            if (diffuseSRV) shader->updateTexturePS(dx, "tex", diffuseSRV);
            if (normalSRV) shader->updateTexturePS(dx, "normalMap", normalSRV);
            shader->apply(dx);
            geoset[i].draw(dx->devicecontext);
        }
    }
};

class Dinosaur : public aniMesh
{
public:
    Vec3 position;
    Vec3 target;
    Vec3 forward;
    float speed;
    float moveWidth;
    float moveHeight;
    float waitdt;
    bool isWaiting;

    Dinosaur(float sceneWidth, float sceneHeight) : moveWidth(sceneWidth * 3.0f), moveHeight(sceneHeight * 3.0f), position(0, 0, 0), forward(0, 0, 1), speed(400.0f), waitdt(0.0f), isWaiting(false)
    { generateRandomTarget(); }


    void generateRandomTarget(){
        float x = ((float)rand() / RAND_MAX) * moveWidth - moveWidth / 2.0f;
        float z = ((float)rand() / RAND_MAX) * moveHeight - moveHeight / 2.0f;
        target = Vec3(x, position.y, z);
    }

    void generateRandomTarget(const Vec3& cameraPos, float minDistanceFromCamera = 2000.0f){
        Vec3 potentialTarget;
        do
        {
            float x = ((float)rand() / RAND_MAX) * moveWidth - moveWidth / 2.0f;
            float z = ((float)rand() / RAND_MAX) * moveHeight - moveHeight / 2.0f;
            potentialTarget = Vec3(x, position.y, z);
        } while ((potentialTarget - cameraPos).Length() < minDistanceFromCamera);

        target = potentialTarget;
    }

    void updatePosition(float dt){
        if (isWaiting) {
            waitdt += dt;
            if (waitdt >= 3.0f) {
                isWaiting = false;
                waitdt = 0.0f;
                generateRandomTarget();
            }
            return;
        }

        Vec3 direction = target - position;
        float distance = direction.Length();
        if (distance < 0.01f) {
            position = target;
            isWaiting = true;
            return;
        }

        Vec3 directionNor = direction / distance;
        float moveDist = speed * dt;
        if (moveDist >= distance) {
            position = target;
            isWaiting = true;
        }
        else {
            position += directionNor * moveDist;
            forward = directionNor;
        }
    }

    void draw(Shader* shader, DXcore* dx, float dt, TextureManager& textures, const Matrix44& Transform){
        updatePosition(dt);

        string currentAnimation = (!isWaiting) ? "Run" : "roar";
        instance.update(currentAnimation, dt);

        Vec3 up(0.0f, 1.0f, 0.0f);
        Vec3 right = up.Cross(forward).Normalize();
        Vec3 adjustedUp = forward.Cross(right).Normalize();

        Matrix44 rotationMatrix(
            right.x, right.y, right.z, 0.0f,
            adjustedUp.x, adjustedUp.y, adjustedUp.z, 0.0f,
            forward.x, forward.y, forward.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        Matrix44 Worldpos = Matrix44::translation(position);
        Matrix44 scaleMatrix = Matrix44::scaling(Vec3(300, 300, 300));
        Matrix44 biggerDefault = scaleMatrix * rotationMatrix * Worldpos;

        shader->updateConstantVS("animatedMeshBuffer", "W", &biggerDefault);
        shader->updateConstantVS("animatedMeshBuffer", "VP", (void*)&Transform);
        shader->updateConstantVS("animatedMeshBuffer", "bones", instance.matrices);
        shader->apply(dx);

        for (size_t i = 0; i < geoset.size(); i++)
        {
            ID3D11ShaderResourceView* diffuseSRV = textures.find(textureFilenames[i]);
            ID3D11ShaderResourceView* normalSRV = nullptr;
            if (i < normalFilenames.size()) {
                normalSRV = textures.find(normalFilenames[i]);
            }

            if (diffuseSRV) shader->updateTexturePS(dx, "tex", diffuseSRV);
            if (normalSRV)  shader->updateTexturePS(dx, "normalMap", normalSRV);

            shader->apply(dx);
            geoset[i].draw(dx->devicecontext);
        }
    }
};
