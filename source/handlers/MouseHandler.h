#pragma once

#include <comet.pch>

#include "Engine.h"
#include "WindowHandler.h"

class MouseHandler
{
public:
    inline static auto &Get()
    {
        static MouseHandler instance;
        return instance;
    }

    static void Initialize();
    static void SetupCallbacks();
    static void ResetMovement();

    // +x is moving the mouse right
    // +y is moving the mouse forward
    static void CaptureCursor();

private:
    MouseHandler() {}
    MouseHandler(MouseHandler const &);
    void operator=(MouseHandler const &) {}

    GLFWwindow *p_GLFWwindow = nullptr;

    std::array<double, 2> m_MovementSinceLastFrame = {0.0, 0.0};
    bool m_CursorCaptured = false;
    bool m_LeftClick = false;
    bool m_RightClick = false;

    double m_ScrollOffset = 0.0;

public:
    static double ScrollOffset() { return Get().m_ScrollOffset; }
    static double DeltaX() { return Get().m_MovementSinceLastFrame[0]; }
    static double DeltaY() { return Get().m_MovementSinceLastFrame[1]; }
    static bool LeftClick() { return Get().m_LeftClick; }
    static bool RightClick() { return Get().m_RightClick; }
};
