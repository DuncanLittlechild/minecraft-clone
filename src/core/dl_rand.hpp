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
/*
    inline std::ranlux24_base rnd;
    inline unsigned long g_seed;

    inline void Init(SeedType seedType, unsigned long seed = 0)
    {
        std::ranlux24_base var;
        switch(seedType){
            case(SET): {
                var = seed;
            } break;
            case(MEDRAND): {
                var = static_cast<std::ranlux24_base::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
            } break;
        }
        rnd = std::move(var);
    }*/

    	// Returns a seeded Mersenne Twister
	// Note: we'd prefer to return a std::seed_seq (to initialize a std::mt19937), but std::seed can't be copied, so it can't be returned by value.
	// Instead, we'll create a std::mt19937, seed it, and then return the std::mt19937 (which can be copied).
	inline std::mt19937 generate()
	{
		std::random_device rd{};

		// Create seed_seq with clock and 7 random numbers from std::random_device
		std::seed_seq ss{
			static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),
				rd(), rd(), rd(), rd(), rd(), rd(), rd() };

		return std::mt19937{ ss };
	}

	// Here's our global std::mt19937 object.
	// The inline keyword means we only have one global instance for our whole program.
	inline std::mt19937 rnd{ generate() }; // generates a seeded std::mt19937 and copies it into our global object

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
