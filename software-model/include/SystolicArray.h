#ifndef SYSTOLICARRAY_H
#define SYSTOLICARRAY_H

#include "PE.h"
#include <vector>
#include <cstdint>

class SystolicArray {
private:
    PE pes[16]; // 4x4 array
    
public:
    SystolicArray();
    void reset();
    std::vector<std::vector<int32_t>> process(int32_t datain_h, int32_t datain_v, int state);
};

#endif // SYSTOLICARRAY_H
