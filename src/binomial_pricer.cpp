#include "binomial_pricer.h"
#include <cmath>

double BinomialPricer::maxi(double a, double b) {
    return (a > b) ? a : b;
}

double BinomialPricer::btPricer(double S0, double K, double r, double vol,
                               double T, double div, int n, bool isCall) {
    // 计算时间步长和参数
    double dt = T/n;
    // 调整漂移项，考虑股息
    double nu = (r - div) - 0.5 * vol * vol;
    double dxi = sqrt(vol * vol * dt + nu * nu * dt * dt);
    // 调整风险中性概率
    double p = 0.5 + 0.5 * nu * dt / dxi;
    
    // 分配存储空间
    std::vector<double> St(n + 1);  // 股票价格
    std::vector<double> V(n + 1);   // 期权价值
    
    // 构建股票价格树的最后一层
    St[0] = S0 * exp(-n * dxi);
    for (int i = 1; i <= n; i++) {
        St[i] = St[i-1] * exp(2 * dxi);
    }
    
    // 在到期时计算期权价值
    for (int i = 0; i <= n; i++) {
        if (isCall) {
            V[i] = maxi(St[i] - K, 0.0);  // 看涨期权支付
        } else {
            V[i] = maxi(K - St[i], 0.0);  // 看跌期权支付
        }
    }
    
    // 向后归纳计算价值
    for (int j = n-1; j >= 0; j--) {
        for (int i = 0; i <= j; i++) {
            // 使用风险中性概率计算期望值
            V[i] = exp(-r * dt) * (p * V[i+1] + (1-p) * V[i]);
        }
    }
    
    return V[0];  // 返回当前期权价值
}

double BinomialPricer::priceEuroCall(double S0, double K, double r, 
                                    double vol, double T, double div, int steps) {
    return btPricer(S0, K, r, vol, T, div, steps, true);
}

double BinomialPricer::priceEuroPut(double S0, double K, double r, 
                                   double vol, double T, double div, int steps) {
    return btPricer(S0, K, r, vol, T, div, steps, false);
}