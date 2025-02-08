#include "utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

void PathGenerator::generatePaths(double S0, double K, double B, double T, double r, 
                            double sigma, double div, const std::string& csvPath, int numPaths) {
    int numSteps = 252;
    double dt = T / numSteps;
    double drift = (r - div - 0.5 * sigma * sigma) * dt;
    double diffusion = sigma * sqrt(dt);

    // 创建目录（如果不存在）
    std::filesystem::path filePath(csvPath);
    if (auto dir = filePath.parent_path(); !dir.empty()) {
        std::filesystem::create_directories(dir);
    }
    std::ofstream outFile(csvPath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file: " << csvPath << std::endl;
        return;
    }
    
    outFile << "Step";
    for (int i = 1; i <= numPaths; i++) {
        outFile << ",Path" << i;
    }
    outFile << ",Barrier,Strike\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> norm_dist(0.0, 1.0);

    std::vector<double> currentPrices(numPaths, S0);
    
    for (int step = 0; step <= numSteps; step++) {
        outFile << step;
        
        for (int path = 0; path < numPaths; path++) {
            if (step > 0) {
                double Z = norm_dist(gen);
                currentPrices[path] *= exp(drift + diffusion * Z);
            }
            outFile << "," << currentPrices[path];
        }
        
        outFile << "," << B << "," << K << "\n";
    }
    
    outFile.close();
    std::cout << "Paths have been saved to '" << csvPath << "'" << std::endl;
}