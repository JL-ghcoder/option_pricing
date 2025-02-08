#include "MonteCarloBarrierOption.h"
#include "utils.h"
#include "sensitivity_analysis.h"
#include "payoff_probability.h"
#include "finite_difference_pricer.h"
#include "binomial_pricer.h"
#include "option_tester.h"
#include <iostream>

int main() {
    
    // 测试
    // OptionTester tester;
    // tester.runAllTests();
    
    // 也可以自定义参数
    OptionTester custom_tester(
        100.0,  // S0
        100.0,  // K
        120.0,  // B
        1.0,    // T
        0.05,   // r
        0.2,    // sigma
        0.0     // div
    );
    // custom_tester.testMonteCarloBarrier(); // 蒙特卡罗定价
    // custom_tester.testSensitivityAnalysis(); // 敏感性分析
    // custom_tester.testPayoffProbability(); // 赔付概率
    // custom_tester.testFiniteDifference(); // 有限差分法定价
    // custom_tester.testBinomialTree(); // 二叉树定价
    custom_tester.generatePricePaths(); 

    return 0;
}