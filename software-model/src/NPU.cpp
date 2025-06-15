#include "NPU.h"
#include <iostream>
#include <algorithm>

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
    
    buffer_A.clear();
    buffer_B.clear();
    
    // For systolic array, we need to prepare data with proper skewing
    // Each cycle, we feed one diagonal of data
    
    // Prepare A matrix data (row-wise feeding with delays)
    // We need K + 3 cycles worth of data for a 4x4 array
    for (int cycle = 0; cycle < K + 3; cycle++) {
        int32_t A_word = 0;
        
        for (int row = 0; row < 4; row++) {
            int8_t val = 0;
            // Calculate which K index this row should be reading at this cycle
            int k_idx = cycle - row;
            
            if (k_idx >= 0 && k_idx < K && row < M) {
                val = A[row][k_idx];
            }
            
            A_word |= ((int32_t)(val & 0xFF) << (24 - row * 8));
        }
        buffer_A.push_back(A_word);
    }
    
    // Prepare B matrix data (column-wise feeding with delays)
    for (int cycle = 0; cycle < K + 3; cycle++) {
        int32_t B_word = 0;
        
        for (int col = 0; col < 4; col++) {
            int8_t val = 0;
            // Calculate which K index this column should be reading at this cycle
            int k_idx = cycle - col;
            
            if (k_idx >= 0 && k_idx < K && col < N) {
                val = B[k_idx][col];
            }
            
            B_word |= ((int32_t)(val & 0xFF) << (24 - col * 8));
        }
        buffer_B.push_back(B_word);
    }
    
    // Initialize result buffer
    buffer_C.resize(16, 0);
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
    
    // Provide data during READ state
    if (ctrl.getState() == 1) { // READ
        int idx = ctrl.getCounter() - 1; // Current cycle in READ state
        
        if (idx >= 0 && idx < static_cast<int>(buffer_A.size())) {
            A_data = buffer_A[idx];
        }
        if (idx >= 0 && idx < static_cast<int>(buffer_B.size())) {
            B_data = buffer_B[idx];
        }
    }
    
    // Process through data loaders
    int32_t datain_h = A_loader.process(ctrl.getState(), A_data, K_reg, ctrl.getCounter());
    int32_t datain_v = B_loader.process(ctrl.getState(), B_data, K_reg, ctrl.getCounter());
    
    // Process through systolic array
    auto results = systolic.process(datain_h, datain_v, ctrl.getState());
    
    // During WRITE state, capture the final results
    if (ctrl.getCWrEn()) {
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