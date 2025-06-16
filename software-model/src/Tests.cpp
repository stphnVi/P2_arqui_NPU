#include "Tests.h"
#include <iostream>
#include <iomanip>
#include <cassert>

namespace Tests {

void testNPU(bool step_mode) {
    // Test case 1: 4x4 * Identity matrix (no tiling needed)
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
    
    testMatrixMultiplication(A, B, "4x4 * Identity", step_mode);
    
    // Test case 2: Simple 2x2 multiplication (no tiling needed)
    std::vector<std::vector<int8_t>> A2 = {
        {1, 2},
        {3, 4}
    };
    
    std::vector<std::vector<int8_t>> B2 = {
        {2, 0},
        {1, 2}
    };
    
    testMatrixMultiplication(A2, B2, "2x2 Simple", step_mode);
    
    // Test case 3: Non-square matrices (no tiling needed)
    std::vector<std::vector<int8_t>> A3 = {
        {1, 2, 3},
        {4, 5, 6}
    };
    
    std::vector<std::vector<int8_t>> B3 = {
        {1, 0},
        {0, 1},
        {1, 1}
    };
    
    testMatrixMultiplication(A3, B3, "2x3 * 3x2", step_mode);
    
    // Test case 4: 5x5 matrices (requires tiling)
    std::vector<std::vector<int8_t>> A4 = {
        {1, 2, 1, 0, 1},
        {0, 1, 2, 1, 0},
        {1, 0, 1, 2, 1},
        {2, 1, 0, 1, 2},
        {1, 1, 1, 1, 1}
    };
    
    std::vector<std::vector<int8_t>> B4 = {
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1}
    };
    
    testMatrixMultiplication(A4, B4, "5x5 * 5x5 (Tiling)", step_mode);
    
    // Test case 5: 6x4 * 4x3 (requires tiling in M dimension)
    std::vector<std::vector<int8_t>> A5 = {
        {1, 2, 1, 2},
        {2, 1, 2, 1},
        {1, 1, 1, 1},
        {2, 2, 2, 2},
        {0, 1, 0, 1},
        {1, 0, 1, 0}
    };
    
    std::vector<std::vector<int8_t>> B5 = {
        {1, 2, 1},
        {0, 1, 2},
        {2, 0, 1},
        {1, 1, 0}
    };
    
    testMatrixMultiplication(A5, B5, "6x4 * 4x3 (M-Tiling)", step_mode);
    
    // Test case 6: Large 7x7 * 7x5 (requires extensive tiling)
    std::vector<std::vector<int8_t>> A6 = {
        {1, 2, 3, 4, 5, 6, 7},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2},
        {1, 0, 1, 0, 1, 0, 1},
        {3, 3, 3, 3, 3, 3, 3},
        {1, 2, 1, 2, 1, 2, 1}
    };
    
    std::vector<std::vector<int8_t>> B6 = {
        {1, 1, 2, 1, 1},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
        {1, 1, 0, 1, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 2, 0, 0}
    };
    
    testMatrixMultiplication(A6, B6, "7x7 * 7x5 (Full Tiling)", step_mode);
}

void testMatrixMultiplication(const std::vector<std::vector<int8_t>>& A,
                             const std::vector<std::vector<int8_t>>& B,
                             const std::string& testName,
                             bool step_mode) {
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "=== " << testName << " ===" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    int M = A.size();
    int K = A[0].size();
    int N = B[0].size();
    
    // Validate matrix dimensions
    assert(B.size() == static_cast<size_t>(K));
    
    std::cout << "Matrix A (" << M << "x" << K << "):" << std::endl;
    for (const auto& row : A) {
        for (int val : row) {
            std::cout << std::setw(3) << (int)val << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Matrix B (" << K << "x" << N << "):" << std::endl;
    for (const auto& row : B) {
        for (int val : row) {
            std::cout << std::setw(3) << (int)val << " ";
        }
        std::cout << std::endl;
    }
    
    NPU npu;
    npu.reset();
    npu.loadMatrices(A, B);
    npu.startComputation(K, M, N);
    
    // Run simulation
    int cycles = 0;
    while (npu.isBusy() && cycles < 50000) { // Increased limit for extensive tiling
        if (step_mode) {
            std::cout << "[Cycle " << cycles << "] Press Enter to step..." << std::flush;
            std::cin.get();
        }

        npu.tick();
        cycles++;
    }
    
    if (cycles >= 50000) {
        std::cout << "WARNING: Simulation reached maximum cycle limit!" << std::endl;
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
        
        // Show differences
        std::cout << "Differences:" << std::endl;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (actual_result[i][j] != expected_result[i][j]) {
                    std::cout << "Position (" << i << "," << j << "): "
                              << "got " << actual_result[i][j] 
                              << ", expected " << expected_result[i][j] << std::endl;
                }
            }
        }
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
    std::cout << "\n" << name << ":" << std::endl;
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

} // namespace Tests
