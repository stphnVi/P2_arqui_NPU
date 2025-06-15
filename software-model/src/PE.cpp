// PE (Processing Element) 
class PE {
private:
    int32_t maccout; //mac counter out
    int32_t west_c, north_c;
    
public:
    PE() : maccout(0), west_c(0), north_c(0) {}
    
    void reset() {
        maccout = 0;
        west_c = 0;
        north_c = 0;
    }
    
    // Returns {out_east, out_south, psum}
    std::tuple<int8_t, int8_t, int32_t> process(int8_t in_west, int8_t in_north, int state) {
        int32_t product = in_west * in_north;
        
        // Negative edge clock behavior
        if (state == 1) { // READ state
            maccout += product;
            west_c = in_west;
            north_c = in_north;
        }
        
        // Positive edge clock behavior
        if (state == 0) { // IDLE state
            maccout = 0;
        }
        
        return std::make_tuple(west_c, north_c, maccout);
    }
    
    int32_t getPsum() const { return maccout; }
};
