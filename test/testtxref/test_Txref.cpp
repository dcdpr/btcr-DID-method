#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include "txref.cpp"

// check that we accept block heights within the correct range
TEST(TxrefTest, accept_good_block_heights) {
    EXPECT_NO_THROW(checkBlockHeightRange(0));
    EXPECT_NO_THROW(checkBlockHeightRange(1));
    EXPECT_NO_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT));
}

RC_GTEST_PROP(TxrefTestRC, goodBlockHeightsAreAccepted, ()
) {
    // generate valid block heights
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    checkBlockHeightRange(height);
}


// check that we reject block heights outside of the range
TEST(TxrefTest, reject_bad_block_heights) {
    EXPECT_THROW(checkBlockHeightRange(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badBlockHeightsAreRejected, ()
) {
    // generate out of range block heights
    auto height = *rc::gen::inRange(-MAX_BLOCK_HEIGHT, -1);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);

    height = *rc::gen::inRange(MAX_BLOCK_HEIGHT+1, 2*MAX_BLOCK_HEIGHT);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);
}


// check that we accept transaction positions within the correct range
TEST(TxrefTest, accept_good_transaction_position) {
    EXPECT_NO_THROW(checkTransactionPositionRange(0));
    EXPECT_NO_THROW(checkTransactionPositionRange(1));
    EXPECT_NO_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION));
}

RC_GTEST_PROP(TxrefTestRC, goodTransactionPositionsAreAccepted, ()
) {
    // generate valid transaction positions
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    checkTransactionPositionRange(pos);
}


// check that we reject transaction positions outside of the range
TEST(TxrefTest, reject_bad_transaction_position) {
    EXPECT_THROW(checkTransactionPositionRange(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badTransactionPositionsAreRejected, ()
) {
    // generate out of range transaction positions
    auto pos = *rc::gen::inRange(-MAX_TRANSACTION_POSITION, -1);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);

    pos = *rc::gen::inRange(MAX_TRANSACTION_POSITION+1, 2*MAX_TRANSACTION_POSITION);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);
}

// check that we accept magic codes within the correct range
TEST(TxrefTest, accept_good_magic_code) {
    EXPECT_NO_THROW(checkMagicCodeRange(0));
    EXPECT_NO_THROW(checkMagicCodeRange(1));
    EXPECT_NO_THROW(checkMagicCodeRange(MAX_MAGIC_CODE));
}

RC_GTEST_PROP(TxrefTestRC, goodMagicCodesAreAccepted, ()
) {
    // generate valid magic codes
    auto code = *rc::gen::inRange(0, MAX_MAGIC_CODE);
    checkMagicCodeRange(code);
}

// check that we reject magic codes outside of the range
TEST(TxrefTest, reject_bad_magic_code) {
    EXPECT_THROW(checkMagicCodeRange(-1), std::runtime_error);
    EXPECT_THROW(checkMagicCodeRange(MAX_MAGIC_CODE + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badMagicCodesAreRejected, ()
) {
    // generate out of range magic codes
    auto code = *rc::gen::inRange(-MAX_MAGIC_CODE, -1);
    RC_ASSERT_THROWS_AS(checkMagicCodeRange(code), std::runtime_error);

    code = *rc::gen::inRange(MAX_MAGIC_CODE + 1, 2 * MAX_MAGIC_CODE);
    RC_ASSERT_THROWS_AS(checkMagicCodeRange(code), std::runtime_error);
}

TEST(TxrefTest, addDashes_inputStringTooShort) {
    EXPECT_THROW(addGroupSeparators("", 0, 1), std::runtime_error);
    EXPECT_THROW(addGroupSeparators("0", 0, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_HRPLongerThanInput) {

    // hrplen is zero, then the "rest" of the input is of length two, so one hyphen should be inserted
    auto result = addGroupSeparators("00", 0, 1);
    EXPECT_EQ(result, "0-0");

    // hrplen is one, then the "rest" of the input is of length one, so zero hyphens should be inserted
    result = addGroupSeparators("00", 1, 1);
    EXPECT_EQ(result, "00");

    // hrplen is two, then the "rest" of the input is of length zero, so zero hyphens should be inserted
    result = addGroupSeparators("00", 2, 1);
    EXPECT_EQ(result, "00");

    // hrplen is three, then the "rest" of the input is of length -1, so exception is thrown
    EXPECT_THROW(addGroupSeparators("00", 3, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_HRPTooLong) {
    EXPECT_THROW(addGroupSeparators("00", bech32::limits::MAX_HRP_LENGTH+1, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_separatorOffsetTooSmall) {
    EXPECT_THROW(addGroupSeparators("00", 0, -1), std::runtime_error);
    EXPECT_THROW(addGroupSeparators("00", 0, 0), std::runtime_error);
}

TEST(TxrefTest, addDashes_separatorOffsetTooLarge) {
    // if separatorOffset is greater than input string length, output should be the same
    auto result = addGroupSeparators("00", 0, 2);
    EXPECT_EQ(result, "00");
    result = addGroupSeparators("00", 0, 10);
    EXPECT_EQ(result, "00");
}

TEST(TxrefTest, addDashes_everyOtherCharacter) {
    auto result = addGroupSeparators("00", 0, 1);
    EXPECT_EQ(result, "0-0");

    result = addGroupSeparators("000", 0, 1);
    EXPECT_EQ(result, "0-0-0");

    result = addGroupSeparators("0000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0");

    result = addGroupSeparators("00000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0-0");
}

TEST(TxrefTest, addDashes_everyFewCharacters) {
    auto result = addGroupSeparators("0000000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0-0-0-0");

    result = addGroupSeparators("0000000", 0, 2);
    EXPECT_EQ(result, "00-00-00-0");

    result = addGroupSeparators("0000000", 0, 3);
    EXPECT_EQ(result, "000-000-0");

    result = addGroupSeparators("0000000", 0, 4);
    EXPECT_EQ(result, "0000-000");
}

TEST(TxrefTest, addDashes_withHRPs) {
    auto result = addGroupSeparators("A0000000", 1, 1);
    EXPECT_EQ(result, "A0-0-0-0-0-0-0");

    result = addGroupSeparators("AB0000000", 2, 2);
    EXPECT_EQ(result, "AB00-00-00-0");

    result = addGroupSeparators("ABCD0000000", 4, 4);
    EXPECT_EQ(result, "ABCD0000-000");

}

TEST(TxrefTest, prettyPrint) {
    std::string hrp = txref::BECH32_HRP_MAIN;
    std::string plain = "tx1rqqqqqqqqygrlgl";
    std::string pretty = prettyPrint(plain, hrp.length());
    EXPECT_EQ(pretty, "tx1:rqqq-qqqq-qygr-lgl");
}


// check that we can extract the magic code for a txref string
TEST(TxrefTest, extract_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqygrlgl";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_MAIN);

    txref = "txtest1xjk0uqayzz5sjae";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_TEST);
}

// check that we can extract the version for a txref string
TEST(TxrefTest, extract_version) {
    std::string txref;
    uint8_t version;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqygrlgl";
    bs = bech32::decode(txref);
    extractVersion(version, bs);
    EXPECT_EQ(version, 0);

    txref = "txtest1xjk0uqayzz5sjae";
    bs = bech32::decode(txref);
    extractVersion(version, bs);
    EXPECT_EQ(version, 0);
}

// check that we can extract the block height from txrefs
TEST(TxrefTest, extract_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqygrlgl";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1rqqqqqlllcegdfk";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1r7llllqqqhgllue";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1r7lllllllte5das";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1rjk0uqayz0u3gl8";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest1xjk0uqayzz5sjae";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);
}

// check that we can extract the transaction position from txrefs
TEST(TxrefTest, extract_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqygrlgl";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1rqqqqqlllcegdfk";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1r7llllqqqhgllue";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1r7lllllllte5das";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1rjk0uqayz0u3gl8";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest1xjk0uqayzz5sjae";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);
}

// check that extracting the txo index from txrefs always returns 0
TEST(TxrefTest, extract_txo_position) {
    std::string txref;
    int txoIndex;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqygrlgl";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rqqqqqlllcegdfk";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1r7llllqqqhgllue";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1r7lllllllte5das";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rjk0uqayz0u3gl8";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "txtest1xjk0uqayzz5sjae";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);
}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps) {
    std::string txref;

    txref = "rqqqqqqqqygrlgl";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1rqqqqqqqqygrlgl");

    txref = "xjk0uqayzz5sjae";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest1xjk0uqayzz5sjae");

}

// check that we correctly encode some sample txrefs
TEST(TxrefTest, txref_encode_mainnet) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qygr-lgl");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF),
              "tx1:rqqq-qqll-lceg-dfk");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0),
              "tx1:r7ll-llqq-qhgl-lue");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-lte5-das");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205),
              "tx1:rjk0-uqay-z0u3-gl8");

    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205),
              "txtest1:xjk0-uqay-zz5s-jae");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);

    auto txref = txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

// //////////////// Extended Txrefs /////////////////////

// check for magic codes that support extended txrefs
TEST(TxrefTest, accept_good_magic_code_for_extended) {
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_MAIN_EXTENDED));
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_TEST_EXTENDED));

    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_MAIN), std::runtime_error);
    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_TEST), std::runtime_error);
}

// check that we can extract the magic code for an extended txref string
TEST(TxrefTest, extract_extended_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::HrpAndDp bs;

    txref = "tx1yjk0uqayzu4xvf9r7x";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);

    txref = "txtest18jk0uqayzu4xz32gap";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_TEST_EXTENDED);

}

// check that we can extract the block height from extended txrefs
TEST(TxrefTest, extract_extended_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1yqqqqqqqqqqqf0ng4y";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1y7llllqqqqqqztam5x";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1yjk0uqayzu4xvf9r7x";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest18jk0uqayzu4xz32gap";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

}

// check that we can extract the transaction position from extended txrefs
TEST(TxrefTest, extract_extended_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::HrpAndDp bs;

    txref = "tx1yqqqqqqqqqqqf0ng4y";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1yqqqqqlllqqqnsg44g";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1yjk0uqayzu4xvf9r7x";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest18jk0uqayzu4xz32gap";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

}

// check that we can extract the txo index from extended txrefs
TEST(TxrefTest, extract_extended_txo_index) {
    std::string txref;
    int txoIndex;
    bech32::HrpAndDp bs;

    txref = "tx1yqqqqqqqqqqqf0ng4y";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1yqqqqqqqqpqqtd6lvu";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 1);

    txref = "tx1yqqqqqqqqu4xckxeu9";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "tx1yjk0uqayzu4xvf9r7x";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "txtest18jk0uqayzu4xz32gap";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps_extended) {
    std::string txref;

    txref = "yjk0uqayzu4xnk6upc";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1yjk0uqayzu4xnk6upc");

    txref = "8jk0uqayzu4xaw4hzl";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest18jk0uqayzu4xaw4hzl");

}

// check that we correctly encode some sample extended txrefs
TEST(TxrefTest, txref_encode_extended_mainnet) {
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 0),
              "tx1:yqqq-qqqq-qqqq-f0ng-4y");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 0),
              "tx1:yqqq-qqll-lqqq-nsg4-4g");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0, 0),
              "tx1:y7ll-llqq-qqqq-ztam-5x");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "tx1:y7ll-llll-lqqq-c5xx-52");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 1),
              "tx1:yqqq-qqqq-qpqq-td6l-vu");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 1),
              "tx1:yqqq-qqll-lpqq-3jpz-vs");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0, 1),
              "tx1:y7ll-llqq-qpqq-qf5v-d7");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "tx1:y7ll-llll-lpqq-6k03-dj");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 0x1ABC),
              "tx1:yqqq-qqqq-qu4x-ckxe-u9");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 0x1ABC),
              "tx1:yqqq-qqll-lu4x-zfay-uf");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0, 0x1ABC),
              "tx1:y7ll-llqq-qu4x-njg2-a8");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0x1ABC),
              "tx1:y7ll-llll-lu4x-fdnh-at");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 466793, 2205, 0x1ABC),
              "tx1:yjk0-uqay-zu4x-vf9r-7x");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 466793, 2205, 0x1ABC),
              "txtest1:8jk0-uqay-zu4x-z32g-ap");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameExtendedParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    auto txoIndex = *rc::gen::inRange(0, MAX_TXO_INDEX);

    auto txref = txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, height, pos, txoIndex);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);
}

// //////////////// Examples from BIP-0136 /////////////////////

// check that we correctly encode some sample txrefs from BIP-0136. These may duplicate
// some tests above, but many of the examples in the BIP are present here for reference.
TEST(TxrefTest, txref_encode_bip_examples) {

    // Genesis Coinbase Transaction (Transaction #0 of Block #0):
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qygr-lgl");

    // Transaction #2205 of Block #466793:
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205),
              "tx1:rjk0-uqay-z0u3-gl8");

    // The following list gives properly encoded Bitcoin mainnet TxRef's
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qygr-lgl");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF),
              "tx1:rqqq-qqll-lceg-dfk");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x0),
              "tx1:r7ll-llqq-qhgl-lue");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-lte5-das");

    // The following list gives properly encoded Bitcoin testnet TxRef's
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0),
              "txtest1:xqqq-qqqq-qfqz-92p");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0x7FFF),
              "txtest1:xqqq-qqll-l43f-htg");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0xFFFFFF, 0x0),
              "txtest1:x7ll-llqq-q6q7-978");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0xFFFFFF, 0x7FFF),
              "txtest1:x7ll-llll-lx34-hlw");

    // The following list gives valid (though strangely formatted) Bitcoin TxRef's
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x71F69, 0x89D),
              "tx1:rjk0-uqay-z0u3-gl8");

    // The following list gives properly encoded Bitcoin mainnet TxRef's with Outpoints
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 0),
              "tx1:yqqq-qqqq-qqqq-f0ng-4y");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 0),
              "tx1:yqqq-qqll-lqqq-nsg4-4g");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x0, 0),
              "tx1:y7ll-llqq-qqqq-ztam-5x");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "tx1:y7ll-llll-lqqq-c5xx-52");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 1),
              "tx1:yqqq-qqqq-qpqq-td6l-vu");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 1),
              "tx1:yqqq-qqll-lpqq-3jpz-vs");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x0, 1),
              "tx1:y7ll-llqq-qpqq-qf5v-d7");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "tx1:y7ll-llll-lpqq-6k03-dj");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0x71F69, 0x89D, 0x123),
              "tx1:yjk0-uqay-zrfq-h48h-5e");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0x71F69, 0x89D, 0x1ABC),
              "tx1:yjk0-uqay-zu4x-vf9r-7x");

    // The following list gives properly encoded Bitcoin testnet TxRef's with Outpoints
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0, 0, 0),
              "txtest1:8qqq-qqqq-qqqq-8hur-kr");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0, 0x7FFF, 0),
              "txtest1:8qqq-qqll-lqqq-ag87-k0");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0xFFFFFF, 0x0, 0),
              "txtest1:87ll-llqq-qqqq-vnjs-hp");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "txtest1:87ll-llll-lqqq-kvfd-hd");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0, 0, 1),
              "txtest1:8qqq-qqqq-qpqq-9445-0m");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0, 0x7FFF, 1),
              "txtest1:8qqq-qqll-lpqq-l2wf-0h");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0xFFFFFF, 0x0, 1),
              "txtest1:87ll-llqq-qpqq-w3m8-we");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "txtest1:87ll-llll-lpqq-5wq6-w4");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0x71F69, 0x89D, 0x123),
              "txtest1:8jk0-uqay-zrfq-edgu-h7");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 0x71F69, 0x89D, 0x1ABC),
              "txtest1:8jk0-uqay-zu4x-z32g-ap");
}

TEST(TxrefTest, txref_decode_bip_examples) {

    int height, pos, txoIndex;
    txref::LocationData loc;
    std::string txref;

    // Genesis Coinbase Transaction (Transaction #0 of Block #0):
    height = 0; pos = 0; txref = "tx1:rqqq-qqqq-qygr-lgl";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);


    // Transaction #2205 of Block #466793:
    height = 466793; pos = 2205; txref = "tx1:rjk0-uqay-z0u3-gl8";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);


    // The following list gives properly encoded Bitcoin mainnet TxRef's
    height = 0; pos = 0; txref = "tx1:rqqq-qqqq-qygr-lgl";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);

    height = 0; pos = 0x7FFF; txref = "tx1:rqqq-qqll-lceg-dfk";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x0; txref = "tx1:r7ll-llqq-qhgl-lue";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x7FFF; txref = "tx1:r7ll-llll-lte5-das";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);


    // The following list gives properly encoded Bitcoin testnet TxRef's
    height = 0; pos = 0; txref = "txtest1:xqqq-qqqq-qfqz-92p";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);

    height = 0; pos = 0x7FFF; txref = "txtest1:xqqq-qqll-l43f-htg";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x0; txref = "txtest1:x7ll-llqq-q6q7-978";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);

    height = 0xFFFFFF; pos = 0x7FFF; txref = "txtest1:x7ll-llll-lx34-hlw";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);


    // The following list gives valid (though strangely formatted) Bitcoin TxRef's
    height = 0x71F69; pos = 0x89D; txref = "tx1:rjk0-uqay-z0u3-gl8";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);


    // The following list gives properly encoded Bitcoin mainnet TxRef's with Outpoints
    height = 0; pos = 0; txoIndex = 0; txref = "tx1:yqqq-qqqq-qqqq-f0ng-4y";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 0; txref = "tx1:yqqq-qqll-lqqq-nsg4-4g";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 0; txref = "tx1:y7ll-llqq-qqqq-ztam-5x";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 0; txref = "tx1:y7ll-llll-lqqq-c5xx-52";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);


    height = 0; pos = 0; txoIndex = 1; txref = "tx1:yqqq-qqqq-qpqq-td6l-vu";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 1; txref = "tx1:yqqq-qqll-lpqq-3jpz-vs";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 1; txref = "tx1:y7ll-llqq-qpqq-qf5v-d7";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 1; txref = "tx1:y7ll-llll-lpqq-6k03-dj";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);


    height = 0x71F69; pos = 0x89D; txoIndex = 0x123; txref = "tx1:yjk0-uqay-zrfq-h48h-5e";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0x71F69; pos = 0x89D; txoIndex = 0x1ABC; txref = "tx1:yjk0-uqay-zu4x-vf9r-7x";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);


    // The following list gives properly encoded Bitcoin testnet TxRef's with Outpoints
    height = 0; pos = 0; txoIndex = 0; txref = "txtest1:8qqq-qqqq-qqqq-8hur-kr";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 0; txref = "txtest1:8qqq-qqll-lqqq-ag87-k0";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 0; txref = "txtest1:87ll-llqq-qqqq-vnjs-hp";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 0; txref = "txtest1:87ll-llll-lqqq-kvfd-hd";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);


    height = 0; pos = 0; txoIndex = 1; txref = "txtest1:8qqq-qqqq-qpqq-9445-0m";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0; pos = 0x7FFF; txoIndex = 1; txref = "txtest1:8qqq-qqll-lpqq-l2wf-0h";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x0; txoIndex = 1; txref = "txtest1:87ll-llqq-qpqq-w3m8-we";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0xFFFFFF; pos = 0x7FFF; txoIndex = 1; txref = "txtest1:87ll-llll-lpqq-5wq6-w4";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);


    height = 0x71F69; pos = 0x89D; txoIndex = 0x123; txref = "txtest1:8jk0-uqay-zrfq-edgu-h7";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

    height = 0x71F69; pos = 0x89D; txoIndex = 0x1ABC; txref = "txtest1:8jk0-uqay-zu4x-z32g-ap";
    loc = txref::decode(txref);
    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);

}

TEST(ClassifyInputStringTest, test_empty) {
    EXPECT_EQ(classifyInputString(""), unknown_param);
}

TEST(ClassifyInputStringTest, test_random) {
    EXPECT_EQ(classifyInputString("oihjediouhwisdubch"), unknown_param);
}

TEST(ClassifyInputStringTest, test_address) {
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhem"), address_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQX"), address_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vc"), address_param);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Se"), address_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4"), address_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), unknown_param);
}

TEST(ClassifyInputStringTest, test_bad_address) {
    // too long
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhemse"), unknown_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQXdd"), unknown_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vcd"), unknown_param);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Sedd"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4dd"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), unknown_param);
    // too short
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeF"), unknown_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZffT9Y"), unknown_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47X"), unknown_param);
}

TEST(ClassifyInputStringTest, test_anomolies) {
    // classifyInputString isn't perfect. Here are some examples where it is wrong

    // should be "unknown_param" since these are too-short bitcoin addresses, but they happen
    // to have the right number of characters after being cleaned of invalid characters
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2"), txref_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfa"), txref_param);
}

TEST(ClassifyInputStringTest, test_txid) {
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"), txid_param);
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca4953991b7852b855"), unknown_param);
}

TEST(ClassifyInputStringTest, test_txref) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1rqqqqqqqqygrlgl"), txref_param);
    EXPECT_EQ(classifyInputString("rqqqqqqqqygrlgl"), txref_param);
    // testnet
    EXPECT_EQ(classifyInputString("txtest1xjk0uqayzz5sjae"), txref_param);
    EXPECT_EQ(classifyInputString("xjk0uqayzz5sjae"), txref_param);
}

TEST(ClassifyInputStringTest, test_txrefext) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1yqqqqqqqqqqqf0ng4y"), txrefext_param);
    EXPECT_EQ(classifyInputString("yqqqqqqqqqqqf0ng4y"), txrefext_param);
    // testnet
    EXPECT_EQ(classifyInputString("txtest18jk0uqayzu4xaw4hzl"), txrefext_param);
    EXPECT_EQ(classifyInputString("8jk0uqayzu4xaw4hzl"), txrefext_param);
}


