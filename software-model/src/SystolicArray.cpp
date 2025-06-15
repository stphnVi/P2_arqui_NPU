
#include "SystolicArray.h"

SystolicArray::SystolicArray() {}

void SystolicArray::reset() {
    for (int i = 0; i < 16; i++) {
        pes[i].reset();
    }
}

std::vector<std::vector<int32_t>> SystolicArray::process(int32_t datain_h, int32_t datain_v, int state) {
    // Extract 8-bit values from 32-bit inputs
    int8_t h_data[4] = {
        (int8_t)((datain_h >> 24) & 0xFF),
        (int8_t)((datain_h >> 16) & 0xFF),
        (int8_t)((datain_h >> 8) & 0xFF),
        (int8_t)(datain_h & 0xFF)
    };
    
    int8_t v_data[4] = {
        (int8_t)((datain_v >> 24) & 0xFF),
        (int8_t)((datain_v >> 16) & 0xFF),
        (int8_t)((datain_v >> 8) & 0xFF),
        (int8_t)(datain_v & 0xFF)
    };
    
    // Temporary storage for data flowing through array
    int8_t dataout_h[12] = {0}; // 3 rows * 4 columns
    int8_t dataout_v[12] = {0}; // 4 rows * 3 columns
    
    // Process 4x4 PE array
    // Row 0
    auto [h0, v0, p0] = pes[0].process(h_data[0], v_data[0], state);
    dataout_h[0] = h0; dataout_v[0] = v0;
    
    auto [h1, v1, p1] = pes[1].process(dataout_h[0], v_data[1], state);
    dataout_h[1] = h1; dataout_v[1] = v1;
    
    auto [h2, v2, p2] = pes[2].process(dataout_h[1], v_data[2], state);
    dataout_h[2] = h2; dataout_v[2] = v2;
    
    auto [h3, v3, p3] = pes[3].process(dataout_h[2], v_data[3], state);
    dataout_v[3] = v3;
    
    // Row 1
    auto [h4, v4, p4] = pes[4].process(h_data[1], dataout_v[0], state);
    dataout_h[3] = h4; dataout_v[4] = v4;
    
    auto [h5, v5, p5] = pes[5].process(dataout_h[3], dataout_v[1], state);
    dataout_h[4] = h5; dataout_v[5] = v5;
    
    auto [h6, v6, p6] = pes[6].process(dataout_h[4], dataout_v[2], state);
    dataout_h[5] = h6; dataout_v[6] = v6;
    
    auto [h7, v7, p7] = pes[7].process(dataout_h[5], dataout_v[3], state);
    dataout_v[7] = v7;
    
    // Row 2
    auto [h8, v8, p8] = pes[8].process(h_data[2], dataout_v[4], state);
    dataout_h[6] = h8; dataout_v[8] = v8;
    
    auto [h9, v9, p9] = pes[9].process(dataout_h[6], dataout_v[5], state);
    dataout_h[7] = h9; dataout_v[9] = v9;
    
    auto [h10, v10, p10] = pes[10].process(dataout_h[7], dataout_v[6], state);
    dataout_h[8] = h10; dataout_v[10] = v10;
    
    auto [h11, v11, p11] = pes[11].process(dataout_h[8], dataout_v[7], state);
    dataout_v[11] = v11;
    
    // Row 3
    auto [h12, v12, p12] = pes[12].process(h_data[3], dataout_v[8], state);
    dataout_h[9] = h12;
    
    auto [h13, v13, p13] = pes[13].process(dataout_h[9], dataout_v[9], state);
    dataout_h[10] = h13;
    
    auto [h14, v14, p14] = pes[14].process(dataout_h[10], dataout_v[10], state);
    dataout_h[11] = h14;
    
    auto [h15, v15, p15] = pes[15].process(dataout_h[11], dataout_v[11], state);
    
    // Return results organized by rows
    return {
        {p0, p1, p2, p3},
        {p4, p5, p6, p7},
        {p8, p9, p10, p11},
        {p12, p13, p14, p15}
    };
}
