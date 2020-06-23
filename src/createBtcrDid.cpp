#include "bitcoinRPCFacade.h"
#include "encodeOpReturnData.h"
#include "satoshis.h"
#include "classifyInputString.h"
#include "txid2txref.h"
#include "t2tSupport.h"
#include "anyoption.h"
#include <iostream>
#include <cstdlib>
#include <memory>
#include <bitcoinapi/bitcoinapi.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

struct CmdlineInput {
    std::string query;
    int txoIndex = -1;
    std::string outputAddress;
    std::string privateKey;
    std::string ddoRef;
    double fee = 0.0;
    bool dryrun = false;
};

struct UnspentData {
    std::string address;
    std::string txid;
    std::string scriptPubKeyHex;
    std::string redeemScript;
    std::string witnessScript;
    int64_t amountSatoshis = 0;
    int utxoIndex = 0;
};


t2t::Transaction getTransaction(const CmdlineInput &cmdlineInput, const BitcoinRPCFacade &btc) {

    // cmdlineInput.txoIndex should be -1 here, meaning it was not specified on the command line. If it
    // isn't -1, it won't really matter because decodeTxref() doesn't even use it, but we
    // should output a message afterward.

    t2t::Transaction transaction;
    t2t::decodeTxref(btc, cmdlineInput.query, transaction);

    if(cmdlineInput.txoIndex >= 0 && (cmdlineInput.txoIndex == transaction.txoIndex)) {
        std::cerr << "Warning: passing txoIndex '" << cmdlineInput.txoIndex
                  << "' on the command line was not needed as the value of '" << transaction.txoIndex
                  << "' is encoded within your txref.\n";
    }
    else if(cmdlineInput.txoIndex > 0) {
        std::cerr << "Error: passing txoIndex '" << cmdlineInput.txoIndex
                  << "' on the command line is not supported as the value of '" << transaction.txoIndex
                  << "' is encoded within your txref.\n";
        std::exit(-1);
    }

    return transaction;
}

std::string extractWitnessScript(const btcaddressinfo_t &btcaddressinfo) {
    if(btcaddressinfo.iswitness) {
        return btcaddressinfo.witness_program;
    }
    return "";
}

std::string extractRedeemScript(const btcaddressinfo_t &btcaddressinfo) {
    if(btcaddressinfo.isscript) {
        if(btcaddressinfo.script == "witness_v0_keyhash")
            return btcaddressinfo.hex;
    }
    return "";
}

std::string find_homedir() {
    std::string ret;
    char * home = getenv("HOME");
    if(home != nullptr)
        ret.append(home);
    return ret;
}

int convertIntegerArg(const std::string & argName, AnyOption *opt) {
    int i;
    try {
        i = std::stoi(opt->getValue(argName.c_str()));
    }
    catch(std::invalid_argument &) {
        std::cerr << "Error: " << argName << " '" << opt->getValue(argName.c_str())
                  << "' is invalid. Check command line usage.\n";
        opt->printUsage();
        std::exit(-1);
    }
    catch(std::out_of_range &) {
        std::cerr << "Error: " << argName << " '" << opt->getValue(argName.c_str())
                  << "' is invalid. Check command line usage.\n";
        opt->printUsage();
        std::exit(-1);
    }
    return i;
}

double convertDoubleArg(int argPosition, AnyOption *opt) {
    double d;
    try {
        d = std::stod(opt->getArgv(argPosition));
    }
    catch(std::invalid_argument &) {
        std::cerr << "Error: argument at position " << argPosition << " '" << opt->getArgv(argPosition)
                  << "' is invalid. Check command line usage.\n";
        opt->printUsage();
        std::exit(-1);
    }
    catch(std::out_of_range &) {
        std::cerr << "Error: argument at position " << argPosition << " '" << opt->getArgv(argPosition)
                  << "' is invalid. Check command line usage.\n";
        opt->printUsage();
        std::exit(-1);
    }
    return d;
}

int parseCommandLineArgs(int argc, char **argv,
                         struct RpcConfig &rpcConfig,
                         struct CmdlineInput &cmdlineInput) {

    auto opt = std::unique_ptr<AnyOption>(new AnyOption());
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: createBtcrDid [options] <inputXXX> <outputAddress> <private key> <fee> <ddoRef>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( " --txoIndex [index]         Index # of which TXO to use from the input transaction (default: 0) " );
    opt->addUsage( " -n --dryrun                Do everything except submit transaction to blockchain" );
    opt->addUsage( "" );
    opt->addUsage( "<inputXXX>      input: (bitcoin address, txid, txref) needs at least slightly more unspent BTCs than your offered fee" );
    opt->addUsage( "<outputAddress> output bitcoin address: will receive transaction change and be the basis for your DID" );
    opt->addUsage( "<private key>   private key in base58 (wallet import format)" );
    opt->addUsage( "<fee>           fee you are willing to pay (suggestion: >0.001 BTC)" );
    opt->addUsage( "<ddoRef>        reference to a DDO you want as part of your DID (optional)" );

    opt->setFlag("help", 'h');
    opt->setFlag("dryrun", 'n');
    opt->setOption("rpchost");
    opt->setOption("rpcuser");
    opt->setOption("rpcpassword");
    opt->setOption("rpcport");
    opt->setCommandOption("config");
    opt->setOption("txoIndex");

    // parse any command line arguments--this is a first pass, mainly to get a possible
    // "config" option that tells if the bitcoin.conf file is in a non-default location
    opt->processCommandArgs( argc, argv );

    // print usage if no options
    if( ! opt->hasOptions()) {
        opt->printUsage();
        return 0;
    }

    // see if there is a bitcoin.conf file to parse. If not, continue.
    if (opt->getValue("config") != nullptr) {
        opt->processFile(opt->getValue("config"));
    }
    else {
        std::string home = find_homedir();
        if(!home.empty()) {
            std::string configPath = home + "/.bitcoin/bitcoin.conf";
            if(!opt->processFile(configPath.data())) {
                std::cerr << "Warning: Config file " << configPath
                          << " not readable. Perhaps try --config? Attempting to continue...\n";
            }
        }
    }

    // parse command line arguments AGAIN--this is because command line args should override config file
    opt->processCommandArgs( argc, argv );


    // print usage if help was requested
    if (opt->getFlag("help") || opt->getFlag('h')) {
        opt->printUsage();
        return 0;
    }

    // was dryrun requested?
    if (opt->getFlag("dryrun") || opt->getFlag('n')) {
        cmdlineInput.dryrun = true;
    }

    // see if there is an rpchost specified. If not, use default
    if (opt->getValue("rpchost") != nullptr) {
        rpcConfig.rpchost = opt->getValue("rpchost");
    }

    // see if there is an rpcuser specified. If not, exit
    if (opt->getValue("rpcuser") == nullptr) {
        std::cerr << "Error: 'rpcuser' not found. Check bitcoin.conf or command line usage.\n";
        opt->printUsage();
        return -1;
    }
    rpcConfig.rpcuser = opt->getValue("rpcuser");

    // see if there is an rpcpassword specified. If not, exit
    if (opt->getValue("rpcpassword") == nullptr) {
        std::cerr << "Error: 'rpcpassword' not found. Check bitcoin.conf or command line usage.\n";
        opt->printUsage();
        return -1;
    }
    rpcConfig.rpcpassword = opt->getValue("rpcpassword");

    // will try both well known ports (8332 and 18332) if one is not specified
    if (opt->getValue("rpcport") != nullptr) {
        rpcConfig.rpcport = convertIntegerArg("rpcport", opt.get());
    }

    // see if a txoIndex was provided.
    if (opt->getValue("txoIndex") != nullptr) {
        cmdlineInput.txoIndex = convertIntegerArg("txoIndex", opt.get());
        if(cmdlineInput.txoIndex < 0) {
            std::cerr << "Error: txoIndex '" << cmdlineInput.txoIndex << "' should be zero or greater. Check command line usage.\n";
            opt->printUsage();
            return -1;
        }
    }

    // get the positional arguments
    if(opt->getArgc() < 4) {
        std::cerr << "Error: all required arguments not found. Check command line usage.\n";
        opt->printUsage();
        return -1;
    }
    cmdlineInput.query = opt->getArgv(0);
    cmdlineInput.outputAddress = opt->getArgv(1);
    cmdlineInput.privateKey = opt->getArgv(2);
    cmdlineInput.fee = convertDoubleArg(3, opt.get());
    if(opt->getArgv(4) != nullptr)
        cmdlineInput.ddoRef = opt->getArgv(4);

    // TODO validate position arguments

    return 1;
}

void printAsJson(const std::string & txid) {
    pt::ptree root;

    if(!txid.empty()) {
        root.put("comment", "transaction submitted");
        root.put("txid", txid);
        root.put("txoIndex", 1); // TODO: always 1 for now
    }
    else
        root.put("error", "the network did not accept our transaction");

    pt::write_json(std::cout, root);
}


int main(int argc, char *argv[]) {

    struct RpcConfig rpcConfig;
    struct CmdlineInput cmdlineInput;

    int ret = parseCommandLineArgs(argc, argv, rpcConfig, cmdlineInput);
    if(ret < 1) {
        std::exit(ret);
    }

    try {
        BitcoinRPCFacade btc(rpcConfig);

        blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

        // 0. Determine InputType

        InputParam inputParam = classifyInputString(cmdlineInput.query);

        // 1. Get unspent transaction output data

        t2t::Transaction transaction;
        UnspentData unspentData;

        if(inputParam == InputParam::address_param) {
            // get unspent amount and txid from inputAddress
            // TODO: disabling this for now since it wasn't implemented correctly
            std::cerr << "Error: creating a DID from a BTC address is not currently supported.\n";
            std::exit(-1);
        }
        else if(inputParam == InputParam::txref_param || inputParam == InputParam::txrefext_param) {
            // decode txref
            transaction = getTransaction(cmdlineInput, btc);
        }
        else if(inputParam == InputParam::txid_param) {
            transaction.query = cmdlineInput.query;
            transaction.txid = cmdlineInput.query;
            transaction.txoIndex = cmdlineInput.txoIndex;
            // if txoIndex is -1, that means nothing was specified on the command line, so set it to zero
            if (transaction.txoIndex < 0)
                transaction.txoIndex = 0;
        }

        utxoinfo_t utxoinfo = btc.gettxout(transaction.txid, transaction.txoIndex);

        if(utxoinfo.bestblock.empty() && utxoinfo.confirmations == 0) {
            // this could mean other things, but basically gettxout() didn't return any data, so:
            std::cerr << "Error: the address at txid: " << transaction.query
                      << " and txoIndex: " << transaction.txoIndex
                      << " has no value--it may have already been spent.\n";
            std::exit(-1);
        }

        assert(utxoinfo.scriptPubKey.addresses.size() == 1);
        unspentData.address = utxoinfo.scriptPubKey.addresses[0];
        btcaddressinfo_t btcaddressinfo = btc.getaddressinfo(unspentData.address);
        unspentData.redeemScript = extractRedeemScript(btcaddressinfo);
        unspentData.witnessScript = extractWitnessScript(btcaddressinfo);

        unspentData.txid = transaction.txid;
        unspentData.utxoIndex = transaction.txoIndex;
        unspentData.amountSatoshis = btc2satoshi(utxoinfo.value);
        unspentData.scriptPubKeyHex = utxoinfo.scriptPubKey.hex;

        // 2. compute change needed to go to outputAddress

        int64_t change = unspentData.amountSatoshis - btc2satoshi(cmdlineInput.fee);

        // 3. create DID transaction and submit to network

        // add input txid and tx index
        std::vector<txout_t> inputs;
        txout_t txout = {unspentData.txid, static_cast<unsigned int>(unspentData.utxoIndex)};
        inputs.push_back(txout);

        // add output address and the change amount
        // TODO validate output address
        std::map<std::string, std::string> amounts;

        // first output is the output address for the change
        amounts.insert(std::make_pair(cmdlineInput.outputAddress, btc_to_string(satoshi2btc(change)) ));

        // second output is the OP_RETURN
        std::string encoded_op_return = encodeOpReturnData(cmdlineInput.ddoRef);
        amounts.insert(std::make_pair("data", encoded_op_return));

        std::string rawTransaction = btc.createrawtransaction(inputs, amounts);

        // sign with private key
        signrawtxinext_t signrawtxin;
        signrawtxin.txid = unspentData.txid;
        signrawtxin.n = static_cast<unsigned int>(unspentData.utxoIndex);
        signrawtxin.scriptPubKey = unspentData.scriptPubKeyHex;
        signrawtxin.redeemScript = unspentData.redeemScript;
        signrawtxin.witnessScript = unspentData.witnessScript;
        signrawtxin.amount = btc_to_string(satoshi2btc(unspentData.amountSatoshis));

        std::string signedRawTransaction =
                btc.signrawtransactionwithkey(rawTransaction,
                        {signrawtxin},
                        {cmdlineInput.privateKey},
                        "ALL");

        if(signedRawTransaction.empty()) {
            std::cerr << "Error: transaction could not be signed. Check your private key.\n";
            std::exit(-1);
        }

        if(cmdlineInput.dryrun) {
            std::cout << "Constructing and signing the transaction was successful, but not submitted to the Bitcoin network.\n";
        }
        else {
            std::cout << "Constructing and signing the transaction was successful. Now submitting to the Bitcoin network.\n";
            std::string resultTxid = btc.sendrawtransaction(signedRawTransaction);
            std::cout << "After a few minutes you can use txid2txref with the following data to compute your txref and DID:\n";
            printAsJson(resultTxid);
        }

        // TODO create a DID object and print out the DID string. Warn that it isn't valid until
        // the transaction has enough confirmations
    }
    catch(BitcoinException &e)
    {
        std::cerr << "Bitcoind error: " << e.getCode() << " " << e.getMessage() << std::endl;
        std::exit(-1);
    }
    catch(std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }

}
