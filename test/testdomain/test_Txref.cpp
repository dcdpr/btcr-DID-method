#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "txref.h"

#include "vout.h"
#include "txid.h"
#include "../mock_bitcoinRPCFacade.h"

using ::testing::Return;
using ::testing::_;

TEST(TxrefTest, constructingTxref_withEmptyTxref_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string txrefStr;

    std::unique_ptr<Txref> txrefp;
    ASSERT_THROW(txrefp.reset(new Txref(txrefStr, btc)), std::runtime_error);

}

TEST(TxrefTest, constructingTxref_withBadTxref_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string txrefStr = "xg4h-jzgq-pqpq-q9r6sd9";

    std::unique_ptr<Txref> txrefp;
    ASSERT_THROW(txrefp.reset(new Txref(txrefStr, btc)), std::runtime_error);

}

TEST(TxrefTest, constructingTxref_withGoodTxidAndVout_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillOnce(Return(rawTransaction1));

    // it will also return a blockhash
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = "3243f6a8885a308d";
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillOnce(Return(blockChainInfo));

    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = 12345;
    std::vector<std::string> blockTransactions {"123abc", txidStr, "234abc"};
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillOnce(Return(blockInfo));

    std::unique_ptr<Txid> txid;

    ASSERT_NO_THROW(txid.reset(new Txid(txidStr, btc)));

    // we made two vouts in the mock above, let's pick the second one
    Vout vout(1);

    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(*txid, vout, btc)));

}

TEST(TxrefTest, constructingTxref_withGoodTxidAndBadVout_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillOnce(Return(rawTransaction1));

    // it will also return a blockhash
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = "3243f6a8885a308d";
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillOnce(Return(blockChainInfo));

    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = 12345;
    std::vector<std::string> blockTransactions {"123abc", txidStr, "234abc"};
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillOnce(Return(blockInfo));

    std::unique_ptr<Txid> txid;

    ASSERT_NO_THROW(txid.reset(new Txid(txidStr, btc)));

    // we made two vouts in the mock above, attempt to reference too many vouts
    Vout vout(10);

    std::unique_ptr<Txref> txrefp;
    ASSERT_THROW(txrefp.reset(new Txref(*txid, vout, btc)), std::runtime_error);

}

TEST(TxrefTest, constructingTxref_withGoodTxref_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // this txrefStr was made via
    // txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 170, 1)
    std::string txrefStr = "r52q-qqpq-qpty-cfg";
    int blockHeight = 170;
    std::vector<std::string>::size_type transactionPos = 1;
    std::string txidStr = "f4184fc596403b9d638783cf57adfe4c75c605f6356fbc91338530e9831e9e16";

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillRepeatedly(Return(rawTransaction1));

    // it will also return a blockhash
    std::string fakeBlockhash = "3243f6a8885a308d";
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = fakeBlockhash;
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

    EXPECT_CALL(btc, getblockhash(_))
            .WillRepeatedly(Return(fakeBlockhash));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillRepeatedly(Return(blockChainInfo));


    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));

    Txid expectedTxid(txidStr, btc);
    Vout expectedVout(0);

    // test that we can construct the txref
    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(txrefStr, btc)));

    // test that the txref's txids and vouts are equal to what we expect
    ASSERT_EQ(expectedTxid, *txrefp->getTxid());
    ASSERT_EQ(expectedVout, *txrefp->getVout());

}

TEST(TxrefTest, constructingTxref_withGoodTxrefExt_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // this txrefStr was made via
    // txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 456789, 1234, 1)
    std::string txrefStr = "y29u‑mqjx‑ppqq‑sfp2‑tt";
    int blockHeight = 456789;
    std::vector<std::string>::size_type transactionPos = 1234;
    std::string txidStr = "6fb8960f70667dc9666329728a19917937896fc476dfc54a3e802e887ecb4e82";


    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillRepeatedly(Return(rawTransaction1));

    // it will also return a blockhash
    std::string fakeBlockhash = "3243f6a8885a308d";
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = fakeBlockhash;
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

    EXPECT_CALL(btc, getblockhash(_))
            .WillRepeatedly(Return(fakeBlockhash));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillRepeatedly(Return(blockChainInfo));

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));

    Txid expectedTxid(txidStr, btc);
    Vout expectedVout(1);

    // test that we can construct the txref
    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(txrefStr, btc)));

    // test that the txref's txids and vouts are equal to what we expect
    ASSERT_EQ(expectedTxid, *txrefp->getTxid());
    ASSERT_EQ(expectedVout, *txrefp->getVout());

}

TEST(TxrefTest, constructingTxref_withGoodTxrefExtVout1_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // this txrefStr was made via
    // txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 1355601, 1022, 1)
    std::string txrefStr = "8z4h-jz7l-qpqq-xkh8-xa";
    int blockHeight = 1355601;
    std::vector<std::string>::size_type transactionPos = 1022;
    std::string txidStr = "cb0252c5ea4e24bee19edd1ed1338ef077dc75d30383097d8c4bae3a9862b35a";


    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillRepeatedly(Return(rawTransaction1));

    // it will also return a blockhash
    std::string fakeBlockhash = "3243f6a8885a308d";
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = fakeBlockhash;
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

    EXPECT_CALL(btc, getblockhash(_))
            .WillRepeatedly(Return(fakeBlockhash));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillRepeatedly(Return(blockChainInfo));

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));

    Txid expectedTxid(txidStr, btc);
    Vout expectedVout(1);

    // test that we can construct the txref
    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(txrefStr, btc)));

    // test that the txref's txids and vouts are equal to what we expect
    ASSERT_EQ(expectedTxid, *txrefp->getTxid());
    ASSERT_EQ(expectedVout, *txrefp->getVout());

}
