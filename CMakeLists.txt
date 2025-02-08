cmake_minimum_required(VERSION 3.10)  
project(CFCoursework)                    

set(CMAKE_CXX_STANDARD 17)  
find_package(Python 3.10.16 COMPONENTS Interpreter Development REQUIRED)

# anaconda环境
set(PYTHON_EXECUTABLE "/opt/anaconda3/envs/crypto_pricing/bin/python")
set(PYTHON_INCLUDE_DIR "/opt/anaconda3/envs/crypto_pricing/include/python3.10")

add_subdirectory(external/pybind11)

add_executable(CFCoursework
    binding.cpp
    src/main.cpp 
    src/MonteCarloBarrierOption.cpp 
    src/utils.cpp
    src/sensitivity_analysis.cpp
    src/payoff_probability.cpp
    src/finite_difference_pricer.cpp
    src/binomial_pricer.cpp
    test/option_tester.cpp
)

# 创建 Python 模块
pybind11_add_module(option_pricing 
    binding.cpp
    src/MonteCarloBarrierOption.cpp 
    src/utils.cpp
    src/sensitivity_analysis.cpp
    src/payoff_probability.cpp
    src/finite_difference_pricer.cpp
    src/binomial_pricer.cpp
    test/option_tester.cpp
)

# c++
target_include_directories(CFCoursework PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/external/pybind11/include
    ${PYTHON_INCLUDE_DIR}
)

# python
target_include_directories(option_pricing PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/external/pybind11/include
    ${PYTHON_INCLUDE_DIR}
)

# 链接 Python 库
target_link_libraries(CFCoursework PRIVATE Python::Python)