#pragma once
#include <random>
constexpr double eps = 1e-6;
constexpr double pi = 3.1415926535897932385;

inline bool double_equal(const double a, const double b) {
	const double dif = (a > b) ? (a - b) : (b - a);
	return dif < eps;
}

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline bool russian_roulette(const double p) {
	const double survival = rand() / static_cast<double>(RAND_MAX);
	return survival > p;
}

inline double get_rand_double(){
	static std::default_random_engine e(time(nullptr));
	static std::uniform_real_distribution<double> dist(0, 1);
	return dist(e);
}

inline void UpdateProgress(int h)
{
    static int progress = 0;
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress / h;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0 / h) << " %\r";
	std::cout.flush();
    progress += 1;
};