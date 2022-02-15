# Steps

0. Export variables
```
export ACCOUNT=<myaccount>
export SENDER_WITHOUT_0X=<...> // tEVM address without leading 0x
export SENDER=0x${SENDER}
export RECEIVER=<0x...> // tEVM address
```
1. Build
```
cd ./evmdeposit
cmake .
make
```
2. Open a wallet
```
cleos -u https://mainnet.telos.net  push action eosio.evm openwallet '['"$ACCOUNT"', '"$SENDER_WITHOUT_0X"']' -p $ACCOUNT@active
```
3. Deploy contract
```
cleos -u https://mainnet.telos.net set contract $ACCOUNT evmdeposit evmdeposit.wasm evmdeposit.abi
```
4. Call action
```
cleos -u https://mainnet.telos.net  push action $ACCOUNT raw '['"$SENDER"', '"$RECEIVER"']' -p $ACCOUNT@active
```