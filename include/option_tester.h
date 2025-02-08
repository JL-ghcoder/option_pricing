#ifndef OPTION_TESTER_H
#define OPTION_TESTER_H

#include <string>
#include <vector>

class OptionTester {
private:
    // 测试参数结构
    struct TestParams {
        double S0;        // 初始价格
        double K;         // 执行价格
        double B;         // 障碍价格
        double T;         // 到期时间
        double r;        // 无风险利率
        double sigma;    // 波动率
        double div;      // 股息率
    };

    TestParams params;
    void printLine(const std::string& method, double price) const;
    void printSeparator() const;
    
public:
    // 构造函数
    OptionTester(double S0 = 90.0, double K = 100.0, double B = 120.0,
                double T = 1.0, double r = 0.05, double sigma = 0.2, 
                double div = 0.0);

    // 运行所有测试
    void runAllTests();

    // coursework用到的所有方法
    void testMonteCarloBarrier();
    void testFiniteDifference();
    void testBinomialTree();
    void testSensitivityAnalysis();
    void testPayoffProbability();
    void generatePricePaths();
};

#endif