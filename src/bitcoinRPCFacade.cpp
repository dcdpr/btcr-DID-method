#include "bitcoinRPCFacade.h"

#include <bitcoinapi/bitcoinapi.h>

namespace {

    int MAINNET_PORT = 8332;
    int TESTNET_PORT = 18332;

    bool isConnectionGood(BitcoinAPI *b) {
        try {
            b->getblockcount();
        }
        catch (BitcoinException &)
        {
            return false;
        }
        return true;
    }
}

BitcoinRPCFacade::BitcoinRPCFacade(
        const RpcConfig & config) {

    std::stringstream ss;
    if(config.rpcport != 0) {
        bitcoinAPI = new BitcoinAPI(config.rpcuser, config.rpcpassword, config.rpchost, config.rpcport);
        if (isConnectionGood(bitcoinAPI))
            return;
        ss << "Error: Can't connect to " << config.rpchost << " on port " << config.rpcport;
    }
    else {
        bitcoinAPI = new BitcoinAPI(config.rpcuser, config.rpcpassword, config.rpchost, MAINNET_PORT);
        if (isConnectionGood(bitcoinAPI))
            return;
        bitcoinAPI = new BitcoinAPI(config.rpcuser, config.rpcpassword, config.rpchost, TESTNET_PORT);
        if (isConnectionGood(bitcoinAPI))
            return;
        ss << "Error: Can't connect to " << config.rpchost << " on either port (" << MAINNET_PORT << "," << TESTNET_PORT << ")";
    }
    throw std::runtime_error(ss.str());
}

BitcoinRPCFacade::~BitcoinRPCFacade() = default;

getrawtransaction_t BitcoinRPCFacade::getrawtransaction(const std::string &txid, int verbose) const {
    return bitcoinAPI->getrawtransaction(txid, verbose);
}

blockinfo_t BitcoinRPCFacade::getblock(const std::string &blockhash) const {
    return bitcoinAPI->getblock(blockhash);
}

std::string BitcoinRPCFacade::getblockhash(int blocknumber) const {
    return bitcoinAPI->getblockhash(blocknumber);
}

blockchaininfo_t BitcoinRPCFacade::getblockchaininfo() const {
    std::string command = "getblockchaininfo";
    Value params, result;
    blockchaininfo_t ret;
    result = bitcoinAPI->sendcommand(command, params);

    ret.chain = result["chain"].asString();
    ret.blocks = result["blocks"].asInt();
    ret.headers = result["headers"].asInt();
    ret.bestblockhash = result["bestblockhash"].asString();
    ret.difficulty = result["difficulty"].asDouble();
    ret.mediantime = result["mediantime"].asInt();
    ret.verificationprogress = result["verificationprogress"].asDouble();
    ret.chainwork = result["chainwork"].asString();
    ret.pruned = result["pruned"].asBool();
    ret.pruneheight = result["pruneheight"].asInt();

    return ret;
}

utxoinfo_t BitcoinRPCFacade::gettxout(const std::string &txid, int n) const {
    return bitcoinAPI->gettxout(txid, n);
}

std::string BitcoinRPCFacade::createrawtransaction(
        const std::vector<txout_t> &inputs,
        const std::map<std::string, double> &amounts) const {
    return bitcoinAPI->createrawtransaction(inputs, amounts);
}

std::string BitcoinRPCFacade::createrawtransaction(
        const std::vector<txout_t> &inputs,
        const std::map<std::string, std::string> &amounts) const {
    return bitcoinAPI->createrawtransaction(inputs, amounts);
}

std::string BitcoinRPCFacade::sendrawtransaction(const std::string &hexString) const {
    std::string command = "sendrawtransaction";
    Value params, result;
    params.append(hexString);
    // not sending anything for maxfeerate
    result = bitcoinAPI->sendcommand(command, params);

    return result.asString();
}

std::string
BitcoinRPCFacade::signrawtransactionwithkey(
        const std::string &rawTx, const std::vector<signrawtxinext_t> &inputs,
        const std::vector<std::string> &privkeys, const std::string &sighashtype) const {

    std::string command = "signrawtransactionwithkey";
    Value params, result;

    params.append(rawTx);

    Value privkeyValues(Json::arrayValue);
    for(const auto & privkey : privkeys){
        privkeyValues.append(privkey);
    }

    Value inputValues(Json::arrayValue);
    for(const auto & input : inputs){
        Value val;
        val["txid"] = input.txid;
        val["vout"] = input.n;
        val["scriptPubKey"] = input.scriptPubKey;
        if(!input.redeemScript.empty()){
            val["redeemScript"] = input.redeemScript;
        }
        val["amount"] = input.amount;
        inputValues.append(val);
    }

    params.append(privkeyValues);
    params.append(inputValues);
    params.append(sighashtype);

    result = bitcoinAPI->sendcommand(command, params);

    if(result["complete"].asBool()) {
        return result["hex"].asString();
    }
    return "";
}


