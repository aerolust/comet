#include "comet/Engine.h"
#include "Renderer.h"

#include "containers/Shader.h"

#include "world/World.h"
#include "world/Chunk.h"
#include "world/Block.h"

#include "entities/Player.h"

int main(void)
{
    Engine::Initialize();

    char vertexshader[] = "../source/render/shaders/basic.vert";
    char fragshader[] = "../source/render/shaders/basic.frag";
    Shader basic(vertexshader, fragshader);

    {
        Timer timer("chunk generation");

        World world;
        for (int x = -4; x < 4; x++)
        {
            for (int z = -4; z < 4; z++)
            { // Generating a chunk
                world.GenerateChunk(x, 0, z);
                world.AddChunkToRenderQueue(x, 0, z);
                world.AddShaderToChunk(x, 0, z, basic.GetID());
            }
        }
    }

    Player player;

    Engine::MainLoop();

    return 0;
}