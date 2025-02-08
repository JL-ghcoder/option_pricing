#ifndef FINITE_DIFFERENCE_PUT_H
#define FINITE_DIFFERENCE_PUT_H

#include <vector>

class FiniteDifferencePricer {
private:
    // 插值法
    static double interpolatePrice(const std::vector<double>& grid, 
                          const std::vector<double>& values, 
                          double spot);
    static double maxi(double a, double b);

public:
    // Main pricing function for European put option
    static double priceEuroPutOption(double spot, double strike, 
                             double r, double vol, double T, double div = 0,
                             int gridSize = 100);

    static double priceEuroCallOption(double spot, double strike, 
                             double r, double vol, double T, double div = 0,
                             int gridSize = 100);
};

#endif