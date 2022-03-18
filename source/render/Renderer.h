#pragma once

#include "render/containers/Mesh.h"
#include "render/containers/Shader.h"

class Renderer
{
public:
    static Renderer &GetInstance()
    {
        static Renderer s_Instance;
        return s_Instance;
    }

    static void NewFrame();
    static void SwapBuffers();
    static void DrawMeshQueue() { GetInstance().DrawMeshQueueFunction(); }

    void AddToMeshQueue(Mesh *mesh);

private:
    Renderer() {}
    Renderer(Renderer const &);
    void operator=(Renderer const &);

    void DrawMeshQueueFunction();

    std::vector<Mesh *> m_MeshQueue;
};