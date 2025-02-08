#include "sensitivity_analysis.h"
#include "MonteCarloBarrierOption.h"
#include <fstream>
#include <iostream>

// 返回一个向量，存储波动率与对应的期权价格的配对
std::vector<std::pair<double, double>> SensitivityTest::sensitivityAnalysis(
    double S0, double K, double B, double T, double r, double div,
    double volStart, double volEnd, int numPoints, int numPaths) {
    
    std::vector<std::pair<double, double>> results; // 配对的结果
    double volStep = (volEnd - volStart) / (numPoints - 1); // 计算波动率的步长，用于确定每次迭代时波动率的变化量
    
    // 循环每一个采样点
    for (int i = 0; i < numPoints; ++i) {
        double currentVol = volStart + i * volStep;
        double price = MonteCarloBarrierOption::mainPricer(S0, K, B, T, r, currentVol, div, 252, numPaths);
        results.push_back({currentVol, price}); // 把结果以列表形式存储
    }
        
    return results;
}

std::vector<std::pair<double, double>> SensitivityTest::sensitivityAnalysisWithCSV(
    double S0, double K, double B, double T, double r, double div,
    const std::string& csvPath,
    double volStart, double volEnd, int numPoints, int numPaths) {
    
    std::vector<std::pair<double, double>> results = sensitivityAnalysis(
        S0, K, B, T, r, div, volStart, volEnd, numPoints, numPaths);
    
    // 保存结果到CSV文件
    std::ofstream outFile(csvPath);
    outFile << "Volatility,OptionPrice\n";
    
    for (const auto& result : results) {
        outFile << result.first << "," << result.second << "\n";
    }
    
    outFile.close();
    std::cout << "Sensitivity analysis results saved to '" << csvPath << "'" << std::endl;
    
    return results;
}