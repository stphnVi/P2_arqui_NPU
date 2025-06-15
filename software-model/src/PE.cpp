#include "PE.h"

PE::PE() : maccout(0), west_c(0), north_c(0) {}

void PE::reset() {
    maccout = 0;
    west_c = 0;
    north_c = 0;
}

std::tuple<int8_t, int8_t, int32_t> PE::process(int8_t in_west, int8_t in_north, int state) {
    int32_t product = in_west * in_north;
    
    // Negative edge clock behavior
    if (state == 1) { // READ state
        maccout += product;
        west_c = in_west;
        north_c = in_north;
    }
    
    // Positive edge clock behavior
    if (state == 0) { // IDLE state
        maccout = 0;
    }
    
    return std::make_tuple(west_c, north_c, maccout);
}

int32_t PE::getPsum() const {
    return maccout;
}