#ifndef DL_SDL3GPU_PROJECT_TEMPLATE_DL_RAND_H
#define DL_SDL3GPU_PROJECT_TEMPLATE_DL_RAND_H
#include <random>
#include <chrono>
namespace DL_Rand {
    enum SeedType{
        SET,
        MEDRAND,
        MAX_SEEDTYPE
    };

    inline std::ranlux24_base rnd;
    inline unsigned long g_seed;

    inline void Init(SeedType seedType, unsigned long seed = 0)
    {
        std::ranlux_base var;
        switch(seedType):{
            case(SET): {
                var = seed;
            } break;
            case(MEDRAND): {
                var = static_cast<std::ranlux24_base::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
            } break;
        }
        rnd = std::move(var);
    }

    // Basic random number generator, not suitable for circumstances where randomness is critical
    template <typename T>
    T GetIntInRange(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(rnd);
    }

    // Basic random number generator, not suitable for circumstances where randomness is critical
    template <typename T>
    T GetRealInRange(T min, T max)
    {
        return std::uniform_real_distribution<T>{min, max}(rnd);
    }
}

#endif // DL_SDL3GPU_PROJECT_TEMPLATE_DL_RAND_H
