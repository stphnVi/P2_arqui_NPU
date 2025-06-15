#include "PE.h"

PE::PE() : maccout(0), west_c(0), north_c(0) {}

void PE::reset() {
    maccout = 0;
    west_c = 0;
    north_c = 0;
}

std::tuple<int8_t, int8_t, int32_t> PE::process(int8_t in_west, int8_t in_north, int state) {
    int8_t out_west = in_west;
    int8_t out_north = in_north;
    
    if (state == 0) { // IDLE - reset accumulator
        maccout = 0;
    } else if (state == 1) { // READ - accumulate ONLY if we have valid data
        // Only accumulate if both inputs are non-zero OR this is a valid computation
        int32_t product = (int32_t)in_west * (int32_t)in_north;
        maccout += product;
    }
    // For WRITE and FINISH states, just maintain the current accumulator value
    
    return std::make_tuple(out_west, out_north, maccout);
}

int32_t PE::getPsum() const {
    return maccout;
}