#include "did.h"
#include "libtxref.h"
#include <algorithm>
#include <stdexcept>

namespace {
    const char schemeAndMethod[] = "did:btcr:";

    void validateDidString(const std::string &did) {
        if (did.find(schemeAndMethod) != 0) {
            throw std::runtime_error(
                    "DID parameter not a valid BTCR DID. Should be of the form 'did:btcr:<txref>'");
        }
    }
}

Did::Did(const std::string &did, const BitcoinRPCFacade & btc) {
    // ensure lowercase
    std::string localDid = did;
    std::transform(localDid.begin(), localDid.end(), localDid.begin(), &::tolower);

    validateDidString(localDid);

    localDid.erase(0, sizeof(schemeAndMethod)-1);

    txref::InputParam inputParam = txref::classifyInputString(localDid);

    if(inputParam != txref::InputParam::txref && inputParam != txref::InputParam::txrefext) {
        throw std::runtime_error(
                "DID parameter doesn't contain a valid txref. Should be of the form 'did:btcr:<txref>'");
    }

    txref = std::make_shared<Txref>(localDid, btc);
}

const std::shared_ptr<Txref> &Did::getTxref() const {
    return txref;
}
