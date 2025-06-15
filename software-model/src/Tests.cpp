
#include "Tests.h"
#include <iostream>
#include <iomanip>
#include <cassert>

namespace Tests {

void testNPU() {
    // Test case 1: 4x4 * Identity matrix
    std::vector<std::vector<int8_t>> A = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    
    std::vector<std::vector<int8_t>> B = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    
    testMatrixMultiplication(A, B, "4x4 * Identity");
    
    // Test case 2: Simple 2x2 multiplication
    std::vector<std::vector<int8_t>> A2 = {
        {1, 2},
        {3, 4}
    };
    
    std::vector<std::vector<int8_t>> B2 = {
        {2, 0},
        {1, 2}
    };
    
    testMatrixMultiplication(A2, B2, "2x2 Simple");
    
    // Test case 3: Non-square matrices
    std::vector<std::vector<int8_t>> A3 = {
        {1, 2, 3},
        {4, 5, 6}
    };
    
    std::vector<std::vector<int8_t>> B3 = {
        {1, 0},
        {0, 1},
        {1, 1}
    };
    
    testMatrixMultiplication(A3, B3, "2x3 * 3x2");
}

void testMatrixMultiplication(const std::vector<std::vector<int8_t>>& A,
                             const std::vector<std::vector<int8_t>>& B,
                             const std::string& testName) {
    
    std::cout << "\n=== " << testName << " ===" << std::endl;
    
    int M = A.size();
    int K = A[0].size();
    int N = B[0].size();
    
    // Validate matrix dimensions
    assert(B.size() == K);
    
    NPU npu;
    npu.reset();
    npu.loadMatrices(A, B);
    npu.startComputation(K, M, N);
    
    // Run simulation
    int cycles = 0;
    while (npu.isBusy() && cycles < 1000) {
        npu.tick();
        cycles++;
    }
    
    auto actual_result = npu.getResult(M, N);
    auto expected_result = computeExpectedResult(A, B);
    
    printMatrix(actual_result, "NPU Result");
    printMatrix(expected_result, "Expected Result");
    
    bool passed = compareMatrices(actual_result, expected_result);
    std::cout << "Test " << (passed ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Completed in " << cycles << " cycles" << std::endl;
    
    if (!passed) {
        std::cout << "ERROR: Results do not match!" << std::endl;
    }
}

std::vector<std::vector<int32_t>> computeExpectedResult(
    const std::vector<std::vector<int8_t>>& A,
    const std::vector<std::vector<int8_t>>& B) {
    
    int M = A.size();
    int K = A[0].size();
    int N = B[0].size();
    
    std::vector<std::vector<int32_t>> result(M, std::vector<int32_t>(N, 0));
    
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < K; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return result;
}

void printMatrix(const std::vector<std::vector<int32_t>>& matrix, 
                const std::string& name) {
    std::cout << name << ":" << std::endl;
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << std::setw(6) << val << " ";
        }
        std::cout << std::endl;
    }
}

bool compareMatrices(const std::vector<std::vector<int32_t>>& actual,
                    const std::vector<std::vector<int32_t>>& expected) {
    if (actual.size() != expected.size()) return false;
    
    for (size_t i = 0; i < actual.size(); i++) {
        if (actual[i].size() != expected[i].size()) return false;
        for (size_t j = 0; j < actual[i].size(); j++) {
            if (actual[i][j] != expected[i][j]) return false;
        }
    }
    return true;
}

} // namespace TestUtils
