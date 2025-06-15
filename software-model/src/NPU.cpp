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
    
    // For simplicity, pack A matrix row by row
    buffer_A.clear();
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j += 4) {
            int32_t word = 0;
            for (int k = 0; k < 4; k++) {
                int8_t val = ((j + k) < K) ? A[i][j + k] : 0;
                word |= ((val & 0xFF) << (24 - k * 8));
            }
            buffer_A.push_back(word);
        }
    }
    
    // Pack B matrix column by column  
    buffer_B.clear();
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < K; i += 4) {
            int32_t word = 0;
            for (int k = 0; k < 4; k++) {
                int8_t val = ((i + k) < K) ? B[i + k][j] : 0;
                word |= ((val & 0xFF) << (24 - k * 8));
            }
            buffer_B.push_back(word);
        }
    }
    
    // Initialize result buffer - simple linear layout for now
    buffer_C.resize(M * N, 0);
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
    
    // Get data from buffers
    if (ctrl.getState() == 1) { // READ state
        if (ctrl.getIdxA() >= 0 && ctrl.getIdxA() < static_cast<int>(buffer_A.size())) {
            A_data = buffer_A[ctrl.getIdxA()];
        }
        if (ctrl.getIdxB() >= 0 && ctrl.getIdxB() < static_cast<int>(buffer_B.size())) {
            B_data = buffer_B[ctrl.getIdxB()];
        }
    }
    
    // Process through data loaders
    int32_t datain_h = A_loader.process(ctrl.getState(), A_data, 
                                       K_reg, ctrl.getCounter());
    int32_t datain_v = B_loader.process(ctrl.getState(), B_data, 
                                       K_reg, ctrl.getCounter());
    
    // Process through systolic array
    auto results = systolic.process(datain_h, datain_v, ctrl.getState());
    
    // Write results to buffer C (simplified for single 4x4 block first)
    if (ctrl.getCWrEn() && ctrl.getCounterOut() < 4) {
        int row = ctrl.getCounterOut();
        // For now, just store the first 4x4 block directly
        for (int col = 0; col < 4 && col < N_reg; col++) {
            if (row < M_reg) {
                int idx = row * N_reg + col;
                if (idx >= 0 && idx < static_cast<int>(buffer_C.size())) {
                    buffer_C[idx] = results[row][col];
                }
            }
        }
    }
    
    // Update busy signal
    if (ctrl.getState() == 3) { // FINISH
        busy = false;
    }
    
    return busy;
}

std::vector<std::vector<int32_t>> NPU::getResult(int M, int N) {
    std::vector<std::vector<int32_t>> result(M, std::vector<int32_t>(N, 0));
    
    // Simple extraction for now
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int idx = i * N + j;
            if (idx >= 0 && idx < static_cast<int>(buffer_C.size())) {
                result[i][j] = buffer_C[idx];
            }
        }
    }
    
    return result;
}

bool NPU::isBusy() const {
    return busy;
}
