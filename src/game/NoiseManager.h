#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_NOISEMANAGER_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_NOISEMANAGER_H
#include "FastNoiseSIMD.h"
#include <memory>


class NoiseManager {
public:
    NoiseManager()
        : worldHeightNoise{FastNoiseSIMD::NewFastNoiseSIMD()}
    {}

    std::unique_ptr<FastNoiseSIMD> worldHeightNoise;

    void Init(unsigned long& seed);
    void GetChunkSIMDMap
};

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_NOISEMANAGER_H
