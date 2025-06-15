#include "DataLoader.h"

DataLoader::DataLoader() : temp_out1(0), temp_out2(0), temp_out3(0), temp_out4(0), out(0) {}

void DataLoader::reset() {
    temp_out1 = 0;
    temp_out2 = 0;
    temp_out3 = 0;
    temp_out4 = 0;
    out = 0;
}

int32_t DataLoader::process(int state, int32_t in_data, int K_reg, int counter) {
    if (state == 0) { // IDLE
        reset();
        return 0;
    }
    
    if (state == 1) { // READ
        if (counter < K_reg) {
            temp_out1 = (in_data >> 24) & 0xFF;
            temp_out2 = (temp_out2 << 8) | ((in_data >> 16) & 0xFF);
            temp_out3 = (temp_out3 << 8) | ((in_data >> 8) & 0xFF);
            temp_out4 = (temp_out4 << 8) | (in_data & 0xFF);
            
            out = (temp_out1 << 24) | 
                  ((temp_out2 & 0xFF) << 16) | 
                  (((temp_out3 >> 8) & 0xFF) << 8) | 
                  ((temp_out4 >> 16) & 0xFF);
        } else {
            temp_out1 <<= 8;
            temp_out2 <<= 8;
            temp_out3 <<= 8;
            temp_out4 <<= 8;
            
            out = ((temp_out2 & 0xFF) << 16) | 
                  (((temp_out3 >> 8) & 0xFF) << 8) | 
                  ((temp_out4 >> 16) & 0xFF);
        }
    }
    
    return out;
}
