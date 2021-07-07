#include <stdexcept>
#include "blockHeight.h"

BlockHeight::BlockHeight(int h) {
    if(h < 0)
        throw std::runtime_error("height can not be less than zero");

    height = h;
}

int BlockHeight::value() const {
    return height;
}

bool BlockHeight::operator==(const BlockHeight &rhs) const {
    return height == rhs.height;
}

bool BlockHeight::operator!=(const BlockHeight &rhs) const {
    return !(rhs == *this);
}
