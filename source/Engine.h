#pragma once

#include <comet.pch>

#include "handlers/EntityHandler.h"

#include "input/Input.h"

#include "render/ShaderProgram.h"
#include "render/Texture.h"
#include "render/TextureMap.h"

#include "Renderer.h"
#include "Camera.h"
#include "Timer.h"

class Engine
{
public:
    inline static auto &Get()
    {
        static Engine instance;
        return instance;
    }

    static void Initialize();
    static void Finalize();

    static void Thread();

private:
    Engine() {}
    Engine(Engine const &);
    void operator=(Engine const &);

    bool m_ShouldClose = false;
    double m_TimeDelta = 0.0; // ms
    double m_TimeLast = 0.0;  // ms

public:
    static bool IsShouldClose() { return Get().m_ShouldClose; }
    static void SetShouldClose(bool ShouldClose) { Get().m_ShouldClose = ShouldClose; }

    static double TimeDelta() { return Get().m_TimeDelta; }
    static double TimeLast() { return Get().m_TimeLast; }
};
