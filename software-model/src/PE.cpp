#include "PE.h"
#include <algorithm>

PE::PE() : maccout(0), west_c(0), north_c(0), relu_enabled(false) {}

void PE::reset() {
    maccout = 0;
    west_c = 0;
    north_c = 0;
}

void PE::enableReLU(bool enable) {
    relu_enabled = enable;
}


//Función de activación
int32_t PE::applyReLU(int32_t value) const {
    return relu_enabled ? std::max(0, value) : value;
}

std::tuple<int8_t, int8_t, int32_t> PE::process(int8_t in_west, int8_t in_north, int state) {
    int8_t out_west = in_west;
    int8_t out_north = in_north;
    
    if (state == 0) { // IDLE - reset accumulator
        maccout = 0;
    } else if (state == 1) { // READ - accumulate
        int32_t product = (int32_t)in_west * (int32_t)in_north;
        maccout += product;
    } else if (state == 2) { // WRITE - apply ReLU if enabled
        maccout = applyReLU(maccout);
    }
    
    return std::make_tuple(out_west, out_north, maccout);
}

int32_t PE::getPsum() const {
    return maccout;
}
