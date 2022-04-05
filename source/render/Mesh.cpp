#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(std::vector<Vertex> *vertices, std::vector<unsigned int> *indices, const Shader &shader)
    : p_Vertices(vertices),
      p_Indices(indices),
      m_Shader(shader),
      m_Count(static_cast<unsigned int>(indices->size())),
      m_PushedToGPU(false),
      m_ModelMatrix(1.0f)
{
    m_TimeCreated = glfwGetTime();
}

Mesh::~Mesh()
{
}

void Mesh::Bind()
{
    glUseProgram(m_Shader.GetID());
    glBindVertexArray(m_VAO);
}

void Mesh::Unbind()
{
    glUseProgram(0);
    glBindVertexArray(0);
}

void Mesh::Initialize()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, 500000 * sizeof(Vertex), (void *)0, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, p_Vertices->size() * sizeof(Vertex), p_Vertices->data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 500000 * sizeof(unsigned int), (void *)0, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, p_Indices->size() * sizeof(unsigned int), p_Indices->data());

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, m_Position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, m_TextureCoordinate)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, m_Normal)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_PushedToGPU = true;
}

void Mesh::Update()
{
    m_TimeDelta = glfwGetTime() - m_TimeCreated;

    if (m_TimeDelta < 1.0)
    {
        m_ModelMatrix = glm::translate(
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, 0.0f)),
            glm::vec3(0.0f, static_cast<float>(50.0 * glm::sin(3.141592653589 / 2 * m_TimeDelta)), 0.0f));
        m_Transparency = m_TimeDelta;
    }
    else
    {
        m_ModelMatrix = glm::mat4(1.0f);
        m_Transparency = 1.0;
    }
}

void Mesh::UpdateGeometry()
{
    m_Count = static_cast<unsigned int>(p_Indices->size());

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, p_Vertices->size() * sizeof(Vertex), p_Vertices->data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, p_Indices->size() * sizeof(unsigned int), p_Indices->data());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Finalize()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);

    m_PushedToGPU = false;
}