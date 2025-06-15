#ifndef NPU_H
#define NPU_H

#include "DataLoader.h"
#include "SystolicArray.h"
#include "Controller.h"
#include <vector>
#include <cstdint>

class NPU {
private:
    DataLoader A_loader, B_loader;
    SystolicArray systolic;
    Controller ctrl;
    
    std::vector<int32_t> buffer_A, buffer_B;
    std::vector<int32_t> buffer_C;
    
    bool busy;
    int K_reg, M_reg, N_reg;
    
public:
    NPU();
    void reset();
    void loadMatrices(const std::vector<std::vector<int8_t>>& A, 
                     const std::vector<std::vector<int8_t>>& B);
    void startComputation(int K, int M, int N);
    bool tick();
    std::vector<std::vector<int32_t>> getResult(int M, int N);
    bool isBusy() const;
};

#endif // NPU_H
