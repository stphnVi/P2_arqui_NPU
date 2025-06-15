#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>

class Controller {
private:
    enum State { IDLE = 0, READ = 1, WRITE = 2, FINISH = 3 };
    
    State state, n_state;
    int out_cycle;
    int a_offset, b_offset;
    int counter_a, counter_b, counter, counter_out;
    int idx_a, idx_b, idx_c;
    
    void updateIndices(int K_reg, int M_reg, int N_reg, bool busy);
    
public:
    Controller();
    void reset();
    void update(bool in_valid, bool busy, int K_reg, int M_reg, int N_reg);
    
    // Getters
    int getState() const;
    int getCounter() const;
    int getIdxA() const;
    int getIdxB() const;
    int getIdxC() const;
    bool getCWrEn() const;
    int getCounterOut() const;
};

#endif // CONTROLLER_H
