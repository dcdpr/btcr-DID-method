#ifndef TXREF_BITCOINRPCFACADE_H
#define TXREF_BITCOINRPCFACADE_H

// Facade class that wraps the BitcoinApi objects

#include <string>
#include <vector>
#include <map>

// forward decls from bitcoinaapi
class BitcoinAPI;
struct getrawtransaction_t;
struct blockinfo_t;
struct utxoinfo_t;
struct txout_t;

// struct for local impl of getblockchaininfo()
struct blockchaininfo_t {
    std::string chain;
    int blocks;
    int headers;
    std::string bestblockhash;
    double difficulty;
    int mediantime;
    double verificationprogress;
    std::string chainwork;
    bool pruned;
    int pruneheight;
    //std::vector<...> softforks;
    //std::vector<...> bip9_softforks;
};

// struct for local impl of signrawtxin_t
struct signrawtxinext_t {
    std::string txid;
    unsigned int n;
    std::string scriptPubKey;
    std::string redeemScript;
    std::string amount;
};


struct RpcConfig {
    std::string rpcuser = "";
    std::string rpcpassword ="";
    std::string rpchost = "127.0.0.1";
    int rpcport = 0;
};

class BitcoinRPCFacade {

private:
    BitcoinAPI *bitcoinAPI;

public:

    // constructor will throw if an RPC connection can't be made to the bitcoind
    explicit BitcoinRPCFacade(
            const RpcConfig & config);

    virtual ~BitcoinRPCFacade();

    // forwards to existing bitcoinapi functions
    virtual getrawtransaction_t getrawtransaction(const std::string& txid, int verbose) const;
    virtual blockinfo_t getblock(const std::string& blockhash) const;
    virtual std::string getblockhash(int blocknumber) const;
    virtual utxoinfo_t gettxout(const std::string& txid, int n) const;

    virtual std::string createrawtransaction(const std::vector<txout_t>& inputs, const std::map<std::string, double>& amounts) const;
    virtual std::string createrawtransaction(const std::vector<txout_t>& inputs, const std::map<std::string, std::string>& amounts) const;

    // implement missing bitcoinapi functions
    virtual blockchaininfo_t getblockchaininfo() const;
    virtual std::string signrawtransactionwithkey(const std::string& rawTx, const std::vector<signrawtxinext_t> & inputs, const std::vector<std::string>& privkeys, const std::string& sighashtype) const;

    // re-implement out-of-date bitcoinapi functions
    virtual std::string sendrawtransaction(const std::string& hexString) const;

protected:
    BitcoinRPCFacade() = default;
};


#endif //TXREF_BITCOINRPCFACADE_H
