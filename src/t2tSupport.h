#ifndef TXREF_T2TSUPPORT_H
#define TXREF_T2TSUPPORT_H

#include "txid2txref.h"
#include "bitcoinRPCFacade.h"

namespace t2t {

    bool isNetworkMismatch(const std::string& hrp, const std::string& networkName);

    void encodeTxid(const BitcoinRPCFacade & btc, const std::string & txid, int txoIndex, struct Transaction & transaction);

    void decodeTxref(const BitcoinRPCFacade & btc, const std::string & txid, struct Transaction & transaction);

}

#endif //TXREF_T2TSUPPORT_H
