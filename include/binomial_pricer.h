#ifndef BINOMIAL_PRICER_H
#define BINOMIAL_PRICER_H

#include <vector>

class BinomialPricer {
private:
    static double maxi(double a, double b);

public:
    // 欧式看涨期权定价
    static double priceEuroCall(
        double S0,        // 初始价格
        double K,         // 执行价格
        double r,         // 无风险利率
        double vol,       // 波动率
        double T,         // 到期时间
        double div = 0.0, // 股息率
        int steps = 100   // 步数
    );
    
    // 欧式看跌期权定价
    static double priceEuroPut(
        double S0,        // 初始价格
        double K,         // 执行价格
        double r,         // 无风险利率
        double vol,       // 波动率
        double T,         // 到期时间
        double div = 0.0, // 股息率
        int steps = 100   // 步数
    );
    
    // 通用二叉树定价方法
    static double btPricer(
        double S0, double K, double r, double vol,
        double T, double div, int steps, bool isCall
    );
};

#endif