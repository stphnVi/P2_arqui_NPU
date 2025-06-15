#include "SystolicArray.h"
#include <cstring>

//A data →  [PE00] → [PE01] → [PE02] → [PE03]
//            ↓        ↓        ↓        ↓
//A data →  [PE10] → [PE11] → [PE12] → [PE13]
//            ↓        ↓        ↓        ↓
//A data →  [PE20] → [PE21] → [PE22] → [PE23]
//            ↓        ↓        ↓        ↓
//A data →  [PE30] → [PE31] → [PE32] → [PE33]


SystolicArray::SystolicArray() {
    reset();
}

void SystolicArray::reset() {
    for (int i = 0; i < 16; i++) {
        pes[i].reset();
    }
    memset(h_regs, 0, sizeof(h_regs));
    memset(v_regs, 0, sizeof(v_regs));
}

std::vector<std::vector<int32_t>> SystolicArray::process(int32_t datain_h, int32_t datain_v, int state) {
    // Extract input values
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
    
    // Temporary storage for PE outputs
    int8_t h_out[4][4];
    int8_t v_out[4][4];
    std::vector<std::vector<int32_t>> results(4, std::vector<int32_t>(4));
    
    // Process each PE in the correct order (systolic flow)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int pe_idx = i * 4 + j;
            
            // Determine inputs for this PE
            int8_t in_west, in_north;
            
            if (j == 0) {
                // Leftmost column gets external input
                in_west = h_vals[i];
            } else {
                // Get from left neighbor's output (stored in register)
                in_west = h_regs[i][j-1];
            }
            
            if (i == 0) {
                // Top row gets external input
                in_north = v_vals[j];
            } else {
                // Get from top neighbor's output (stored in register)
                in_north = v_regs[i-1][j];
            }
            
            // Process through PE
            auto [west_out, north_out, psum] = pes[pe_idx].process(in_west, in_north, state);
            
            // Store outputs
            h_out[i][j] = west_out;
            v_out[i][j] = north_out;
            results[i][j] = psum;
        }
    }
    
    // Update registers for next cycle
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            h_regs[i][j] = h_out[i][j];
        }
    }
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            v_regs[i][j] = v_out[i][j];
        }
    }
    
    return results;
}
