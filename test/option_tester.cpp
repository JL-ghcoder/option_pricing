#include "option_tester.h"
#include "MonteCarloBarrierOption.h"
#include "finite_difference_pricer.h"
#include "binomial_pricer.h"
#include "sensitivity_analysis.h"
#include "payoff_probability.h"
#include "utils.h"
#include <iostream>
#include <iomanip>

OptionTester::OptionTester(double S0, double K, double B, double T, 
                         double r, double sigma, double div)
    : params{S0, K, B, T, r, sigma, div} {}

void OptionTester::printLine(const std::string& method, double price) const {
    std::cout << std::setw(25) << std::left << method 
              << "$ " << std::fixed << std::setprecision(4) << price << std::endl;
}

void OptionTester::printSeparator() const {
    std::cout << std::string(40, '-') << std::endl;
}

void OptionTester::runAllTests() {
    std::cout << "\n===== Option Pricing Test Results =====\n\n";
    
    std::cout << "Parameters:\n";
    std::cout << "Initial Price (S0): " << params.S0 << "\n";
    std::cout << "Strike Price (K): " << params.K << "\n";
    std::cout << "Barrier Level (B): " << params.B << "\n";
    std::cout << "Time to Maturity (T): " << params.T << "\n";
    std::cout << "Risk-free Rate (r): " << params.r << "\n";
    std::cout << "Volatility (σ): " << params.sigma << "\n";
    std::cout << "Dividend Rate: " << params.div << "\n";
    printSeparator();

    testMonteCarloBarrier();
    testFiniteDifference();
    testBinomialTree();
    testSensitivityAnalysis();
    testPayoffProbability();
    generatePricePaths();
}

void OptionTester::testMonteCarloBarrier() {
    std::cout << "\n=== Monte Carlo Barrier Option ===\n";
    double price = MonteCarloBarrierOption::mainPricer(params.S0, params.K, params.B, params.T,
                                  params.r, params.sigma, params.div);
    printLine("Monte Carlo Price:", price);
}

void OptionTester::testFiniteDifference() {
    std::cout << "\n=== Finite Difference Method ===\n";
    double price = FiniteDifferencePricer::priceEuroPutOption(params.S0, params.K, params.r, 
                                           params.sigma, params.T, params.div);
    printLine("Finite Difference Price:", price);
}

void OptionTester::testBinomialTree() {
    std::cout << "\n=== Binomial Tree Method ===\n";
    double call_price = BinomialPricer::priceEuroCall(params.S0, params.K, 
                                                     params.r, params.sigma, 
                                                     params.T, params.div, 100);
    double put_price = BinomialPricer::priceEuroPut(params.S0, params.K, 
                                                   params.r, params.sigma, 
                                                   params.T, params.div, 100);
    printLine("Call Option Price:", call_price);
    printLine("Put Option Price:", put_price);
}

void OptionTester::testSensitivityAnalysis() {
    std::cout << "\n=== Sensitivity Analysis ===\n";
    auto results = SensitivityTest::sensitivityAnalysis(
        params.S0, params.K, params.B, params.T, params.r, params.div);
    
    std::cout << "Volatility | Option Price\n";
    std::cout << "-----------+-------------\n";
    for (const auto& [vol, price] : results) {
        printf("%9.2f%% | %11.4f\n", vol * 100, price);
    }
}

void OptionTester::testPayoffProbability() {
    std::cout << "\n=== Payoff Probability Analysis ===\n";
    double threshold = 5.0;
    double prob = PayoffProbability::calculateProbability(
        params.S0, params.K, params.B, params.T, params.r, params.sigma, params.div, threshold);
    std::cout << "Probability of payoff > $" << threshold << ": " 
              << (prob * 100) << "%" << std::endl;
}

void OptionTester::generatePricePaths() {
    std::cout << "\n=== Generating Price Paths ===\n";
    PathGenerator::generatePaths(params.S0, params.K, params.B, params.T,
                               params.r, params.sigma, params.div, "GeneratedPaths.csv", 5);
    std::cout << "Price paths have been generated and saved to file.\n";
}