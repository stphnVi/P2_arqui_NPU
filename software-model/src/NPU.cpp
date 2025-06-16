#include "NPU.h"
#include <iostream>
#include <algorithm>
#include <cmath>

NPU::NPU() : busy(false), K_reg(0), M_reg(0), N_reg(0),
             current_tile_i(0), current_tile_j(0), current_tile_k(0),
             tiles_M(0), tiles_N(0), tiles_K(0), using_tiling(false),
             tile_computation_busy(false) {}

void NPU::reset() {
    A_loader.reset();
    B_loader.reset();
    systolic.reset();
    ctrl.reset();
    busy = false;
    tile_computation_busy = false;
    K_reg = M_reg = N_reg = 0;
    buffer_A.clear();
    buffer_B.clear();
    buffer_C.clear();
    
    // Reset tiling state
    current_tile_i = current_tile_j = current_tile_k = 0;
    tiles_M = tiles_N = tiles_K = 0;
    using_tiling = false;
    original_A.clear();
    original_B.clear();
    final_result.clear();
}

void NPU::computeTileDimensions() {
    const int TILE_SIZE = 4;
    
    tiles_M = (M_reg + TILE_SIZE - 1) / TILE_SIZE;  // Ceiling division
    tiles_N = (N_reg + TILE_SIZE - 1) / TILE_SIZE;
    tiles_K = (K_reg + TILE_SIZE - 1) / TILE_SIZE;
    
    using_tiling = (M_reg > TILE_SIZE || N_reg > TILE_SIZE || K_reg > TILE_SIZE);
    
    std::cout << "Matrix dimensions: " << M_reg << "x" << K_reg << " * " << K_reg << "x" << N_reg << std::endl;
    std::cout << "Tiling required: " << (using_tiling ? "YES" : "NO") << std::endl;
    if (using_tiling) {
        std::cout << "Tile grid: " << tiles_M << "x" << tiles_N << " (with " << tiles_K << " K-tiles)" << std::endl;
    }
}

void NPU::loadTileData(int tile_i, int tile_j, int tile_k) {
    const int TILE_SIZE = 4;
    
    buffer_A.clear();
    buffer_B.clear();
    
    // Calculate effective tile dimensions
    int tile_M = std::min(TILE_SIZE, M_reg - tile_i * TILE_SIZE);
    int tile_K = std::min(TILE_SIZE, K_reg - tile_k * TILE_SIZE);
    int tile_N = std::min(TILE_SIZE, N_reg - tile_j * TILE_SIZE);
    
    // Prepare A tile data [tile_i*4:(tile_i+1)*4, tile_k*4:(tile_k+1)*4]
    for (int cycle = 0; cycle < tile_K + 3; cycle++) {
        int32_t A_word = 0;
        
        for (int row = 0; row < 4; row++) {
            int8_t val = 0;
            int k_idx = cycle - row;
            
            if (k_idx >= 0 && k_idx < tile_K && row < tile_M) {
                int global_row = tile_i * TILE_SIZE + row;
                int global_k = tile_k * TILE_SIZE + k_idx;
                if (global_row < M_reg && global_k < K_reg) {
                    val = original_A[global_row][global_k];
                }
            }
            
            A_word |= ((int32_t)(val & 0xFF) << (24 - row * 8));
        }
        buffer_A.push_back(A_word);
    }
    
    // Prepare B tile data [tile_k*4:(tile_k+1)*4, tile_j*4:(tile_j+1)*4]
    for (int cycle = 0; cycle < tile_K + 3; cycle++) {
        int32_t B_word = 0;
        
        for (int col = 0; col < 4; col++) {
            int8_t val = 0;
            int k_idx = cycle - col;
            
            if (k_idx >= 0 && k_idx < tile_K && col < tile_N) {
                int global_k = tile_k * TILE_SIZE + k_idx;
                int global_col = tile_j * TILE_SIZE + col;
                if (global_k < K_reg && global_col < N_reg) {
                    val = original_B[global_k][global_col];
                }
            }
            
            B_word |= ((int32_t)(val & 0xFF) << (24 - col * 8));
        }
        buffer_B.push_back(B_word);
    }
    
    // Reset result buffer for this tile
    buffer_C.assign(16, 0);
    
    std::cout << "Loaded tile (" << tile_i << "," << tile_j << "," << tile_k 
              << ") - size: " << tile_M << "x" << tile_K << " * " << tile_K << "x" << tile_N << std::endl;
}

void NPU::prepareCurrentTile() {
    loadTileData(current_tile_i, current_tile_j, current_tile_k);
    
    // Reset systolic array and controller for new tile computation
    systolic.reset();
    ctrl.reset();
    A_loader.reset();
    B_loader.reset();
    
    tile_computation_busy = true;
}

void NPU::accumulateTileResult() {
    const int TILE_SIZE = 4;
    
    // Calculate effective tile dimensions
    int tile_M = std::min(TILE_SIZE, M_reg - current_tile_i * TILE_SIZE);
    int tile_N = std::min(TILE_SIZE, N_reg - current_tile_j * TILE_SIZE);
    
    // Accumulate results into final result matrix
    for (int i = 0; i < tile_M; i++) {
        for (int j = 0; j < tile_N; j++) {
            int global_i = current_tile_i * TILE_SIZE + i;
            int global_j = current_tile_j * TILE_SIZE + j;
            final_result[global_i][global_j] += buffer_C[i * 4 + j];
        }
    }
    
    std::cout << "Accumulated tile (" << current_tile_i << "," << current_tile_j 
              << "," << current_tile_k << ") result" << std::endl;
}

bool NPU::moveToNextTile() {
    // Move through tiles in K, then J, then I order
    current_tile_k++;
    if (current_tile_k >= tiles_K) {
        current_tile_k = 0;
        current_tile_j++;
        if (current_tile_j >= tiles_N) {
            current_tile_j = 0;
            current_tile_i++;
            if (current_tile_i >= tiles_M) {
                return false; // All tiles complete
            }
        }
    }
    return true;
}

void NPU::loadMatrices(const std::vector<std::vector<int8_t>>& A, 
                      const std::vector<std::vector<int8_t>>& B) {
    // Store original matrices for tiling
    original_A = A;
    original_B = B;
    
    if (!using_tiling) {
        // For small matrices, use original logic
        int M = A.size();
        int K = A[0].size();
        int N = B[0].size();
        
        buffer_A.clear();
        buffer_B.clear();
        
        // Original data loading logic
        for (int cycle = 0; cycle < K + 3; cycle++) {
            int32_t A_word = 0;
            
            for (int row = 0; row < 4; row++) {
                int8_t val = 0;
                int k_idx = cycle - row;
                
                if (k_idx >= 0 && k_idx < K && row < M) {
                    val = A[row][k_idx];
                }
                
                A_word |= ((int32_t)(val & 0xFF) << (24 - row * 8));
            }
            buffer_A.push_back(A_word);
        }
        
        for (int cycle = 0; cycle < K + 3; cycle++) {
            int32_t B_word = 0;
            
            for (int col = 0; col < 4; col++) {
                int8_t val = 0;
                int k_idx = cycle - col;
                
                if (k_idx >= 0 && k_idx < K && col < N) {
                    val = B[k_idx][col];
                }
                
                B_word |= ((int32_t)(val & 0xFF) << (24 - col * 8));
            }
            buffer_B.push_back(B_word);
        }
        
        buffer_C.resize(16, 0);
    }
}

void NPU::startComputation(int K, int M, int N) {
    K_reg = K;
    M_reg = M;  
    N_reg = N;
    
    computeTileDimensions();
    
    if (using_tiling) {
        // Initialize final result matrix
        final_result.assign(M_reg, std::vector<int32_t>(N_reg, 0));
        
        // Start with first tile
        current_tile_i = current_tile_j = current_tile_k = 0;
        prepareCurrentTile();
    }
    
    busy = true;
    ctrl.reset();
}

bool NPU::tick() {
    if (!busy) return false;
    
    if (using_tiling) {
        if (!tile_computation_busy) {
            // Current tile is done, move to next
            accumulateTileResult();
            
            if (moveToNextTile()) {
                prepareCurrentTile();
            } else {
                // All tiles complete
                busy = false;
                return false;
            }
        }
        
        // Process current tile
        int effective_K = std::min(4, K_reg - current_tile_k * 4);
        ctrl.update(false, tile_computation_busy, effective_K, 4, 4);
    } else {
        // Non-tiling mode
        ctrl.update(false, busy, K_reg, M_reg, N_reg);
    }
    
    int32_t A_data = 0;
    int32_t B_data = 0;
    
    // Provide data during READ state
    if (ctrl.getState() == 1) { // READ
        int idx = ctrl.getCounter() - 1;
        
        if (idx >= 0 && idx < static_cast<int>(buffer_A.size())) {
            A_data = buffer_A[idx];
        }
        if (idx >= 0 && idx < static_cast<int>(buffer_B.size())) {
            B_data = buffer_B[idx];
        }
    }
    
    // Process through data loaders
    int effective_K = using_tiling ? std::min(4, K_reg - current_tile_k * 4) : K_reg;
    int32_t datain_h = A_loader.process(ctrl.getState(), A_data, effective_K, ctrl.getCounter());
    int32_t datain_v = B_loader.process(ctrl.getState(), B_data, effective_K, ctrl.getCounter());
    
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
        if (using_tiling) {
            tile_computation_busy = false;
        } else {
            busy = false;
        }
    }
    
    std::cout << "State: " << ctrl.getState()
              << " | Counter: " << ctrl.getCounter()
              << " | A_data: " << std::hex << A_data
              << " | B_data: " << std::hex << B_data << std::dec;
    
    if (using_tiling) {
        std::cout << " | Tile: (" << current_tile_i << "," << current_tile_j << "," << current_tile_k << ")";
    }
    std::cout << std::endl;

    return busy;
}

std::vector<std::vector<int32_t>> NPU::getResult(int M, int N) {
    if (using_tiling) {
        return final_result;
    } else {
        // Original logic for small matrices
        std::vector<std::vector<int32_t>> result(M, std::vector<int32_t>(N, 0));
        
        for (int i = 0; i < M && i < 4; i++) {
            for (int j = 0; j < N && j < 4; j++) {
                result[i][j] = buffer_C[i * 4 + j];
            }
        }
        
        return result;
    }
}

bool NPU::isBusy() const {
    return busy;
}
