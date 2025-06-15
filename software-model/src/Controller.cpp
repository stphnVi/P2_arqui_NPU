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
    // Update state first
    state = n_state;
    
    // Simple state machine for single 4x4 block
    switch (state) {
        case IDLE:
            if (busy) {
                n_state = READ;
                counter = 0;
            }
            break;
        case READ:
            counter++;
            if (counter >= K_reg + 6) { // Allow for pipeline delays
                n_state = WRITE;
                counter_out = 0;
            }
            break;
        case WRITE:
            counter_out++;
            if (counter_out >= 4) { // Output 4 rows
                n_state = FINISH;
            }
            break;
        case FINISH:
            n_state = IDLE;
            break;
    }
    
    // Update indices
    updateIndices(K_reg, M_reg, N_reg, busy);
}

void Controller::updateIndices(int K_reg, int /*M_reg*/, int /*N_reg*/, bool /*busy*/) {
    // Simple indexing for single block
    if (state == READ && counter > 0 && counter <= K_reg) {
        // For matrix A: advance through words as needed
        if ((counter - 1) % 4 == 3 || counter == K_reg) {
            // Move to next word when we've consumed 4 elements
            // But only if we're not already at the end
            int K_words = (K_reg + 3) / 4;
            if (idx_a < K_words - 1) {
                idx_a++;
            }
        }
        
        // For matrix B: same logic
        if ((counter - 1) % 4 == 3 || counter == K_reg) {
            int K_words = (K_reg + 3) / 4;
            if (idx_b < K_words - 1) {
                idx_b++;
            }
        }
    }
    
    if (state == FINISH) {
        idx_a = 0;
        idx_b = 0;
        idx_c = 0;
    }
}

// Getters
int Controller::getState() const { return state; }
int Controller::getCounter() const { return counter; }
int Controller::getIdxA() const { return idx_a; }
int Controller::getIdxB() const { return idx_b; }
int Controller::getIdxC() const { return idx_c; }
bool Controller::getCWrEn() const { return state == WRITE; }
int Controller::getCounterOut() const { return counter_out; }
