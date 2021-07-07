# btcr-DID-method

To support the development of our DID method, we created two
command-line applications. The first application, `txid2txref`, can
convert Bitcoin transaction ids, or
[txids](https://bitcoin.org/en/glossary/txid), to
[txrefs](https://github.com/bitcoin/bips/blob/master/bip-0136.mediawiki),
and vice-versa. It uses bitcoind to verify the transactions. The second
application, `createBtcrDid`, will create a [decentralized identifier
(DID)](https://w3c-ccg.github.io/did-spec/) on the Bitcoin
blockchain. It also uses bitcoind to submit the transaction needed to
create the DID.

## Building txid2txref and createBtcrDid

This README assumes that you have bitcoind running somewhere, locally
or remotely. bitcoind needs to be a full node--either on mainnet or
testnet--and needs to have `—txindex` and RPC turned on.

We have developed and tested `txid2txref` and `createBtcrDid` on MacOS,
Debian, Ubuntu and MacOS systems. Each OS has it differences, so please check the instructions below:

### Debian / Ubuntu

The following instructions should work with any of Debian 8: "jessie", 9: "stretch" or 10: "buster". These 
instructions also work with Ubuntu 16: "xenial xerus" and 18: "bionic beaver". Intermediate versions of the above 
will probably work as well, but if you have any problems please let us know.

#### Install some pre-requirements

You will need to have a basic C++ development setup and several dependent packages:
```
$ sudo apt-get update
$ sudo apt-get install make cmake gcc g++ libcurl4-openssl-dev libjsoncpp-dev uuid-dev libjsonrpccpp-dev libjsonrpccpp-tools libboost-dev
```

#### Build and install libbitcoin-api-cpp

Download [libbitcoin-api-cpp from Github](https://github.com/minium/bitcoin-api-cpp/). Clone it or grab a zip file, as you prefer.

```
$ git clone https://github.com/minium/bitcoin-api-cpp.git
$ # or
$ wget https://github.com/minium/bitcoin-api-cpp/archive/master.zip && unzip master.zip && rm master.zip
```

This is a cmake-based project, so the standard process can be done here:

```
cd bitcoin-api-cpp
mkdir build
cd build
cmake ..
make
sudo make install
```

#### Build txid2txref and createBtcrDid

If you don't already have it, download [btcr-DID-method from Github](https://github.com/dcdpr/btcr-DID-method/). Clone it or grab a zip file, as you prefer.

```
$ git clone https://github.com/dcdpr/btcr-DID-method.git
$ # or
$ wget https://github.com/dcdpr/btcr-DID-method/archive/master.zip && unzip master.zip && rm master.zip
```

This is a cmake-based project, so the standard process can be done here:

```
cd btcr-DID-method
mkdir build
cd build
cmake ..
make
```

### MacOS

The following instructions should work with any of MacOS 10.13: "High Sierra", 10.14: "Mojave" and 10.15: "Catalina".

#### Install some pre-requirements

You will want to install [homebrew](https://brew.sh/) if you haven't already. See their installation instructions, or try:
```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
``` 

Next, you will need to get some packages:
```
$ brew update
$ brew install jsoncpp libjson-rpc-cpp boost
```

#### Build and install bitcoin-api-cpp

homebrew installs jsoncpp (from above step) a little differently than bitcoin-api-cpp expects. Until the bitcoin-api-cpp folks update their project, 
you should download our local copy of [bitcoin-api-cpp from Github](https://github.com/danpape/bitcoin-api-cpp/). Clone it or grab a zip file, as you prefer.

```
$ git clone https://github.com/danpape/bitcoin-api-cpp.git
$ # or
$ wget https://github.com/danpape/bitcoin-api-cpp/archive/master.zip && unzip master.zip && rm master.zip
```

This is a cmake-based project, so the standard process can be done here:

```
cd bitcoin-api-cpp
mkdir build
cd build
cmake ..
make
sudo make install
```

#### Build txid2txref and createBtcrDid

If you don't already have it, download [btcr-DID-method from Github](https://github.com/dcdpr/btcr-DID-method/). Clone it or grab a zip file, as you prefer.

```
$ git clone https://github.com/dcdpr/btcr-DID-method.git
$ # or
$ wget https://github.com/dcdpr/btcr-DID-method/archive/master.zip && unzip master.zip && rm master.zip
```

This is a cmake-based project, so the standard process can be done here:

```
cd btcr-DID-method
mkdir build
cd build
cmake ..
make
```

# Running txid2txref

The `txid2txref` application is based on the [txid2bech32
app](https://github.com/WebOfTrustInfo/btcr-hackathon) created during
the 2017 BTCR Virtual Hackathon.

`txid2txref` is run as `txid2txref [txid]` and when given a valid `txid`,
will output a `txref` along with some other info. `txid2txref` will
communicate with an instance of bitcoind via RPC to retrieve information
about that transaction so it can create the `txref`. `txid2txref` can also
be run as `txid2txref [txref]` and will output info about the bitcoin
transaction referred to.

You can ask `txid2txref` for help and it will show all the runtime options
available:

```
$ ./src/txid2txref --help
Usage: txid2txref [options] <txid|txref>

 -h  --help                 Print this help
 --rpcconnect [hostname or IP]  RPC host (default: 127.0.0.1)
 --rpcuser [user]           RPC user
 --rpcpassword [pass]       RPC password
 --rpcport [port]           RPC port (default: try both 8332 and 18332)
 --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf)
 --txoIndex [index #]       Index # for XTO within the transaction (default: 0)

<txid|txref>                input: can be a txid to encode, or a txref to decode
```

Many of the runtime options for `txid2txref` are for connecting to
bitcoind over RPC. There are four `--rpc*` options that you can use for
connecting to a local or remote bitcoind:

```
$ ./src/txid2txref --rpcuser bitcoinrpc --rpcpassword super-secret-passwd --rpcconnect 127.0.0.1 --rpcport 18332 <txid>
{...}
```

If you have bitcoind running locally, you probably have a bitcoin.conf
file with all the RPC info in there. If so, you can use it instead of
setting all of the `--rpc*` options:

```
$ # by default it will look in <homedir>/.bitcoin/bitcoin.conf
$ ./src/txid2txref <txid>
{...}

$ # or you can use a different location
$ ./src/txid2txref --config /tmp/bitcoin.conf <txid>
{...}
```

When given a txid, `txid2txref` will connect to bitcoind to verify it is a
valid txid and, if found, gather some info about it. Then it will output
some JSON with details about that transaction, and the txref.

```
$ ./src/txid2txref f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7521234
Error: transaction f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7521234 not found.

$ ./src/txid2txref f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107
{
    "txid": "f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107",
    "txref": "txtest1:xyv2-xzpq-q63z-7p4",
    "did": "did:btcr:xyv2-xzpq-q63z-7p4",
    "network": "test",
    "block-height": "1152194",
    "transaction-index": "1",
    "txo-index": "0",
    "query-string": "f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107"
}
```

`txid2txref` can also output an extended txref if the --txoIndex flag is given.

```
$ ./src/txid2txref f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107 | jq '{txref}'
{
  "txref": "txtest1:xyv2-xzpq-q63z-7p4"
}

$ ./src/txid2txref --txoIndex 1 f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107 | jq '{txref}'
{
  "txref": "txtest1:8yv2-xzpq-qpqq-cew3-4s"
}
```

Alternately, instead of a txid, `txid2txref` can be given a txref to
decode and look up the transaction info in bitcoind:

```
$ ./src/txid2txref txtest1:8yv2-xzpq-qpqq-cew3-4s
{
    "txid": "f8cdaff3ebd9e862ed5885f8975489090595abe1470397f79780ead1c7528107",
    "txref": "txtest1:8yv2-xzpq-qpqq-cew3-4s",
    "did": "did:btcr:8yv2-xzpq-qpqq-cew3-4s",
    "network": "test",
    "block-height": "1152194",
    "transaction-index": "1",
    "txo-index": "1",
    "query-string": "txtest1:8yv2-xzpq-qpqq-cew3-4s"
}
```


# Running createBtcrDid

You can ask `createBtcrDid` for help, and it will show all the runtime
options available:

```
> ./src/createBtcrDid --help
Usage: createBtcrDid [options] <inputXXX> <outputAddress> <private key> <fee> <ddoRef>

 -h  --help                 Print this help
 --rpcconnect [hostname or IP]  RPC host (default: 127.0.0.1)
 --rpcuser [user]           RPC user
 --rpcpassword [pass]       RPC password
 --rpcport [port]           RPC port (default: try both 8332 and 18332)
 --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf)
 --txoIndex [index]         Index # of which TXO to use from the input transaction (default: 0)

<inputXXX>      input: (bitcoin address, txid, txref) needs at least slightly more unspent BTCs than your offered fee
<outputAddress> output bitcoin address: will receive transaction change and be the basis for your DID
<private key>   private key in base58 (wallet import format)
<fee>           fee you are willing to pay (suggestion: >0.001 BTC)
<ddoRef>        reference to a DDO you want as part of your DID (optional)
```

Many of the runtime options for `createBtcrDid` are for connecting to
bitcoind over RPC. There are four `--rpc*` options that you can use for
connecting to a local or remote bitcoind:

```
$ ./src/createBtcrDid --rpcuser bitcoinrpc --rpcpassword super-secret-passwd --rpcconnect 127.0.0.1 --rpcport 18332 ...
```

If you have bitcoind running locally, you probably have a bitcoin.conf
file with all the RPC info in there. If so, you can use it instead of
setting all of the `--rpc*` options:

```
$ # by default it will look in <homedir>/.bitcoin/bitcoin.conf
$ ./src/createBtcrDid ...
{...}

$ # or you can use a different location
$ ./src/createBtcrDid --config /tmp/bitcoin.conf ...
{...}
```

`createBtcrDid` has many required positional parameters:

#### inputXXX
This is the input transaction--where you need to have at least slightly
more unspent BTCs than your offered fee (see below). You can refer to
this transaction in many ways: a Bitcoin address (plus txoIndex, given
with `--txoIndex` parameter); a txid (plus txoIndex); a txref (plus
txoIndex).

#### outputAddress
This is the output Bitcoin address. It will receive transaction change and be the basis for your DID.

#### private key
private key in base58 (wallet import format)

#### fee
fee you are willing to pay (suggestion: >0.001 BTC)

#### ddoRef
reference to a DDO you want as part of your DID. Usually a URL to a
JSON-LD document. (optional)

## Example of how to use createBtcrDid

Some prerequisites:
- you need a running instance of bitcoind, with the option "txindex=1"
enabled so you have full transaction history.
- you need to have some BTC, either on the main or
test blockchain (whatever your bitcoind is running)
- you need the txid of a transaction with your BTC, and the index of the
TXO you want to use (if it is other than 0)
- if you want your DID to have a reference to an external document with
more info (a DDO) then make sure you have a URL for where it can be
retrieved from.

Make sure you know the txid and how much you have to spend from it. You can use
bitcoin-cli to check:

```
$ bitcoin-cli listunspent | jq -r '.[] | { txid: .txid, address: .address, vout: .vout, amount: .amount }'
{
  "txid": "79d864cc59b0c3ac240fc78e5a79edb13182b88c9ed1c60526eda6657a5d5e9e",
  "address": "mvwGweRzRDwydpJfW1uqWJN4iZvNBZ9zZ4",
  "vout": 0,
  "amount": 0.02
}
```

If you want to start using txrefs right away, you can use `txid2txref` to
convert your txid:

```
$ ./src/txid2txref --txoIndex 0 79d864cc59b0c3ac240fc78e5a79edb13182b88c9ed1c60526eda6657a5d5e9e | jq '{txref}'
{
  "txref": "txtest1:xvn9-0z8q-qn3s-nf7"
}
```

You need to decide how much you want to spend to create your DID--this
is the transaction fee. You probably want this as low as possible. For
the testnet, you can probably get by with a fee of 0.0005 BTC for now.

Now you need a new address for your transaction change to go to. This
will also be the basis for your DID. You can make one with bitcoin-cli:

```
$ bitcoin-cli getrawchangeaddress
myxJdFGMAnX4SiBg2hTKsZRr8ReE5irjS5
```

Finally, you need the private key for the address of your original
transaction. You can get it from bitcoin-cli:

```
$ bitcoin-cli dumpprivkey mvwGweRzRDwydpJfW1uqWJN4iZvNBZ9zZ4
randomrandomrandomrandomrandomrandomrandomrandomrand
```

Putting this all together, now you are ready to run `createBtcrDid`:

```
$ ./src/createBtcrDid \
79d864cc59b0c3ac240fc78e5a79edb13182b88c9ed1c60526eda6657a5d5e9e \
myxJdFGMAnX4SiBg2hTKsZRr8ReE5irjS5 \
randomrandomrandomrandomrandomrandomrandomrandomrand \
0.0005 \
https://raw.githubusercontent.com/danpape/self/master/ddo-ext.jsonld

Transaction submitted. Result txid: cd94e5a4a1aa1b19988faed93d31d50195b75390130304358369a63e8caec5ef

```

Using a block explorer, you can check that it looks good. For instance,
you can look up this example transaction on
[blockcypher.com](https://live.blockcypher.com/btc-testnet/tx/cd94e5a4a1aa1b19988faed93d31d50195b75390130304358369a63e8caec5ef/)
or [smartbit.com.au](https://testnet.smartbit.com.au/tx/cd94e5a4a1aa1b19988faed93d31d50195b75390130304358369a63e8caec5ef)
and can see that 0.0005 BTC was spent, from
`mvwGweRzRDwydpJfW1uqWJN4iZvNBZ9zZ4` to
`myxJdFGMAnX4SiBg2hTKsZRr8ReE5irjS5`. There is also an extra TXO there,
with 0.0 BTC which contains the DDO linked from github.

Finally, you can again use `txid2txref` to get your DID:
```
$ ./src/txid2txref --txoIndex 1 cd94e5a4a1aa1b19988faed93d31d50195b75390130304358369a63e8caec5ef | jq '{did}'
{
  "did": "did:btcr:8km9-0zyz-qpqq-sutc-5x"
}
```

## Note: bech32bis update
In Decemeber, 2019, Pieter Wuille did [research](https://gist.github.com/sipa/a9845b37c1b298a7301c33a04090b2eb) into the error detecting 
properties of the bech32 encoding alorithm. Based on a problem and fix he found, an internal constant in the algorithm has been updated from `1` to `0x3FFFFFFF`. This 
has the effect of changing the checksum values embedded in the txrefs and DIDs output by our programs.

More info can be found [here](https://lists.linuxfoundation.org/pipermail/bitcoin-dev/2019-December/017521.html).

If you find that a DID is reported as invalid by BTCR, you may be using an old one with a (now) invalid checksum. In that case, you should update the txref part of your DID with the `txid2tref` program.  There are no known outdated implementations.
