#pragma once
#include "mathLibrary.h"
#include <algorithm> 
#include "window.h"
#include <Windows.h>

using namespace std;

class Camera {
public:
    Vec4 eye;
    Vec4 center;
    Vec4 up;
    Transform transform;

    float speed = 100.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;

    float fov = 60.0f;     // view field
    float nearPlane = 0.1f;
    float farPlane = 100000.0f;

    float width;
    float height;

    Camera(const Vec4& _eye, const Vec4& _center, const Vec4& _up, float w, float h)
        : eye(_eye), center(_center), up(_up), width(w), height(h)
    {
        Vec4 forward = (center - eye).normalize();
        yaw = atan2f(forward.z, forward.x);
        pitch = asinf(forward.y);

        updateViewMatrix();
        updateProjectionMatrix(); // get aspect by win width and height
    }

    void updateProjectionMatrix() {
        float aspect = width / height;
        transform.setProjectionMatrix(fov, aspect, nearPlane, farPlane);
    }

    void updateViewMatrix() {
        Vec4 forward(
            cosf(yaw) * cosf(pitch),
            sinf(pitch),
            sinf(yaw) * cosf(pitch),
            0.0f
        );
        forward = forward.normalize();
        center = eye + forward;
        transform.setViewMatrix(eye, center, up);
    }

    // call when win changes
    void onWindowResize(float newWidth, float newHeight) {
        width = newWidth;
        height = newHeight;
        updateProjectionMatrix();
    }

    void moveForward(float dt) {
        Vec4 forward = (center - eye).normalize();
        forward.y = 0.0f;
        forward = forward.normalize();

        eye += forward * speed * dt;
        center += forward * speed * dt;
        updateViewMatrix();
    }

    void moveBackward(float dt) {
        Vec4 forward = (center - eye).normalize();
        forward.y = 0.0f;
        forward = forward.normalize();

        eye -= forward * speed * dt;
        center -= forward * speed * dt;
        updateViewMatrix();
    }

    void moveLeft(float dt) {
        Vec4 forward = (center - eye).normalize();
        Vec4 right = up.Cross(forward).normalize();
        eye += right * speed * dt;
        center += right * speed * dt;
        updateViewMatrix();
    }

    void moveRight(float dt) {
        Vec4 forward = (center - eye).normalize();
        Vec4 right = up.Cross(forward).normalize();
        eye -= right * speed * dt;
        center -= right * speed * dt;
        updateViewMatrix();
    }

    void mouseInput(float deltaX, float deltaY) {
        const float sensitivity = 0.004f;
        yaw += deltaX * sensitivity;
        pitch -= deltaY * sensitivity;

        float pitchLimit = PI / 2.0f - 0.01f;
        if (pitch > pitchLimit) pitch = pitchLimit;
        if (pitch < -pitchLimit) pitch = -pitchLimit;

        updateViewMatrix();
    }

    void updateFromKeyboard(const Window& win, float dt) {
        if (win.keys['W']) {
            moveForward(dt);
        }
        if (win.keys['S']) {
            moveBackward(dt);
        }
        if (win.keys['A']) {
            moveLeft(dt);
        }
        if (win.keys['D']) {
            moveRight(dt);
        }
    }

    void updateFromRawInput(float rawDeltaX, float rawDeltaY, float dt) {
        mouseInput(rawDeltaX, rawDeltaY);
    }

    void updateFromMouse(const Window& win, bool& firstMouseInput, float dt) {
        float centerX = win.width / 2.0f;
        float centerY = win.height / 2.0f;

        float deltaX = (float)(win.mousex - centerX);
        float deltaY = (float)(win.mousey - centerY);

        if (!firstMouseInput) {
            mouseInput(deltaX, deltaY);
        }
        else {
            firstMouseInput = false;
        }

        const_cast<Window&>(win).centerCursor();
    }



};