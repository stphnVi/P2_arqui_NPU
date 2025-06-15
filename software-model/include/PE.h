#ifndef PE_H
#define PE_H

#include <cstdint>
#include <tuple>

class PE {
private:
    int32_t maccout;
    int32_t west_c, north_c;
    
public:
    PE();
    void reset();
    std::tuple<int8_t, int8_t, int32_t> process(int8_t in_west, int8_t in_north, int state);
    int32_t getPsum() const;
};

#endif // PE_H
