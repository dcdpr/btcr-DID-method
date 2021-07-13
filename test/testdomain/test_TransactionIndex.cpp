#include <gtest/gtest.h>

#include "transactionIndex.h"

TEST(TransactionIndexTest, construct_valid_TransactionIndex) {

    ASSERT_NO_THROW(std::unique_ptr<TransactionIndex>(new TransactionIndex(0)));
    ASSERT_NO_THROW(std::unique_ptr<TransactionIndex>(new TransactionIndex(1)));
    ASSERT_NO_THROW(std::unique_ptr<TransactionIndex>(new TransactionIndex(10)));

}

TEST(TransactionIndexTest, TransactionIndex_with_negative_index_is_invalid) {

    ASSERT_THROW(std::unique_ptr<TransactionIndex>(new TransactionIndex(-1)), std::runtime_error);

}

TEST(TransactionIndexTest, test_transactionIndex_equality) {

    TransactionIndex tp1(3);
    TransactionIndex tp2(3);
    TransactionIndex tp3(4);

    ASSERT_EQ(tp1, tp2);
    ASSERT_NE(tp1, tp3);
}
