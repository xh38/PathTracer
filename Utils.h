#pragma once
#include <random>
constexpr float eps = 1e-6;
constexpr float pi = 3.1415926535897932385;

inline bool float_equal(const float a, const float b) {
	const float dif = (a > b) ? (a - b) : (b - a);
	return dif < eps;
}

inline float degrees_to_radians(float degrees) {
	return degrees * pi / 180.0f;
}

inline bool russian_roulette(const float p) {
	const float survival = rand() / static_cast<float>(RAND_MAX);
	return survival > p;
}

inline float get_rand_float(){
	static std::default_random_engine e(time(nullptr));
	static std::uniform_real_distribution<float> dist(0, 1);
	return dist(e);
}

inline void UpdateProgress(int h)
{
    static float progress = 0.0f;
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0f) << " %\r";
    std::cout.flush();
    progress += 1.0 / h;
};