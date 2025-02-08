#ifndef SENSITIVITY_ANALYSIS_H
#define SENSITIVITY_ANALYSIS_H

#include <vector>
#include <utility>

class SensitivityTest {
// numPoints决定sigma的步长，numPaths决定路径数
public:
    // 这个方法用于接口
    static std::vector<std::pair<double, double>> sensitivityAnalysis(
        double S0, double K, double B, double T, double r, double div = 0.0,
        double volStart = 0.1, double volEnd = 0.4, int numPoints = 10,
        int numPaths = 10000);
    
    static std::vector<std::pair<double, double>> sensitivityAnalysisWithCSV(
        double S0, double K, double B, double T, double r, double div = 0.0,
        const std::string& csvPath = "sensitivity_results.csv",
        double volStart = 0.1, double volEnd = 0.4, 
        int numPoints = 10, int numPaths = 10000);
};

#endif