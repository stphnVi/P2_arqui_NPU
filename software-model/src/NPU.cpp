#include "NPU.h"

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
    
    // Pack A matrix into 32-bit words (4 elements per word)
    buffer_A.clear();
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j += 4) {
            int32_t word = 0;
            for (int k = 0; k < 4 && (j + k) < K; k++) {
                word |= ((A[i][j + k] & 0xFF) << (24 - k * 8));
            }
            buffer_A.push_back(word);
        }
    }
    
    // Pack B matrix into 32-bit words
    buffer_B.clear();
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < K; i += 4) {
            int32_t word = 0;
            for (int k = 0; k < 4 && (i + k) < K; k++) {
                word |= ((B[i + k][j] & 0xFF) << (24 - k * 8));
            }
            buffer_B.push_back(word);
        }
    }
    
    // Initialize result buffer
    int result_size = ((M + 3) / 4) * ((N + 3) / 4) * 4;
    buffer_C.resize(result_size, 0);
}

void NPU::startComputation(int K, int M, int N) {
    K_reg = K;
    M_reg = M;
    N_reg = N;
    busy = true;
}

bool NPU::tick() {
    if (K_reg > 0) {
        ctrl.update(false, busy, K_reg, M_reg, N_reg);
        
        // Get data from buffers
        int32_t A_data = (ctrl.getIdxA() < buffer_A.size()) ? 
                        buffer_A[ctrl.getIdxA()] : 0;
        int32_t B_data = (ctrl.getIdxB() < buffer_B.size()) ? 
                        buffer_B[ctrl.getIdxB()] : 0;
        
        // Process through data loaders
        int32_t datain_h = A_loader.process(ctrl.getState(), A_data, 
                                           K_reg, ctrl.getCounter());
        int32_t datain_v = B_loader.process(ctrl.getState(), B_data, 
                                           K_reg, ctrl.getCounter());
        
        // Process through systolic array
        auto results = systolic.process(datain_h, datain_v, ctrl.getState());
        
        // Write results to buffer C
        if (ctrl.getCWrEn() && ctrl.getIdxC() < buffer_C.size()) {
            int row_select = ctrl.getCounterOut();
            if (row_select < 4) {
                // Pack 4 results into one 128-bit word (stored as 4 32-bit words)
                for (int i = 0; i < 4; i++) {
                    if (ctrl.getIdxC() * 4 + i < buffer_C.size()) {
                        buffer_C[ctrl.getIdxC() * 4 + i] = results[row_select][i];
                    }
                }
            }
        }
        
        // Update busy signal
        if (ctrl.getState() == 3) { // FINISH
            busy = false;
        }
    }
    
    return busy;
}

std::vector<std::vector<int32_t>> NPU::getResult(int M, int N) {
    std::vector<std::vector<int32_t>> result(M, std::vector<int32_t>(N, 0));
    
    // Extract results from buffer_C
    int idx = 0;
    for (int block_row = 0; block_row < (M + 3) / 4; block_row++) {
        for (int block_col = 0; block_col < (N + 3) / 4; block_col++) {
            for (int row = 0; row < 4 && (block_row * 4 + row) < M; row++) {
                for (int col = 0; col < 4 && (block_col * 4 + col) < N; col++) {
                    if (idx < buffer_C.size()) {
                        result[block_row * 4 + row][block_col * 4 + col] = buffer_C[idx];
                        idx++;
                    }
                }
            }
        }
    }
    
    return result;
}

bool NPU::isBusy() const {
    return busy;
}
