#ifndef MONTECARLOBARRIEROPTION_H
#define MONTECARLOBARRIEROPTION_H

#include <random>

class MonteCarloBarrierOption {
public:
    static double mainPricer(
        double S0,        // 初始价格
        double K,         // 执行价格
        double B,         // 障碍价格
        double T,         // 到期时间
        double r,         // 无风险利率
        double sigma,     // 波动率
        double div,       // 股息率
        int numSteps = 252,   // 时间步数
        int numPaths = 10000  // 路径数
    );
};

#endif