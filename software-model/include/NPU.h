#ifndef NPU_H
#define NPU_H

#include "DataLoader.h"
#include "SystolicArray.h"
#include "Controller.h"
#include <vector>
#include <cstdint>

class NPU {
private:
    DataLoader A_loader, B_loader;
    SystolicArray systolic;
    Controller ctrl;
    
    std::vector<int32_t> buffer_A, buffer_B;
    std::vector<int32_t> buffer_C;
    
    // Original matrices for tiling
    std::vector<std::vector<int8_t>> original_A, original_B;
    std::vector<std::vector<int32_t>> final_result;
    
    bool busy;
    int K_reg, M_reg, N_reg;
    
    // Tiling state
    int current_tile_i, current_tile_j, current_tile_k;
    int tiles_M, tiles_N, tiles_K;
    bool using_tiling;
    bool tile_computation_busy;
    
    // Helper methods
    void computeTileDimensions();
    void prepareCurrentTile();
    void accumulateTileResult();
    bool moveToNextTile();
    void loadTileData(int tile_i, int tile_j, int tile_k);
    
public:
    NPU();
    void reset();
    void loadMatrices(const std::vector<std::vector<int8_t>>& A, 
                     const std::vector<std::vector<int8_t>>& B);
    void startComputation(int K, int M, int N);
    bool tick();
    std::vector<std::vector<int32_t>> getResult(int M, int N);
    bool isBusy() const;
};

#endif // NPU_H
