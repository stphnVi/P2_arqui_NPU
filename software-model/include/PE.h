#ifndef PE_H
#define PE_H

#include <cstdint>
#include <tuple>

class PE {
private:
    int32_t maccout;
    int32_t west_c, north_c;
    bool relu_enabled;
    
    // Apply ReLU to accumulator value
    int32_t applyReLU(int32_t value) const;
    
public:
    PE();
    void reset();
    void enableReLU(bool enable = true);
    std::tuple<int8_t, int8_t, int32_t> process(int8_t in_west, int8_t in_north, int state);
    int32_t getPsum() const;
};

#endif // PE_H

