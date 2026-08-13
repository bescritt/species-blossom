#include <iostream>
#include <cmath>
#include "../randomc.h"

int main() {
    // deterministic check for BRandom: two generators with same seed produce same first value
    TRandomMersenne r(42);
    TRandomMersenne r2(42);
    unsigned long br1 = r.BRandom();
    unsigned long br2 = r2.BRandom();
    if(br1 != br2) {
        std::cerr << "BRandom not deterministic: " << br1 << " vs " << br2 << std::endl;
        return 4;
    }
    // test Random in [0,1)
    double v = r.Random();
    if(!(v >= 0.0 && v < 1.0)) {
        std::cerr << "Random out of range: " << v << std::endl;
        return 2;
    }
    // test IRandom deterministic (range check)
    long a = r.IRandom(0,10);
    long b = r.IRandom(0,10);
    if(a < 0 || a > 10 || b < 0 || b > 10) {
        std::cerr << "IRandom out of range: " << a << "," << b << std::endl;
        return 3;
    }
    std::cout << "mersenne: ok" << std::endl;
    return 0;
}
