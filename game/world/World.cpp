#include "World.h"

World::World()
{
    std::cout << "World::World()" << std::endl;
    int num_threads = std::thread::hardware_concurrency();
    m_ThreadPool.resize(num_threads / 2);
}

World::~World()
{
    std::cout << "World::~World()" << std::endl;
}

void World::GenerateChunk(const glm::ivec3 &index)
{
    Chunk *chunk = new Chunk(index);
    m_ChunkMap.insert_or_assign(index, chunk);
}

void World::AddChunkToRenderQueue(const glm::ivec3 &index)
{
    Renderer::AddMesh(index, m_ChunkMap.at(index)->GetMesh());
}

void World::AddShaderToChunk(const glm::ivec3 &index, unsigned int shader)
{
    m_ChunkMap.at(index)->GetMesh()->SetShaderID(shader);
}

void World::ProcessRequestedChunks(const std::unordered_set<glm::ivec3> &requestedChunkIndices)
{
    std::unordered_set<glm::ivec3> chunksToRemove;

    // Loops through the currently loaded chunks and checks if they are
    // present in the requested chunks. If not, added to the chunksToRemove set.
    // Removes from the rendering mesh queue as well. Removing from the map
    // while looping over it is bad.
    for (const auto &oldChunk : m_ChunkMap)
    {
        if (requestedChunkIndices.find(oldChunk.first) == requestedChunkIndices.end())
        {
            // old chunk not found in requested chunks, remove remove queue and map
            Renderer::RemoveMesh(oldChunk.first);
            chunksToRemove.insert(oldChunk.first);
        }
    }

    // Loops through the set of chunksToRemove set and removes them from the
    // currently loaded chunks in the world
    for (const auto &chunk : chunksToRemove)
    {
        // Necessary to delete chunk from heap and map, otherwise memory leak occurs
        delete m_ChunkMap.at(chunk);
        m_ChunkMap.erase(chunk);
    }

    std::vector<std::thread> threads;

    for (const auto &newChunkIndex : requestedChunkIndices)
    {
        if (m_ChunkMap.find(newChunkIndex) == m_ChunkMap.end())
        {
            // new chunk not found in old chunks, add to render queue

            // std::thread thread(&World::GenerateChunk, this, newChunkIndex);
            // thread.join();

            GenerateChunk(newChunkIndex);
            AddChunkToRenderQueue(newChunkIndex);
            AddShaderToChunk(newChunkIndex, 1); // TODO: add shader properly
        }
    }
}