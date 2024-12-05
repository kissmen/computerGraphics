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

    float speed = 0.08f;
    float yaw = 0.0f;
    float pitch = 0.0f;

    Camera(const Vec4& _eye, const Vec4& _center, const Vec4& _up)
        : eye(_eye), center(_center), up(_up) {
        Vec4 forward = (center - eye).normalize();
        yaw = atan2f(forward.z, forward.x);
        pitch = asinf(forward.y);

        updateViewMatrix();
        transform.setProjectionMatrix(60.0f, 1.0f, 0.1f, 1000.0f);
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

    void moveForward() {
        Vec4 forward = (center - eye).normalize();
        forward.y = 0.0f;
        forward = forward.normalize();

        eye += forward * speed;
        center += forward * speed;
        updateViewMatrix();
    }

    void moveBackward() {
        Vec4 forward = (center - eye).normalize();
        forward.y = 0.0f;
        forward = forward.normalize();

        eye -= forward * speed;
        center -= forward * speed;
        updateViewMatrix();
    }

    void moveLeft() {
        Vec4 forward = (center - eye).normalize();
        Vec4 right = up.Cross(forward).normalize();
        eye += right * speed;
        center += right * speed;
        updateViewMatrix();
    }

    void moveRight() {
        Vec4 forward = (center - eye).normalize();
        Vec4 right = up.Cross(forward).normalize();
        eye -= right * speed;
        center -= right * speed;
        updateViewMatrix();
    }

    void processMouseInput(float deltaX, float deltaY) {
        const float sensitivity = 0.0008f; // Adjust as needed
        yaw += deltaX * sensitivity;
        pitch -= deltaY * sensitivity;

        float pitchLimit = PI / 2.0f - 0.01f;
        if (pitch > pitchLimit) pitch = pitchLimit;
        if (pitch < -pitchLimit) pitch = -pitchLimit;

        updateViewMatrix();
    }

    void updateFromKeyboard(const Window& window) {
        if (window.keys['W']) {
            moveForward();
        }
        if (window.keys['S']) {
            moveBackward();
        }
        if (window.keys['A']) {
            moveLeft();
        }
        if (window.keys['D']) {
            moveRight();
        }
    }

    void updateFromMouse(const Window& window, bool& firstMouseInput) {
        float centerX = window.width / 2.0f;
        float centerY = window.height / 2.0f;

        float deltaX = static_cast<float>(window.mousex) - centerX;
        float deltaY = static_cast<float>(window.mousey) - centerY;

        if (!firstMouseInput) {
            processMouseInput(deltaX, deltaY);
        }
        else {
            firstMouseInput = false;
        }

        // Center the cursor
        const_cast<Window&>(window).centerCursor();
    }
};