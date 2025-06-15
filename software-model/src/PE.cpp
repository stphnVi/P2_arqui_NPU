#include "PE.h"

PE::PE() : maccout(0), west_c(0), north_c(0) {}

void PE::reset() {
    maccout = 0;
    west_c = 0;
    north_c = 0;
}

std::tuple<int8_t, int8_t, int32_t> PE::process(int8_t in_west, int8_t in_north, int state) {
    // Always pass through the input values to outputs (for systolic flow)
    int8_t out_west = in_west;
    int8_t out_north = in_north;
    
    if (state == 0) { // IDLE state - reset accumulator
        maccout = 0;
    } else if (state == 1) { // READ state - accumulate
        int32_t product = (int32_t)in_west * (int32_t)in_north;
        maccout += product;
    }
    // For other states, maintain current accumulator value
    
    return std::make_tuple(out_west, out_north, maccout);
}

int32_t PE::getPsum() const {
    return maccout;
}
