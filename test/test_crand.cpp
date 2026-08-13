#include <iostream>
#include "../rand.hpp"

int main() {
    float f = CRand::frand(1.0f);
    if(!(f >= 0.0f && f < 1.0f)) {
        std::cerr << "frand out of range: " << f << std::endl;
        return 2;
    }
    int i = CRand::irand(10);
    if(!(i >= 0 && i < 10)) {
        std::cerr << "irand out of range: " << i << std::endl;
        return 3;
    }
    std::cout << "crand: ok" << std::endl;
    return 0;
}
