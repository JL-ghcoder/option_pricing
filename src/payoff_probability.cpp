#include "payoff_probability.h"
#include "MonteCarloBarrierOption.h"
#include <random>
#include <cmath>

double PayoffProbability::calculateProbability(
    double S0, double K, double B, double T, double r, 
    double sigma, double div, double threshold, int numPaths) {
    
    int numSteps = 252;
    double dt = T / numSteps;

    // 调整漂移项，考虑股息率
    double drift = (r - div - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * sqrt(dt);
    
    std::random_device rd;
    std::mt19937 gen(rd()); // 随机数生成器
    std::normal_distribution<> norm_dist(0.0, 1.0); // 正态分布随机数Z
    
    int hits = 0;
    
    for (int path = 0; path < numPaths; ++path) {
        double St = S0;
        double maxS = S0;
        
        for (int step = 0; step < numSteps; ++step) {
            double Z = norm_dist(gen);
            St *= exp(drift + diffusion * Z);
            maxS = std::max(maxS, St);
        }
        
        if (maxS < B && (St - K) > threshold) {
            hits++;
        }
    }
    
    return static_cast<double>(hits) / numPaths;
}