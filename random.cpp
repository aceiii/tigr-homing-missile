#include "random.h"

int randomInt(int min, int max) {
    static std::mt19937 gen {(std::random_device {})()};
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}
