#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include "libtxref.h"

// In this "API" test file, we should only be referring to symbols in the "txref" namespace.

//check that we correctly decode txrefs for both main and testnet
TEST(TxrefApiTest, txref_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1-rqqq-qqqq-qygr-lgl";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1-rqqq-qqll-lceg-dfk";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0x7FFF);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1-r7ll-llqq-qhgl-lue";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1-r7ll-llll-lte5-das";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x7FFF);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1-rjk0-uqay-z0u3-gl8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "txtest1-xjk0-uqay-zz5s-jae";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0);
}

// check that we can deal with weird txref formatting (invalid characters)
TEST(TxrefApiTest, txref_decode_weird_formatting) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1---rqqq-<qqqq>-q>y<gr-l#g#l--";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);
    EXPECT_EQ(loc.txref, "tx1:rqqq-qqqq-qygr-lgl");

    txref = "tx1-rqqq qqqq qygr lgl";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);
    EXPECT_EQ(loc.txref, "tx1:rqqq-qqqq-qygr-lgl");

    txref = "tx1rqqq,qqqq.qygr.lgl";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);
    EXPECT_EQ(loc.txref, "tx1:rqqq-qqqq-qygr-lgl");

    txref = "tx@test1-xj$$k0-uq@@ay---zz5s-j%$a e";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0);
    EXPECT_EQ(loc.txref, "txtest1:xjk0-uqay-zz5s-jae");
}


// check that we can deal with missing HRPs at the start of the txref
TEST(TxrefApiTest, txref_decode_no_HRPs) {
    std::string txref;
    txref::LocationData loc;

    txref = "rqqq-qqqq-qygr-lgl";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);
    EXPECT_EQ(loc.txref, "tx1:rqqq-qqqq-qygr-lgl");

    txref = "xjk0-uqay-zz5s-jae";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0);
    EXPECT_EQ(loc.txref, "txtest1:xjk0-uqay-zz5s-jae");
}


// check that we correctly decode extended txrefs for both main and testnet
TEST(TxrefApiTest, txref_extended_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1:yqqq-qqqq-qqqq-f0ng-4y";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1:y7ll-llqq-qqqq-ztam-5x";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1:yjk0-uqay-zu4x-vf9r-7x";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);

    txref = "txtest1:8jk0-uqay-zu4x-z32g-ap";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);
}

//check that we correctly decode extended txrefs even with missing HRPs and weird formatting
TEST(TxrefApiTest, txref_extended_decode_weird) {
    std::string txref;
    txref::LocationData loc;

    txref = "y q#q q- # q q q# %q   -#q % q qq# f0 (ng-)  4y";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "y 7 l l - l l q q - q q q q - zt  a m - 5  x   ";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "y#$%jk0$%^-u$%^&qa^&*(y-#$%z^&*u4x^&*-vf%^&*(9r-^&*(7x";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);

    txref = "8jk.0.-.u.q.a..y.-..z.u..4.x..-..z.3.2.g.-.a....p..";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST_EXTENDED);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);
}

// check that we correctly encode txrefs in main and testnet
TEST(TxrefApiTest, txref_encode) {
    EXPECT_EQ(txref::encode(0, 0),
              "tx1:rqqq-qqqq-qygr-lgl");
    EXPECT_EQ(txref::encode(0, 0x7FFF),
              "tx1:rqqq-qqll-lceg-dfk");
    EXPECT_EQ(txref::encode(0xFFFFFF, 0),
              "tx1:r7ll-llqq-qhgl-lue");
    EXPECT_EQ(txref::encode(0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-lte5-das");
    EXPECT_EQ(txref::encode(466793, 2205),
              "tx1:rjk0-uqay-z0u3-gl8");
    EXPECT_EQ(txref::encodeTestnet(0, 0),
              "txtest1:xqqq-qqqq-qfqz-92p");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF),
              "txtest1:x7ll-llll-lx34-hlw");
    EXPECT_EQ(txref::encodeTestnet(467883, 2355),
              "txtest1:xk63-uqnf-zz0k-3h7");
}

// check that we correctly encode extended txrefs for main and testnet
TEST(TxrefApiTest, txref_extended_encode) {

    EXPECT_EQ(txref::encode(0, 0, 100),
              "tx1:yqqq-qqqq-qyrq-sf0p-h4");

    EXPECT_EQ(txref::encode(0, 0, 0x1FFF),
              "tx1:yqqq-qqqq-qll8-7t5w-lr");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 100),
              "tx1:yqqq-qqll-8yrq-9jpd-f9");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 200),
              "tx1:y7ll-lrqq-qgxq-4j80-r2");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0x1FFF),
              "tx1:y7ll-lrll-8ll8-vplh-nk");

    EXPECT_EQ(txref::encode(466793, 2205, 10),
              "tx1:yjk0-uqay-z2qq-fycf-mp");

    EXPECT_EQ(txref::encode(466793, 2205, 0x1FFF),
              "tx1:yjk0-uqay-zll8-25h5-aq");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 100),
              "txtest1:8qqq-qqqq-qyrq-73q2-5j");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0x1FFF),
              "txtest1:8qqq-qqqq-qll8-snm9-uy");

    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 100),
              "txtest1:8qqq-qqll-lyrq-ywmh-57");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 200),
              "txtest1:87ll-llqq-qgxq-hlrz-n2");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0x1FFF),
              "txtest1:87ll-llll-lll8-pgwt-a2");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 10),
              "txtest1:8jk0-uqay-z2qq-8uhz-cx");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0x1FFF),
              "txtest1:8jk0-uqay-zll8-yvcl-78");
}

// check that we return regular txref for txoIndex=0, unless forceExtended is true
TEST(TxrefApiTest, txref_extended_encode_force_zero) {

    EXPECT_EQ(txref::encode(0, 0, 0),
              "tx1:rqqq-qqqq-qygr-lgl");
    EXPECT_EQ(txref::encode(0, 0, 0, true),
              "tx1:yqqq-qqqq-qqqq-f0ng-4y");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 0),
              "tx1:rqqq-qqll-8nvy-ezc");
    EXPECT_EQ(txref::encode(0, 0x1FFF, 0, true),
              "tx1:yqqq-qqll-8qqq-u5ay-t5");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0),
              "tx1:r7ll-lrqq-qw4q-a8c");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0, true),
              "tx1:y7ll-lrqq-qqqq-w7ka-8p");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0),
              "tx1:r7ll-lrll-8e38-mdl");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0, true),
              "tx1:y7ll-lrll-8qqq-m9c3-e3");

    EXPECT_EQ(txref::encode(466793, 2205, 0),
              "tx1:rjk0-uqay-z0u3-gl8");
    EXPECT_EQ(txref::encode(466793, 2205, 0, true),
              "tx1:yjk0-uqay-zqqq-assj-h8");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0),
              "txtest1:xqqq-qqqq-qfqz-92p");
    EXPECT_EQ(txref::encodeTestnet(0, 0, 0, true),
              "txtest1:8qqq-qqqq-qqqq-8hur-kr");

    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 0),
              "txtest1:xqqq-qqll-l43f-htg");
    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 0, true),
              "txtest1:8qqq-qqll-lqqq-ag87-k0");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 0),
              "txtest1:x7ll-llqq-q6q7-978");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 0, true),
              "txtest1:87ll-llqq-qqqq-vnjs-hp");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0),
              "txtest1:x7ll-llll-lx34-hlw");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0, true),
              "txtest1:87ll-llll-lqqq-kvfd-hd");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0),
              "txtest1:xjk0-uqay-zz5s-jae");
    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0, true),
              "txtest1:8jk0-uqay-zqqq-ngle-5q");
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encode(height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encodeTestnet(height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encode(height, pos, index);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == index);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT_TESTNET
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION_TESTNET
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encodeTestnet(height, pos, index, true);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == index);
}

