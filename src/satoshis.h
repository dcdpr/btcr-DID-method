#ifndef TXREF_SATOSHIS_H
#define TXREF_SATOSHIS_H

// BTC <-> satoshi conversions

#include <cstdint>
#include <cmath>
#include <ios>
#include <iomanip>
#include <limits>

const int SATOSHIS_PER_BTC = 100000000;

inline int64_t btc2satoshi(double value) {
    return static_cast<int64_t>(value * SATOSHIS_PER_BTC + (value < 0.0 ? -.5 : .5));
}

inline double satoshi2btc(int64_t value) {
    return static_cast<double>(value) / SATOSHIS_PER_BTC;
}

inline std::string btc_to_string(double d) {

    static const int WIDTH =
            static_cast<int>(std::ceil(std::log10(std::numeric_limits<uint64_t>::max())) + 1); // 1 for decimal
    static const int PRECISION =
            static_cast<int>(std::ceil(std::log10(double(SATOSHIS_PER_BTC))));

    std::stringstream s;
    s << std::fixed
      << std::setw(WIDTH)
      << std::setprecision(PRECISION)
      << d;
    // trim extra spaces from start
    auto start = s.str().find_first_not_of(' ');
    if(start == std::string::npos)
        return "";
    return s.str().substr(start);
}

#endif //TXREF_SATOSHIS_H
