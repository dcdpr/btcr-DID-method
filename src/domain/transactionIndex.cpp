#include <stdexcept>
#include "transactionIndex.h"

TransactionIndex::TransactionIndex(int i) {
    if(i < 0)
        throw std::runtime_error("index can not be less than zero");

    index = i;
}

int TransactionIndex::value() const {
    return index;
}

bool TransactionIndex::operator==(const TransactionIndex &rhs) const {
    return index == rhs.index;
}

bool TransactionIndex::operator!=(const TransactionIndex &rhs) const {
    return !(rhs == *this);
}
