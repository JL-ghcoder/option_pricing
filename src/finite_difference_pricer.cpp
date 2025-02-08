#include "finite_difference_pricer.h"
#include <cmath>
#include <algorithm>

double FiniteDifferencePricer::maxi(double a, double b) {
    return (a > b) ? a : b;
}

// 线性插值方法
double FiniteDifferencePricer::interpolatePrice(
    const std::vector<double>& grid, 
    const std::vector<double>& values, 
    double spot) {
    
    // Find the closest grid points
    auto it = std::lower_bound(grid.begin(), grid.end(), spot);
    if (it == grid.begin()) return values.front();
    if (it == grid.end()) return values.back();
    
    int idx = std::distance(grid.begin(), it);
    double x0 = grid[idx-1];
    double x1 = grid[idx];
    double y0 = values[idx-1];
    double y1 = values[idx];
    
    // Linear interpolation
    return y0 + (spot - x0) * (y1 - y0) / (x1 - x0);
}

// 实现EuroPutOption的定价
double FiniteDifferencePricer::priceEuroPutOption(
    double spot, double strike, double r, double vol, 
    double T, double div, int gridSize) {
    
    // 初始化网络
    const double sMax = 2.0 * strike;  // 上界设为执行价格的2倍
    const double ds = sMax / gridSize;
    
    // Time steps (based on stability condition)
    // 时间步长dt ds的选择必须满足一定的约束，否则数值解可能会不稳定
    double dt = 1.0 / (vol * vol * gridSize * gridSize);
    int timeSteps = static_cast<int>(std::ceil(T / dt));
    dt = T / timeSteps;  // 调整dt以精确匹配T
    
    // Initialize grid and vectors
    std::vector<double> S(gridSize + 1);
    std::vector<double> Vprev(gridSize + 1);
    std::vector<double> Vcurr(gridSize + 1);
    std::vector<double> Gammas(gridSize + 1);
    std::vector<double> Deltas(gridSize + 1);
    
    // Set up price grid
    for (int i = 0; i <= gridSize; ++i) {
        S[i] = i * ds;
    }
    
    // Terminal condition (put option payoff)
    for (int i = 0; i <= gridSize; ++i) {
        Vcurr[i] = std::max(strike - S[i], 0.0);  // Put option payoff
    }
    
    // 时间迭代
    for (int j = timeSteps-1; j >= 0; --j) {
        // Copy current values to previous
        Vprev = Vcurr;
        
        // Interior points
        for (int i = 1; i < gridSize; ++i) {
            // Calculate Greeks
            Gammas[i] = (Vprev[i+1] - 2.0 * Vprev[i] + Vprev[i-1]) / (ds * ds);
            Deltas[i] = (Vprev[i+1] - Vprev[i-1]) / (2.0 * ds);
            
            // Explicit finite difference scheme
            Vcurr[i] = Vprev[i] + 
                      dt * (0.5 * vol * vol * S[i] * S[i] * Gammas[i] +
                           (r - div) * S[i] * Deltas[i] - 
                           r * Vprev[i]);
        }
        
        // 边界条件
        // At S = 0 (put option value = present value of strike)
        Vcurr[0] = strike * std::exp(-r * (T - j*dt));
        // At S = Smax (put option value approaches 0)
        Vcurr[gridSize] = 0.0;
    }
    
    // Interpolate to get the price at the desired spot price
    return interpolatePrice(S, Vcurr, spot);
}

double FiniteDifferencePricer::priceEuroCallOption(
    double spot, double strike, double r, double vol, 
    double T, double div,int gridSize) {
    
    // 初始化网格
    const double sMax = 3.0 * strike;  // 对于看涨期权，上界设得更大一些
    const double ds = sMax / gridSize;
    
    // Time steps (based on stability condition)
    double dt = 1.0 / (vol * vol * gridSize * gridSize);
    int timeSteps = static_cast<int>(std::ceil(T / dt));
    dt = T / timeSteps;  // 调整dt以精确匹配T
    
    // Initialize grid and vectors
    std::vector<double> S(gridSize + 1);
    std::vector<double> Vprev(gridSize + 1);
    std::vector<double> Vcurr(gridSize + 1);
    std::vector<double> Gammas(gridSize + 1);
    std::vector<double> Deltas(gridSize + 1);
    
    // Set up price grid
    for (int i = 0; i <= gridSize; ++i) {
        S[i] = i * ds;
    }
    
    // Terminal condition (call option payoff)
    for (int i = 0; i <= gridSize; ++i) {
        Vcurr[i] = std::max(S[i] - strike, 0.0);  // Call option payoff
    }
    
    // 时间迭代
    for (int j = timeSteps-1; j >= 0; --j) {
        // Copy current values to previous
        Vprev = Vcurr;
        
        // Interior points
        for (int i = 1; i < gridSize; ++i) {
            // Calculate Greeks
            Gammas[i] = (Vprev[i+1] - 2.0 * Vprev[i] + Vprev[i-1]) / (ds * ds);
            Deltas[i] = (Vprev[i+1] - Vprev[i-1]) / (2.0 * ds);
            
            // Explicit finite difference scheme
            Vcurr[i] = Vprev[i] + 
                      dt * (0.5 * vol * vol * S[i] * S[i] * Gammas[i] +
                           (r - div) * S[i] * Deltas[i] - 
                           r * Vprev[i]);
        }
        
        // 边界条件
        // At S = 0 (call option value = 0)
        Vcurr[0] = 0.0;
        // At S = Smax (call option value ≈ S - Ke^(-r(T-t)))
        Vcurr[gridSize] = S[gridSize] - strike * std::exp(-r * (T - j*dt));
    }
    
    // Interpolate to get the price at the desired spot price
    return interpolatePrice(S, Vcurr, spot);
}