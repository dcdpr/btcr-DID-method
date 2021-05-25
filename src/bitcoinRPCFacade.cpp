#include "bitcoinRPCFacade.h"

#include <bitcoinapi/bitcoinapi.h>

namespace {

    int MAINNET_PORT = 8332;
    int TESTNET_PORT = 18332;

    // blockhash for block 0 can be found with "bitcoin-cli getblockhash 0"
    char block_0_hash[] = "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f";
    // hex for transaction 0 can be found with "bitcoin-cli getblock 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f 2 | jq '.tx[0].hex'"
    char block_0_tx_0_hex[] = "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff4d04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000";

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
        bitcoinAPI = new BitcoinAPI(config.rpcuser, config.rpcpassword, config.rpcconnect, config.rpcport);
        if (isConnectionGood(bitcoinAPI))
            return;
        ss << "Error: Can't connect to " << config.rpcconnect << " on port " << config.rpcport;
    }
    else {
        bitcoinAPI = new BitcoinAPI(config.rpcuser, config.rpcpassword, config.rpcconnect, MAINNET_PORT);
        if (isConnectionGood(bitcoinAPI))
            return;
        bitcoinAPI = new BitcoinAPI(config.rpcuser, config.rpcpassword, config.rpcconnect, TESTNET_PORT);
        if (isConnectionGood(bitcoinAPI))
            return;
        ss << "Error: Can't connect to " << config.rpcconnect << " on either port (" << MAINNET_PORT << "," << TESTNET_PORT << ")";
    }
    throw std::runtime_error(ss.str());
}

BitcoinRPCFacade::~BitcoinRPCFacade() = default;

getrawtransaction_t BitcoinRPCFacade::getrawtransaction(const std::string &txid, int verbose) const {
    if(txid != "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b")
        return bitcoinAPI->getrawtransaction(txid, verbose);
    else {
        // if we are not on mainnet, just call out to the api
        blockchaininfo_t blockChainInfo = getblockchaininfo();
        if(blockChainInfo.chain != "main")
            return bitcoinAPI->getrawtransaction(txid, verbose);

        // mainnet genesis transaction 4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b
        // can not be retrieved from bitcoind, so we will fake it here just enough to allow callers of
        // getrawtransaction() to work with it.
        getrawtransaction_t tx;
        tx.blockhash = block_0_hash;
        tx.hex = block_0_tx_0_hex;
        tx.vout.resize(1);
        return tx;
    }
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
        if(!input.witnessScript.empty()){
            val["witnessScript"] = input.witnessScript;
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

btcaddressinfo_t BitcoinRPCFacade::getaddressinfo(const std::string &address) const {
    std::string command = "getaddressinfo";
    Value params, result;

    params.append(address);

    result = bitcoinAPI->sendcommand(command, params);

    btcaddressinfo_t ret;

    ret.address = result["address"].asString();
    ret.scriptPubKey = result["scriptPubKey"].asString();
    ret.ismine = result["ismine"].asBool();
    ret.iswatchonly = result["iswatchonly"].asBool();
    ret.solvable = result["solvable"].asBool();
    ret.desc = result["desc"].asString();
    ret.isscript = result["isscript"].asBool();
    ret.ischange = result["ischange"].asBool();
    ret.iswitness = result["iswitness"].asBool();
    ret.witness_version = result["witness_version"].asInt();
    ret.witness_program = result["witness_program"].asString();
    ret.script = result["script"].asString();
    ret.hex = result["hex"].asString();
//array?    ret.pubkeys = result["pubkeys"]; // Array of pubkeys associated with the known redeemscript (only if "script" is "multisig")
    ret.sigsrequired = result["sigsrequired"].asInt();
    ret.pubkey = result["pubkey"].asString();
    //ret.embedded = result["embedded"]; // {...},           (object, optional) Information about the address embedded in P2SH or P2WSH, if relevant and known. It includes all getaddressinfo output fields for the embedded address, excluding metadata ("timestamp", "hdkeypath", "hdseedid") and relation to the wallet ("ismine", "iswatchonly").
    ret.iscompressed = result["iscompressed"].asBool();
    ret.label = result["label"].asString();
    ret.timestamp = result["timestamp"].asInt();
    ret.hdkeypath = result["hdkeypath"].asString();
    ret.hdseedid = result["hdseedid"].asString();
    ret.hdmasterfingerprint = result["hdmasterfingerprint"].asString();
//array?    ret.labels = result["labels"]; // Array of labels associated with the address.

    return ret;
}


