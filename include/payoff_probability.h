#ifndef PAYOFF_PROBABILITY_H
#define PAYOFF_PROBABILITY_H

class PayoffProbability {
public:
    static double calculateProbability(
        double S0,        // Initial price
        double K,         // Strike price
        double B,         // Barrier price
        double T,         // Time to maturity
        double r,         // Risk-free rate
        double sigma,     // Volatility
        double div,       // Dividend yield
        double threshold, // Payoff threshold
        int numPaths = 100000
    );
};

#endif