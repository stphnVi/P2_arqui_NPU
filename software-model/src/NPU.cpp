#include "NPU.h"
#include <iostream>

NPU::NPU() : busy(false), K_reg(0), M_reg(0), N_reg(0) {}

void NPU::reset() {
    A_loader.reset();
    B_loader.reset();
    systolic.reset();
    ctrl.reset();
    busy = false;
    K_reg = M_reg = N_reg = 0;
    buffer_A.clear();
    buffer_B.clear();
    buffer_C.clear();
}

void NPU::loadMatrices(const std::vector<std::vector<int8_t>>& A, 
                      const std::vector<std::vector<int8_t>>& B) {
    int M = A.size();
    int K = A[0].size();
    int N = B[0].size();
    
    // Store matrices in a format suitable for 4x4 systolic processing
    // We'll create K entries, each containing a 4x4 slice
    buffer_A.clear();
    buffer_B.clear();
    
    // For each K step, pack 4 rows of A and 4 columns of B
    for (int k = 0; k < K; k++) {
        // Pack 4 elements from column k of A (for 4 rows)
        int32_t A_word = 0;
        for (int i = 0; i < 4; i++) {
            int8_t val = (i < M) ? A[i][k] : 0;
            A_word |= ((int32_t)(val & 0xFF) << (24 - i * 8));
        }
        buffer_A.push_back(A_word);
        
        // Pack 4 elements from row k of B (for 4 columns)  
        int32_t B_word = 0;
        for (int j = 0; j < 4; j++) {
            int8_t val = (j < N) ? B[k][j] : 0;
            B_word |= ((int32_t)(val & 0xFF) << (24 - j * 8));
        }
        buffer_B.push_back(B_word);
    }
    
    // Initialize result buffer
    buffer_C.resize(16, 0); // 4x4 results
}

void NPU::startComputation(int K, int M, int N) {
    K_reg = K;
    M_reg = M;  
    N_reg = N;
    busy = true;
    ctrl.reset();
}

bool NPU::tick() {
    if (!busy) return false;
    
    ctrl.update(false, busy, K_reg, M_reg, N_reg);
    
    int32_t A_data = 0;
    int32_t B_data = 0;
    
    // Provide data during READ cycles
    if (ctrl.getState() == 1) { // READ
        int idx = ctrl.getIdxA();
        if (idx >= 0 && idx < static_cast<int>(buffer_A.size())) {
            A_data = buffer_A[idx];
        }
        if (idx >= 0 && idx < static_cast<int>(buffer_B.size())) {
            B_data = buffer_B[idx];
        }
    }
    
    // Process through data loaders (simplified - just pass through)
    int32_t datain_h = A_loader.process(ctrl.getState(), A_data, K_reg, ctrl.getCounter());
    int32_t datain_v = B_loader.process(ctrl.getState(), B_data, K_reg, ctrl.getCounter());
    
    // Process through systolic array
    auto results = systolic.process(datain_h, datain_v, ctrl.getState());
    
    // During WRITE state, capture the final results
    if (ctrl.getCWrEn()) {
        // Store all 4x4 results
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                buffer_C[i * 4 + j] = results[i][j];
            }
        }
    }
    
    if (ctrl.getState() == 3) { // FINISH
        busy = false;
    }
    
    return busy;
}

std::vector<std::vector<int32_t>> NPU::getResult(int M, int N) {
    std::vector<std::vector<int32_t>> result(M, std::vector<int32_t>(N, 0));
    
    // Extract the relevant portion from the 4x4 buffer
    for (int i = 0; i < M && i < 4; i++) {
        for (int j = 0; j < N && j < 4; j++) {
            result[i][j] = buffer_C[i * 4 + j];
        }
    }
    
    return result;
}

bool NPU::isBusy() const {
    return busy;
}
