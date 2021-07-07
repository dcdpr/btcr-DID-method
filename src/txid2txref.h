#ifndef TXREF_TXID2TXREF_H
#define TXREF_TXID2TXREF_H

#include <string>

namespace t2t {

    struct Transaction {
        std::string txid = "";
        std::string txref = "";
        std::string network = "";
        int blockHeight = 0;
        int transactionIndex = 0;
        int txoIndex = 0;
        std::string query = "";
    };

}

#endif //TXREF_TXID2TXREF_H
