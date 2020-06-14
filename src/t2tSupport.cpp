#include "t2tSupport.h"
#include "libtxref.h"
#include "txid2txref.h"
#include "bitcoinRPCFacade.h"

#include <bitcoinapi/types.h>
#include <iostream>

namespace t2t {

    void encodeTxid(const BitcoinRPCFacade & btc, const std::string & txid, int txoIndex, struct Transaction & transaction) {

        blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

        // determine what network we are on
        bool isTestnet = blockChainInfo.chain == "test";

        // use txid to call getrawtransaction to find the blockhash
        getrawtransaction_t rawTransaction = btc.getrawtransaction(txid, 1);
        std::string blockHash = rawTransaction.blockhash;

        // use blockhash to call getblock to find the block height
        blockinfo_t blockInfo = btc.getblock(blockHash);
        int blockHeight = blockInfo.height;

        // warn if #confirmations are too low
        int numConfirmations = blockInfo.confirmations;
        if (numConfirmations < 6) {
            std::cerr << "Warning: 6 confirmations are required for a valid txref: only "
                      << numConfirmations << " found." << std::endl;
        }

        // go through block's transaction array to find transaction position
        std::vector<std::string> blockTransactions = blockInfo.tx;
        std::vector<std::string>::size_type blockIndex;
        for (blockIndex = 0; blockIndex < blockTransactions.size(); ++blockIndex) {
            std::string blockTxid = blockTransactions.at(blockIndex);
            if (blockTxid == txid)
                break;
        }

        if (blockIndex == blockTransactions.size()) {
            std::cerr << "Error: Could not find transaction " << txid
                      << " within the block." << std::endl;
            std::exit(-1);
        }

        // verify that the txoIndex provided on command line is valid for this txid
        auto numTxos = static_cast<int>(rawTransaction.vout.size());
        if(txoIndex >= numTxos) {
            std::cerr << "Error: txoIndex provided [" << txoIndex
                      << "] is too large for transaction " << txid << std::endl;
            std::exit(-1);
        }

        // call txref code with block height, transaction position, and txoIndex (if provided) to get txref
        std::string txref;
        if (isTestnet) {
            txref = txref::encodeTestnet(
                    blockHeight, static_cast<int>(blockIndex), txoIndex, false);
        } else {
            txref = txref::encode(
                    blockHeight, static_cast<int>(blockIndex), txoIndex, false);
        }

        // output
        transaction.query = txid;
        transaction.txid = txid;
        transaction.txref = txref;
        transaction.blockHeight = blockHeight;
        transaction.position = static_cast<int>(blockIndex);
        transaction.network = blockChainInfo.chain;
        transaction.txoIndex = txoIndex;
    }

    void decodeTxref(const BitcoinRPCFacade &btc, const std::string & txref, struct Transaction &transaction) {

        txref::LocationData locationData = txref::decode(txref);

        blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

        // get block hash for block
        std::string blockHash = btc.getblockhash(locationData.blockHeight);

        // use block hash to get the block info
        blockinfo_t blockInfo = btc.getblock(blockHash);

        // get the txid from the transaction
        std::string txid;
        try {
            txid = blockInfo.tx.at(
                    static_cast<unsigned long>(locationData.transactionPosition));
        }
        catch (std::out_of_range &) {
            std::cerr << "Error: Could not find txid for transactionPosition '" << locationData.transactionPosition
                      << "' within the block." << std::endl;
            std::exit(-1);
        }

        // output
        transaction.query = txref;
        transaction.txid = txid;
        transaction.txref = locationData.txref;
        transaction.blockHeight = locationData.blockHeight;
        transaction.position = locationData.transactionPosition;
        transaction.txoIndex = locationData.txoIndex;
        transaction.network = blockChainInfo.chain;
    }

}
