#include "MonteCarloBarrierOption.h"
#include <cmath>
#include <random>

// 实现方法
double MonteCarloBarrierOption::mainPricer(
    double S0, double K, double B, double T, 
    double r, double sigma, double div,
    int numSteps, int numPaths) {
    
    // 初始化随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> norm_dist(0.0, 1.0);
    
    double dt = T / numSteps;
    double drift = (r - div - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * sqrt(dt);
    double discount = exp(-r * T);
    
    double sum = 0.0;
    for (int path = 0; path < numPaths; ++path) {
        double St = S0;
        double maxS = S0;
        
        for (int step = 0; step < numSteps; ++step) {
            double Z = norm_dist(gen);
            St *= exp(drift + diffusion * Z);
            maxS = std::max(maxS, St);
        }
        
        double payoff = 0.0;
        if (maxS < B) {
            payoff = std::max(St - K, 0.0);
        }
        sum += payoff;
    }
    return discount * sum / numPaths;
}