#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_NOISEMANAGER_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_NOISEMANAGER_H
#include "FastNoiseSIMD.h"
#include "core/global_consts.h"
#include <memory>


class NoiseManager {
public:
    NoiseManager()
        : worldHeightNoise{FastNoiseSIMD::NewFastNoiseSIMD()}
    {}

    std::unique_ptr<FastNoiseSIMD> worldHeightNoise;

    // Initialises fastnoiseSIMD generators
    void Init(unsigned long& seed);
    // Sets the provided buffer to the noisemap of the provided chunk
    // Buffer must be float[CHUNKX*CHUNKY]
    void SetBufferToChunkSIMDMap(float* buffer, int xStart, int yStart);
};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_NOISEMANAGER_H
