#include "SystolicArray.h"

SystolicArray::SystolicArray() {}

void SystolicArray::reset() {
    for (int i = 0; i < 16; i++) {
        pes[i].reset();
    }
}

std::vector<std::vector<int32_t>> SystolicArray::process(int32_t datain_h, int32_t datain_v, int state) {
    // Extract individual bytes as signed values
    int8_t h_vals[4] = {
        (int8_t)((datain_h >> 24) & 0xFF),
        (int8_t)((datain_h >> 16) & 0xFF),
        (int8_t)((datain_h >> 8) & 0xFF),
        (int8_t)(datain_h & 0xFF)
    };
    
    int8_t v_vals[4] = {
        (int8_t)((datain_v >> 24) & 0xFF),
        (int8_t)((datain_v >> 16) & 0xFF),
        (int8_t)((datain_v >> 8) & 0xFF),
        (int8_t)(datain_v & 0xFF)
    };
    
    std::vector<std::vector<int32_t>> results(4, std::vector<int32_t>(4));
    
    // Simplified: Each PE(i,j) directly processes A[i][k] * B[k][j] for current k
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int pe_idx = i * 4 + j;
            
            // PE(i,j) computes element (i,j) of result matrix
            // It gets A[i][k] and B[k][j] for current k
            int8_t a_val = h_vals[i];  // A[i][k]
            int8_t b_val = v_vals[j];  // B[k][j]
            
            auto [h_out, v_out, psum] = pes[pe_idx].process(a_val, b_val, state);
            results[i][j] = psum;
        }
    }
    
    return results;
}