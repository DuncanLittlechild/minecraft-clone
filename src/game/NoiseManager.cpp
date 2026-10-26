#include "NoiseManager.h"
#include "FastNoiseSIMD.h"

void NoiseManager::Init(unsigned long& seed)
{
    worldHeightNoise->SetSeed(seed++);
    worldHeightNoise->SetNoiseType(FastNoiseSIMD::NoiseType::SimplexFractal);
    worldHeightNoise->SetFractalOctaves(1);
    worldHeightNoise->SetFrequency(0.02);
    
}