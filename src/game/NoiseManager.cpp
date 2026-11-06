#include "NoiseManager.h"
#include "FastNoiseSIMD.h"

void NoiseManager::Init(unsigned long& seed)
{
    worldHeightNoise->SetSeed(seed++);
    worldHeightNoise->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    worldHeightNoise->SetFractalOctaves(1);
    worldHeightNoise->SetFrequency(0.02);
}

void NoiseManager::SetBufferToChunkSIMDMap(float* buffer, int xStart, int yStart)
{
    worldHeightNoise->FillNoiseSet(buffer, xStart, yStart, 0, CHUNKX, CHUNKY, 1);
    for (int i {0}; i < CHUNKX * CHUNKY; ++i)
    {
        buffer[i] = (buffer[i] + 1) / 2;
    }
}