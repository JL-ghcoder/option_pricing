#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "MonteCarloBarrierOption.h"
#include "utils.h"
#include "finite_difference_pricer.h"
#include "binomial_pricer.h"
#include "sensitivity_analysis.h"
#include "payoff_probability.h"
#include "option_tester.h"

namespace py = pybind11;

PYBIND11_MODULE(option_pricing, m) {
    m.doc() = "Option pricing library implemented in C++"; // 模块文档

    // 绑定 MonteCarloBarrierOption
    py::class_<MonteCarloBarrierOption>(m, "MonteCarloBarrierOption")
        .def_static("price", &MonteCarloBarrierOption::mainPricer);

    // 绑定 FiniteDifferencePricer
    py::class_<FiniteDifferencePricer>(m, "FiniteDifferencePricer")
        .def_static("price_put", &FiniteDifferencePricer::priceEuroPutOption)
        .def_static("price_call", &FiniteDifferencePricer::priceEuroCallOption);

    // 绑定 BinomialPricer 的静态方法
    py::class_<BinomialPricer>(m, "BinomialPricer")
        .def_static("price_call", &BinomialPricer::priceEuroCall)
        .def_static("price_put", &BinomialPricer::priceEuroPut);

    // 绑定 PayoffProbability
    py::class_<PayoffProbability>(m, "PayoffProbability")
        .def_static("calculate", &PayoffProbability::calculateProbability);

    // 绑定 PathGenerator
    py::class_<PathGenerator>(m, "PathGenerator")
        .def_static("generate_paths", &PathGenerator::generatePaths);

    // 绑定 SensitivityTest
    py::class_<SensitivityTest>(m, "SensitivityTest")
        .def_static("analyze", &SensitivityTest::sensitivityAnalysis);

    // 绑定 OptionTester
    py::class_<OptionTester>(m, "OptionTester")
        .def(py::init<double, double, double, double, double, double, double>(),
             py::arg("S0") = 90.0,
             py::arg("K") = 100.0,
             py::arg("B") = 120.0,
             py::arg("T") = 1.0,
             py::arg("r") = 0.05,
             py::arg("sigma") = 0.2,
             py::arg("div") = 0.0)
        .def("run_all_tests", &OptionTester::runAllTests)
        .def("test_monte_carlo", &OptionTester::testMonteCarloBarrier)
        .def("test_finite_difference", &OptionTester::testFiniteDifference)
        .def("test_binomial", &OptionTester::testBinomialTree)
        .def("test_sensitivity", &OptionTester::testSensitivityAnalysis)
        .def("test_probability", &OptionTester::testPayoffProbability)
        .def("generate_paths", &OptionTester::generatePricePaths);
}