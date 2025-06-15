#include "Controller.h"

Controller::Controller() : state(IDLE), n_state(IDLE), out_cycle(4), 
                          a_offset(0), b_offset(0), counter_a(0), counter_b(0), 
                          counter(0), counter_out(0), idx_a(0), idx_b(0), idx_c(0) {}

void Controller::reset() {
    state = IDLE;
    n_state = IDLE;
    out_cycle = 4;
    a_offset = 0;
    b_offset = 0;
    counter_a = 0;
    counter_b = 0;
    counter = 0;
    counter_out = 0;
    idx_a = 0;
    idx_b = 0;
    idx_c = 0;
}

void Controller::update(bool /*in_valid*/, bool busy, int K_reg, int M_reg, int N_reg) {
    state = n_state;
    
    switch (state) {
        case IDLE:
            if (busy) {
                n_state = READ;
                counter = 0;
            }
            break;
        case READ:
            counter++;
            // Need enough cycles for all K values plus pipeline delay
            if (counter >= K_reg + 8) {
                n_state = WRITE;
                counter_out = 0;
            }
            break;
        case WRITE:
            counter_out++;
            if (counter_out >= 4) { // Write 4 rows of results
                n_state = FINISH;
            }
            break;
        case FINISH:
            n_state = IDLE;
            break;
    }
    
    updateIndices(K_reg, M_reg, N_reg, busy);
}

void Controller::updateIndices(int K_reg, int /*M_reg*/, int /*N_reg*/, bool /*busy*/) {
    if (state == READ && counter > 0) {
        // Simple sequential access through K dimension
        int k_cycle = counter - 1;
        if (k_cycle < K_reg) {
            idx_a = k_cycle; // Will be used to index into flattened A data
            idx_b = k_cycle; // Will be used to index into flattened B data
        }
    }
    
    if (state == FINISH) {
        idx_a = 0;
        idx_b = 0;
        idx_c = 0;
    }
}

int Controller::getState() const { return state; }
int Controller::getCounter() const { return counter; }
int Controller::getIdxA() const { return idx_a; }
int Controller::getIdxB() const { return idx_b; }
int Controller::getIdxC() const { return idx_c; }
bool Controller::getCWrEn() const { return state == WRITE; }
int Controller::getCounterOut() const { return counter_out; }
