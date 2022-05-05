#pragma once

#include <comet.pch>

#include "handlers/InterfaceHandler.h"
#include "handlers/WindowHandler.h"

#include "render/Mesh.h"
#include "render/ShaderProgram.h"

#include "Camera.h"

struct Lock
{
    std::mutex AddQueue;
    std::mutex UpdateQueue;
    std::mutex DeleteQueue;
};

class Renderer
{
public:
    inline static auto &Get()
    {
        static Renderer instance;
        return instance;
    }

    static Lock QueueLock;

    static void Initialize();
    static void NewFrame();
    static void SwapBuffers();
    static void ResetRenderer();

    static void DrawMeshQueue();
    static void DrawInterfaceQueue();

    // These functions can be called from other threads when adding to the
    // rendering queue that runs on the main thread. For this reason,
    // Each of these functions locks and unlocks their respective map/set
    // before modifying them at all. ProcessMeshQueues() reads from these
    // map/set queues and then clears them, requiring a plain mutex for R/W.
    static void AddMeshToQueue(glm::ivec3 index, const Mesh &mesh);
    static void UpdateMeshInQueue(glm::ivec3 index);
    static void DeleteMeshFromQueue(glm::ivec3 index);
    static void ProcessMeshQueues();

private:
    Renderer() {}
    Renderer(Renderer const &);
    void operator=(Renderer const &);

    bool m_Resetting = false;
    unsigned int m_DrawCallsPerFrame = 0;

    glm::vec3 m_OverlayColor = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_BackgroundColor = {0.0f, 0.0f, 0.0f};

    std::unordered_map<glm::ivec3, Mesh> m_MeshMap;

    // Queues for safe mesh management
    std::unordered_map<glm::ivec3, Mesh> m_MeshesToAdd;
    std::unordered_set<glm::ivec3> m_MeshesToUpdate;
    std::unordered_set<glm::ivec3> m_MeshesToDelete;

public:
    static glm::vec3 OverlayColor() { return Get().m_OverlayColor; }
    static void SetOverlayColor(const glm::vec3 &OverlayColor) { Get().m_OverlayColor = OverlayColor; }

    static glm::vec3 BackgroundColor() { return Get().m_BackgroundColor; }
    static void SetBackgroundColor(const glm::vec3 &BackgroundColor) { Get().m_BackgroundColor = BackgroundColor; }

    static bool IsResetting() { return Get().m_Resetting; }
    static void SetResetting(bool Resetting) { Get().m_Resetting = Resetting; }

    static unsigned int DrawCallsPerFrame() { return Get().m_DrawCallsPerFrame; }
    static void SetDrawCallsPerFrame(unsigned int DrawCallsPerFrame) { Get().m_DrawCallsPerFrame = DrawCallsPerFrame; }
};
