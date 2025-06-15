#ifndef DATALOADER_H
#define DATALOADER_H

#include <cstdint>

class DataLoader {
private:
    int8_t temp_out1;
    int16_t temp_out2;
    int32_t temp_out3;
    int32_t temp_out4;
    int32_t out;
    
public:
    DataLoader();
    void reset();
    int32_t process(int state, int32_t in_data, int K_reg, int counter);
};

#endif // DATALOADER_H
