#ifndef TESTS_H
#define TESTS_H

#include "NPU.h"
#include <vector>
#include <cstdint>
#include <string>

namespace Tests {
    void testNPU();
    void testMatrixMultiplication(const std::vector<std::vector<int8_t>>& A,
                                 const std::vector<std::vector<int8_t>>& B,
                                 const std::string& testName);
    std::vector<std::vector<int32_t>> computeExpectedResult(
        const std::vector<std::vector<int8_t>>& A,
        const std::vector<std::vector<int8_t>>& B);
    void printMatrix(const std::vector<std::vector<int32_t>>& matrix, 
                    const std::string& name);
    bool compareMatrices(const std::vector<std::vector<int32_t>>& actual,
                        const std::vector<std::vector<int32_t>>& expected);
}

#endif // TESTS_H
