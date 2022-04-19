#include "Renderer.h"

void Renderer::Initialize()
{
    // Creates the static instance in memory
    Instance();

    glEnable(GL_DEPTH_TEST);

    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::NewFrame()
{
    // Switch between triangle rendering modes with keys L and N
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_L) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_N) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Clearing the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Background color
    glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 0.0f);
}

void Renderer::SwapBuffers() { glfwSwapBuffers(WindowHandler::GetGLFWWindow()); }

void Renderer::DrawMeshQueue()
{
    unsigned int shaderID;

    ProcessMeshQueues();

    // Binding the texture map
    glBindTexture(GL_TEXTURE_2D, 1);

    // Solid Geometry Loop
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    unsigned int solidMeshCount = 0;
    unsigned int transparentMeshCount = 0;

    for (auto &[index, mesh] : Instance().m_MeshMap)
    {
        if (index.y == 1)
            continue;

        solidMeshCount++;

        shaderID = mesh.Shader()->GetID();

        mesh.Update();

        // Binding the next mesh in queue
        mesh.Bind();

        // Uniforms
        glUniform3iv(glGetUniformLocation(shaderID, "u_Index"), 1, &index[0]);
        glUniform1f(glGetUniformLocation(shaderID, "u_Transparency"), mesh.Transparency());
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ModelMatrix"), 1, GL_FALSE, &mesh.ModelMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ViewMatrix"), 1, GL_FALSE, &Camera::ViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ProjMatrix"), 1, GL_FALSE, &Camera::ProjMatrix()[0][0]);
        glUniform1i(glGetUniformLocation(shaderID, "u_Texture"), 0);

        // Drawing mesh
        glDrawElements(GL_TRIANGLES, mesh.Count(), GL_UNSIGNED_INT, (void *)0);
    }

    // Transparent Geometry Loop
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    for (auto &[index, mesh] : Instance().m_MeshMap)
    {
        if (index.y == 0)
            continue;

        transparentMeshCount++;

        shaderID = mesh.Shader()->GetID();

        mesh.Update();

        // Binding the next mesh in queue
        mesh.Bind();

        // Uniforms
        glUniform3iv(glGetUniformLocation(shaderID, "u_Index"), 1, &index[0]);
        glUniform1f(glGetUniformLocation(shaderID, "u_Transparency"), mesh.Transparency());
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ModelMatrix"), 1, GL_FALSE, &mesh.ModelMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ViewMatrix"), 1, GL_FALSE, &Camera::ViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "u_ProjMatrix"), 1, GL_FALSE, &Camera::ProjMatrix()[0][0]);
        glUniform1i(glGetUniformLocation(shaderID, "u_Texture"), 0);

        // Drawing mesh
        glDrawElements(GL_TRIANGLES, mesh.Count(), GL_UNSIGNED_INT, (void *)0);
    }
}

void Renderer::ProcessMeshQueues()
{
    {
        std::lock_guard<std::mutex> locked(Instance().m_AddMeshQueueLock);
        for (const auto &[index, mesh] : Instance().m_MeshesToAdd)
        {
            Instance().m_MeshMap.insert_or_assign(index, mesh);
            Instance().m_MeshMap.at(index).Initialize();
        }
        Instance().m_MeshesToAdd.clear();
    }

    {
        std::lock_guard<std::mutex> locked(Instance().m_UpdateMeshQueueLock);
        for (const auto &index : Instance().m_MeshesToUpdate)
        {
            Instance().m_MeshMap.at(index).UpdateGeometry();
        }
        Instance().m_MeshesToUpdate.clear();
    }

    {
        std::lock_guard<std::mutex> locked(Instance().m_DeleteMeshQueueLock);
        for (const auto &index : Instance().m_MeshesToDelete)
        {
            if (Instance().m_MeshMap.find(index) != Instance().m_MeshMap.end())
            {
                Instance().m_MeshMap.at(index).Finalize();
                Instance().m_MeshMap.erase(index);
            }
        }
        Instance().m_MeshesToDelete.clear();
    }
}

void Renderer::AddMeshToQueue(const glm::ivec3 &index, const Mesh &mesh)
{
    std::lock_guard<std::mutex> locked(Instance().m_AddMeshQueueLock);
    Instance().m_MeshesToAdd.insert_or_assign(index, mesh);
}

void Renderer::UpdateMeshInQueue(const glm::ivec3 &index)
{
    std::lock_guard<std::mutex> locked(Instance().m_UpdateMeshQueueLock);
    Instance().m_MeshesToUpdate.insert(index);
}

void Renderer::DeleteMeshFromQueue(const glm::ivec3 &index)
{
    std::lock_guard<std::mutex> locked(Instance().m_DeleteMeshQueueLock);
    Instance().m_MeshesToDelete.insert(index);
}