#include "Controller.h"

Controller::Controller() : state(IDLE), n_state(IDLE), out_cycle(0), 
                          a_offset(0), b_offset(0), counter_a(0), counter_b(0), 
                          counter(0), counter_out(0), idx_a(0), idx_b(0), idx_c(0) {}

void Controller::reset() {
    state = IDLE;
    n_state = IDLE;
    out_cycle = 0;
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

void Controller::update(bool in_valid, bool busy, int K_reg, int M_reg, int N_reg) {
    // Update state
    state = n_state;
    
    // Calculate block offsets
    a_offset = (M_reg + 3) / 4;
    b_offset = (N_reg + 3) / 4;
    
    // State transition logic
    switch (state) {
        case IDLE:
            n_state = (in_valid || busy) ? READ : IDLE;
            break;
        case READ:
            n_state = (counter <= (K_reg + 6)) ? READ : WRITE;
            break;
        case WRITE:
            n_state = (counter_out < out_cycle) ? WRITE : 
                     (counter_b == b_offset) ? FINISH : IDLE;
            break;
        case FINISH:
            n_state = IDLE;
            break;
    }
    
    // Update counters
    if (state == READ) {
        counter++;
    } else {
        counter = 0;
    }
    
    if (state == WRITE) {
        counter_out++;
    } else {
        counter_out = 0;
    }
    
    // Update output cycle
    if (state == READ && busy) {
        out_cycle = (counter_a == (a_offset - 1) && (M_reg & 3) != 0) ? 
                   (M_reg & 3) : 4;
    }
    
    // Update buffer indices
    updateIndices(K_reg, M_reg, N_reg, busy);
}

void Controller::updateIndices(int K_reg, int M_reg, int N_reg, bool busy) {
    // Update idx_a
    if (state == WRITE) {
        if (K_reg == 1) {
            idx_a = 1;
        } else if (n_state == IDLE) {
            idx_a++;
        }
    } else if (state == IDLE && counter_a == a_offset) {
        idx_a = 0;
    } else if (state == FINISH) {
        idx_a = 0;
    } else if (state == READ && counter < K_reg - 1) {
        idx_a++;
    }
    
    // Update idx_b
    if (state == IDLE && busy) {
        idx_b = K_reg * counter_b;
    } else if (state == FINISH) {
        idx_b = 0;
    } else if (state == READ && counter < K_reg) {
        idx_b++;
    }
    
    // Update idx_c
    if (state == FINISH) {
        idx_c = 0;
    } else if (state == WRITE && n_state == WRITE) {
        idx_c++;
    }
}

// Getters
int Controller::getState() const { return state; }
int Controller::getCounter() const { return counter; }
int Controller::getIdxA() const { return idx_a; }
int Controller::getIdxB() const { return idx_b; }
int Controller::getIdxC() const { return idx_c; }
bool Controller::getCWrEn() const { return n_state == WRITE; }
int Controller::getCounterOut() const { return counter_out; }
