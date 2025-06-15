#include "Controller.h"

Controller::Controller() : state(IDLE), n_state(IDLE), 
                          counter(0), counter_out(0), 
                          idx_a(0), idx_b(0) {}

void Controller::reset() {
    state = IDLE;
    n_state = IDLE;
    counter = 0;
    counter_out = 0;
    idx_a = 0;
    idx_b = 0;
}

void Controller::update(bool /*in_valid*/, bool busy, int K_reg, int /*M_reg*/, int /*N_reg*/) {
    state = n_state;
    
    switch (state) {
        case IDLE:
            if (busy) {
                n_state = READ;
                counter = 0;
                idx_a = 0;
                idx_b = 0;
            }
            break;
            
        case READ:
            counter++;
            
            // Update indices for data feeding
            if (counter <= K_reg + 6) { // Extra cycles for pipeline flush
                // Staggered data injection pattern
                // We need to inject data with proper delays
                idx_a = counter - 1;
                idx_b = counter - 1;
            }
            
            // Transition to WRITE after all data is processed
            if (counter >= K_reg + 10) { // Ensure all computations complete
                n_state = WRITE;
                counter_out = 0;
            }
            break;
            
        case WRITE:
            counter_out++;
            if (counter_out >= 4) {
                n_state = FINISH;
            }
            break;
            
        case FINISH:
            n_state = IDLE;
            break;
    }
}

int Controller::getState() const { return state; }
int Controller::getCounter() const { return counter; }
int Controller::getIdxA() const { return idx_a; }
int Controller::getIdxB() const { return idx_b; }
bool Controller::getCWrEn() const { return state == WRITE; }
int Controller::getCounterOut() const { return counter_out; }