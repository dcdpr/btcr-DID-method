#include <stdexcept>
#include "vout.h"

Vout::Vout(int i) {
    if(i < 0)
        throw std::runtime_error("index can not be less than zero");

    index = i;
}

int Vout::value() const {
    return index;
}

bool Vout::operator==(const Vout &rhs) const {
    return index == rhs.index;
}

bool Vout::operator!=(const Vout &rhs) const {
    return !(rhs == *this);
}
