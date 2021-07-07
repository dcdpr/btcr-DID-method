#ifndef BTCR_DID_TRANSACTIONINDEX_H
#define BTCR_DID_TRANSACTIONINDEX_H

/**
 * This class represents a TransactionIndex, an index identifying a particular transaction in a block
 */
class TransactionIndex {
public:
    /**
     * Construct a TransactionIndex from the given index
     * @param i the index of the particular transaction in a block
     */
    explicit TransactionIndex(int i);

    /**
     * Get this TransactionIndex as an integer value
     * @return this TransactionIndex as an integer value
     */
    int value() const;

    bool operator==(const TransactionIndex &rhs) const;

    bool operator!=(const TransactionIndex &rhs) const;

private:
    int index;
};


#endif //BTCR_DID_TRANSACTIONINDEX_H
