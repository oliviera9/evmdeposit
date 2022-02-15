#include "evmdeposit.hpp"
#include <rlp/rlp.hpp>

evmdeposit::evmdeposit(name self, name code, datastream<const char *> ds) : contract(self, code, ds), _accounts(EVM_CONTRACT, EVM_CONTRACT.value) {}

evmdeposit::~evmdeposit() {}

std::vector<uint8_t> to_bin(std::string binstr)
{
    std::vector<uint8_t> out;
    for (auto i = 0; i < binstr.size(); i++)
    {
        out.push_back((uint8_t)binstr[i]);
    }
    return out;
}
std::vector<uint8_t> hex2bin(std::string hexstr)
{
    size_t len = hexstr.size();
    if (len % 2 != 0)
        return {}; // ERROR

    size_t final_len = len / 2;
    std::vector<uint8_t> chrs(final_len);
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
    {
        chrs[j] = (std::toupper(hexstr[i]) % 32 + 9) % 25 * 16 + (std::toupper(hexstr[i + 1]) % 32 + 9) % 25;
    }
    return chrs;
}
std::string bin2hex(const std::vector<uint8_t> &bin)
{
    std::string res;
    const char hex[] = "0123456789abcdef";
    for (auto byte : bin)
    {
        res += hex[byte >> 4];
        res += hex[byte & 0xf];
    }

    return res;
}

ACTION evmdeposit::raw(std::string receiver, std::string sender)
{
    auto accounts_byaddress = _accounts.get_index<eosio::name("byaddress")>();
    eosio::checksum160 addr_160 = eosio_evm::toChecksum160(sender.substr(2, 42));
    eosio::checksum256 addr_256 = eosio_evm::pad160(addr_160);
    auto account = accounts_byaddress.find(addr_256);
    check(account != accounts_byaddress.end(), "not existing account");
    uint256_t nonce = account == accounts_byaddress.end() ? 0 : account->get_nonce(); // A scalar value equal to the number of transactions sent by the sender;
    uint256_t gas_price = 500000000000;                                               // A scalar value equal to the number of Wei to be paid per unit of gas for all computation costs incurred as a result of the execution of this transaction;
    uint256_t gas_limit = 30000;                                                      // A scalar value equal to the maximum amount of gas that should be used in executing this transaction
    std::vector<uint8_t> to = hex2bin(receiver.substr(2, 42));                        // Currently set as vector of bytes.
    uint256_t value = 0;                                                              // A scalar value equal to the number of Wei to be transferred to the message call’s recipient or, in the case of contract creation, as an endowment to the newly created account; forma
    std::vector<uint8_t> data = hex2bin("");                                          // An unlimited size byte array specifying the input data of the message call
    std::string tx = rlp::encode(nonce, gas_price, gas_limit, to, value, data, 27, 0, 0);
    // std::string tx2 = bin2hex(to_bin(tx));
    // check(1 == 2, "invalid check " + tx2);
    action(permission_level{get_self(), name("active")}, EVM_CONTRACT, name("raw"),
           make_tuple(get_self(),
                      tx,
                      addr_160))
        .send();
}
