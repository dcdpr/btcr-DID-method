include(CTest)
find_package(CURL)
include(../cmake/FindJSONCPP.cmake)

# Almost all of these integration tests require an existing
# ~/.bitcoin/bitcoin.conf with rpc* parameters that point to a
# running testnet bitcoind. See top-level README.md for details.

############################################################
# Convenience targets for testing

add_custom_target(check
    COMMAND echo [----] Running tests
    USES_TERMINAL
    COMMAND ${CMAKE_CTEST_COMMAND}  --output-on-failure -C Debug
    DEPENDS txid2txref createBtcrDid)

add_custom_target(checkVerbose
    COMMAND echo [----] Running tests
    USES_TERMINAL
    COMMAND ${CMAKE_CTEST_COMMAND}  -V --output-on-failure -C Debug
    DEPENDS txid2txref createBtcrDid)


############################################################
# Integration tests for txid2txref

# no arguments
add_test(NAME "IntegrationTests_t2t_Is_Runnable"
    COMMAND $<TARGET_FILE:txid2txref>)

# ask for help
add_test(NAME "IntegrationTests_t2t_Is_Runnable-h"
    COMMAND $<TARGET_FILE:txid2txref> -h)

# test for bad arguments
add_test(NAME "IntegrationTests_t2t_bad_arg_txoIndex"
        COMMAND $<TARGET_FILE:txid2txref> --txoIndex -1 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_bad_arg_txoIndex" PROPERTIES
        PASS_REGULAR_EXPRESSION "Error: txoIndex '-1' should be zero or greater."
        FAIL_REGULAR_EXPRESSION "Warning")

add_test(NAME "IntegrationTests_t2t_bad_arg_invalid_txoIndex"
        COMMAND $<TARGET_FILE:txid2txref> --txoIndex fij f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_bad_arg_invalid_txoIndex" PROPERTIES
        PASS_REGULAR_EXPRESSION "Error: txoIndex 'fij' is invalid. Check command line usage."
        FAIL_REGULAR_EXPRESSION "Warning")

# get txref from an invalid txid.

add_test(NAME "IntegrationTests_t2t_txref_from_invalid_txid"
        COMMAND $<TARGET_FILE:txid2txref> foobarfoobar)
set_tests_properties("IntegrationTests_t2t_txref_from_invalid_txid" PROPERTIES
        PASS_REGULAR_EXPRESSION "Error: foobarfoobar is an invalid txid or txref."
        FAIL_REGULAR_EXPRESSION "Warning")

# get txref from an unknown txid.

add_test(NAME "IntegrationTests_t2t_txref_from_unknown_txid"
        COMMAND $<TARGET_FILE:txid2txref> f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528108)
set_tests_properties("IntegrationTests_t2t_txref_from_unknown_txid" PROPERTIES
        PASS_REGULAR_EXPRESSION "Error: transaction f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528108 not found."
        FAIL_REGULAR_EXPRESSION "Warning")

# get txref from a known txid.

add_test(NAME "IntegrationTests_t2t_txref_from_known_txid"
    COMMAND $<TARGET_FILE:txid2txref> f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_txref_from_known_txid" PROPERTIES
        PASS_REGULAR_EXPRESSION "txtest1:xyv2-xzpq-q63z-7p4"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

# get txrefext from a known txid.

# we decided during the btcr summer 2019 hackathon to not generate or accept the txrefext form of a txid
# where the txoIndex is 0. In those cases we should only generate or accept the short txref form. This
# test was changed to show t2t outputting a txref instead of outputting a txrefext
add_test(NAME "IntegrationTests_t2t_txref_from_known_txid_index_0"
    COMMAND $<TARGET_FILE:txid2txref> --txoIndex 0 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_txref_from_known_txid_index_0" PROPERTIES
        PASS_REGULAR_EXPRESSION "txtest1:xyv2-xzpq-q63z-7p4"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

add_test(NAME "IntegrationTests_t2t_txrefext_from_known_txid_index_1"
    COMMAND $<TARGET_FILE:txid2txref> --txoIndex 1 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107)
set_tests_properties("IntegrationTests_t2t_txrefext_from_known_txid_index_1" PROPERTIES
        PASS_REGULAR_EXPRESSION "txtest1:8yv2-xzpq-qpqq-cew3-4s"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

# get txid from txref.

add_test(NAME "IntegrationTests_t2t_txid_from_txref"
        COMMAND $<TARGET_FILE:txid2txref> txtest1:xyv2-xzpq-q63z-7p4)
set_tests_properties("IntegrationTests_t2t_txid_from_txref" PROPERTIES
        PASS_REGULAR_EXPRESSION "f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

# get txid from txrefext.

add_test(NAME "IntegrationTests_t2t_txid_from_txrefext"
        COMMAND $<TARGET_FILE:txid2txref> txtest1:8yv2-xzpq-qpqq-cew3-4s)
set_tests_properties("IntegrationTests_t2t_txid_from_txrefext" PROPERTIES
        PASS_REGULAR_EXPRESSION "f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

# check for incompatible arguments--txoIndex should not be used when input is a txref as
# that is already encoded in the txref

add_test(NAME "IntegrationTests_t2t_txid_from_txref_bad_txoIndex"
        COMMAND $<TARGET_FILE:txid2txref> --txoIndex 3 txtest1:xyv2-xzpq-q63z-7p4)
set_tests_properties("IntegrationTests_t2t_txid_from_txref_bad_txoIndex" PROPERTIES
        PASS_REGULAR_EXPRESSION "Warning: txoIndex '3' was ignored"
        FAIL_REGULAR_EXPRESSION "Error")

add_test(NAME "IntegrationTests_t2t_txid_from_txrefext_bad_txoIndex"
        COMMAND $<TARGET_FILE:txid2txref> --txoIndex 3 8yv2-xzpq-qpqq-cew3-4s)
set_tests_properties("IntegrationTests_t2t_txid_from_txrefext_bad_txoIndex" PROPERTIES
        PASS_REGULAR_EXPRESSION "Warning: txoIndex '3' was ignored"
        FAIL_REGULAR_EXPRESSION "Error")

############################################################
# Integration tests for createBtcrDid

# TODO: Not sure yet how to make an integration test for createBtcrDid,
# since it requires new source and destination BTC addresses each
# time... Maybe we need to do something with a bitcoind using regtest.

# no arguments
add_test(NAME "IntegrationTests_cbd_Is_Runnable"
    COMMAND $<TARGET_FILE:createBtcrDid>)

# ask for help
add_test(NAME "IntegrationTests_cbd_Is_Runnable-h"
    COMMAND $<TARGET_FILE:createBtcrDid> -h)

# check for incompatible arguments--txoIndex should not be used when input is a txref as
# that is already encoded in the txref

add_test(NAME "IntegrationTests_cbd_bad_args_index_0_warning"
        COMMAND $<TARGET_FILE:createBtcrDid> -n --txoIndex 0 txtest1:xs9n-2rjq-q3up-v2y 2NF8nkNM13FTVn97yHGZQgBpBUZvJc7WmMc cRnkSDWFHbPHbsWy2vK3u413J4KUm7e255a166b6d1640d104e8b 0.0001 https://example.com/ddo-test.jsonld)
set_tests_properties("IntegrationTests_cbd_bad_args_index_0_warning" PROPERTIES
        PASS_REGULAR_EXPRESSION "Warning: passing txoIndex '0' on the command line was not needed as the value of '0' is encoded within your txref")

add_test(NAME "IntegrationTests_cbd_bad_args_index_1_warning"
        COMMAND $<TARGET_FILE:createBtcrDid> -n --txoIndex 1 txtest1:8s9n-2rjq-qpqq-tmjd-2k 2NF8nkNM13FTVn97yHGZQgBpBUZvJc7WmMc cRnkSDWFHbPHbsWy2vK3u413J4KUm7e255a166b6d1640d104e8b 0.0001 https://example.com/ddo-test.jsonld)
set_tests_properties("IntegrationTests_cbd_bad_args_index_1_warning" PROPERTIES
        PASS_REGULAR_EXPRESSION "Warning: passing txoIndex '1' on the command line was not needed as the value of '1' is encoded within your txref")

add_test(NAME "IntegrationTests_cbd_bad_args_index_2_error"
        COMMAND $<TARGET_FILE:createBtcrDid> -n --txoIndex 2 txtest1:8s9n-2rjq-qpqq-tmjd-2k 2NF8nkNM13FTVn97yHGZQgBpBUZvJc7WmMc cRnkSDWFHbPHbsWy2vK3u413J4KUm7e255a166b6d1640d104e8b 0.0001 https://example.com/ddo-test.jsonld)
set_tests_properties("IntegrationTests_cbd_bad_args_index_2_error" PROPERTIES
        PASS_REGULAR_EXPRESSION "Error: passing txoIndex '2' on the command line is not supported as the value of '1' is encoded within your txref"
        FAIL_REGULAR_EXPRESSION "Warning")


############################################################
# Integration tests for didResolver

# no arguments
add_test(NAME "IntegrationTests_didResolver_Is_Runnable"
        COMMAND $<TARGET_FILE:didResolver>)

# ask for help
add_test(NAME "IntegrationTests_didResolver_Is_Runnable-h"
        COMMAND $<TARGET_FILE:didResolver> -h)

# DID with zero updates/spends:
#
# did:btcr:8xvh-jzj2-ppqq-v8n2-8g encodes txid
# f3b955b2db510ba2f60446de51a9e4bcfb96a0e2ad8154d39bfefaffb305e6b0,
# output index 1. This is unspent.
add_test(NAME "IntegrationTests_didResolver_followTip_zero_spends"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:8xvh-jzj2-ppqq-v8n2-8g)

set_tests_properties("IntegrationTests_didResolver_followTip_zero_spends" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: f3b955b2db510ba2f60446de51a9e4bcfb96a0e2ad8154d39bfefaffb305e6b0"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

# DID with one update/spend:
#
# did:btcr:8svh-jzt7-qpqq-lqdx-gu encodes txid
# 2e71684bba5c953c09c9f2363233af9938af4fe05d2b22df3df1c3dabdcc8200,
# output index 1. This is spent to create new DID
# did:btcr:8jvh-jzxc-ppqq-y0z5-qw. That encodes txid
# 604a0cf5c5b794ca8b90a817b0624d5e6f1fab1ff4675f72b7a31679945ebdd9,
# output index 1. This is unspent. Both DIDs should have the same "tip" (604a...)
add_test(NAME "IntegrationTests_didResolver_followTip_one_spend_p1"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:8svh-jzt7-qpqq-lqdx-gu)
set_tests_properties("IntegrationTests_didResolver_followTip_one_spend_p1" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 604a0cf5c5b794ca8b90a817b0624d5e6f1fab1ff4675f72b7a31679945ebdd9"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

add_test(NAME "IntegrationTests_didResolver_followTip_one_spend_p2"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:8jvh-jzxc-ppqq-y0z5-qw)
set_tests_properties("IntegrationTests_didResolver_followTip_one_spend_p2" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 604a0cf5c5b794ca8b90a817b0624d5e6f1fab1ff4675f72b7a31679945ebdd9"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

# DID with two updates/spends:
#
# did:btcr:8z4h-jz7l-qpqq-v4c5-up encodes txid
# cb0252c5ea4e24bee19edd1ed1338ef077dc75d30383097d8c4bae3a9862b35a,
# output index 1. This is spent to create new DID
# did:btcr:8x4h-jz54-qpqq-k29f-jw. That encodes txid
# 3289a36a8ca15017269a6b6e3977732fc21c8106d85020d5493ee9f5c8e00958,
# output index 1. This is spent to create new DID
# did:btcr:8g4h-jz2g-qpqq-m23x-xr. That encodes txid
# 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5,
# output index 1. This is unspent. All three DIDs should have the
# same "tip" (8a76...)
add_test(NAME "IntegrationTests_didResolver_followTip_two_spends_p1"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:8z4h-jz7l-qpqq-v4c5-up)
set_tests_properties("IntegrationTests_didResolver_followTip_two_spends_p1" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

add_test(NAME "IntegrationTests_didResolver_followTip_two_spends_p2"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:8x4h-jz54-qpqq-k29f-jw)
set_tests_properties("IntegrationTests_didResolver_followTip_two_spends_p2" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5"
        FAIL_REGULAR_EXPRESSION "Warning;Error")

add_test(NAME "IntegrationTests_didResolver_followTip_two_spends_p3"
        COMMAND $<TARGET_FILE:didResolver> --exitAfterFollowTip did:btcr:8g4h-jz2g-qpqq-m23x-xr)
set_tests_properties("IntegrationTests_didResolver_followTip_two_spends_p3" PROPERTIES
        PASS_REGULAR_EXPRESSION "txid with unspent output: 8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5"
        FAIL_REGULAR_EXPRESSION "Warning;Error")


############################################################
# Other small integration tests

add_executable(IntegrationTests_chainSoQuery itest_chainSoQuery.cpp ../src/chainQuery.cpp ../src/chainSoQuery.cpp ../src/curlWrapper.cpp)

target_compile_features(IntegrationTests_chainSoQuery PRIVATE cxx_std_11)
target_compile_options(IntegrationTests_chainSoQuery PRIVATE ${DCD_CXX_FLAGS})
set_target_properties(IntegrationTests_chainSoQuery PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(IntegrationTests_chainSoQuery
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${JSONCPP_INCLUDE_DIRS})

target_link_libraries(IntegrationTests_chainSoQuery
        PUBLIC
        nlohmann-json ${JSONCPP_LIBRARIES} ${CURL_LIBRARIES})

add_test(NAME "IntegrationTests_chainSoQuery"
        COMMAND $<TARGET_FILE:IntegrationTests_chainSoQuery>)
