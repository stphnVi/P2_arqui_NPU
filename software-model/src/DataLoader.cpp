#include "DataLoader.h"

DataLoader::DataLoader() : temp_out1(0), temp_out2(0), temp_out3(0), temp_out4(0), out(0) {}

void DataLoader::reset() {
    temp_out1 = 0;
    temp_out2 = 0;
    temp_out3 = 0;
    temp_out4 = 0;
    out = 0;
}

int32_t DataLoader::process(int state, int32_t in_data, int /*K_reg*/, int /*counter*/) {
    if (state == 0) { // IDLE
        reset();
        return 0;
    }
    
    if (state == 1) { // READ - just pass through the data
        return in_data;
    }
    
    return 0;
}