#include "bitcoinRPCFacade.h"
#include "encodeOpReturnData.h"
#include "anyoption.h"
#include "libtxref.h"
#include <iostream>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <bitcoinapi/bitcoinapi.h>


struct TransactionData {
    std::string inputString;
    std::string outputAddress;
    std::string privateKey;
    std::string ddoRef;
    double fee = 0.0;
    int txoIndex = 0;
};


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

int parseCommandLineArgs(int argc, char **argv, struct RpcConfig &config, struct TransactionData &transactionData) {

    auto opt = std::unique_ptr<AnyOption>(new AnyOption());
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: createBtcrDid [options] <inputXXX> <changeAddress> <network> <WIF> <fee> <ddoRef>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpcconnect [hostname or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( "" );
    opt->addUsage( "<did>                       the BTCR DID to verify. Could be txref or txref-ext based" );

    opt->setFlag("help", 'h');
    opt->setOption("rpcconnect");
    opt->setOption("rpcuser");
    opt->setOption("rpcpassword");
    opt->setOption("rpcport");
    opt->setCommandOption("config");

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
                          << " not readable. Perhaps try --config? Attempting to continue..."
                          << std::endl;
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

    // see if there is an rpcconnect specified. If not, use default
    if (opt->getValue("rpcconnect") != nullptr) {
        config.rpcconnect = opt->getValue("rpcconnect");
    }

    // see if there is an rpcuser specified. If not, exit
    if (opt->getValue("rpcuser") == nullptr) {
        std::cerr << "'rpcuser' not found. Check bitcoin.conf or command line usage." << std::endl;
        opt->printUsage();
        return -1;
    }
    config.rpcuser = opt->getValue("rpcuser");

    // see if there is an rpcpassword specified. If not, exit
    if (opt->getValue("rpcpassword") == nullptr) {
        std::cerr << "'rpcpassword' not found. Check bitcoin.conf or command line usage." << std::endl;
        opt->printUsage();
        return -1;
    }
    config.rpcpassword = opt->getValue("rpcpassword");

    // will try both well known ports (8332 and 18332) if one is not specified
    if (opt->getValue("rpcport") != nullptr) {
        config.rpcport = convertIntegerArg("rpcport", opt.get());
    }

    // get the positional arguments
    if(opt->getArgc() < 1) {
        std::cerr << "Error: all required arguments not found. Check command line usage." << std::endl;
        opt->printUsage();
        return -1;
    }
    transactionData.inputString = opt->getArgv(0);

    // TODO validate position arguments

    return 1;
}


int main(int argc, char *argv[]) {

    struct RpcConfig rpcConfig;
    struct TransactionData transactionData;

    int ret = parseCommandLineArgs(argc, argv, rpcConfig, transactionData);
    if (ret < 1) {
        std::exit(ret);
    }


    try {

        BitcoinRPCFacade btc(rpcConfig);

        blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

        // 0. Determine InputType

        txref::InputParam inputParam = txref::classifyInputString(transactionData.inputString);

            // skeleton of a diddo verifier:
    //
    // 1) verify that diddo is valid json-ld (how?)
    // 2) extract txref from diddo (id field?)
    // 3) decode txref to find block height, transaction index, and txo index
    // 4) follow txo chain if necessary (as in didResolver) to find the last unspent output
    // 5) Extract the hex-encoded public key that signed the transaction and verify it matches
    //    the #keys-1 key in the authentication array
    // 6) verify any signatures embeded in the other parts of the diddo
    // 7) what else do we need to verify? urls? endpoints? references to x? what else can be signed?

    }
    catch(BitcoinException &e)
    {
        std::cerr << e.getCode() << " " << e.getMessage() << std::endl;
        std::exit(-1);
    }
    catch(std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }

}
