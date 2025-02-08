#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <random>

class PathGenerator {
public:
    static void generatePaths(double S0, double K, double B, double T, double r, 
                            double sigma, double div, const std::string& csvPath = "paths_data.csv", int numPaths = 10);
};

#endif