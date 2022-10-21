//
//  signature.hpp
//  sources
//
//  Created by 007 on 2022/1/18.
//

#ifndef signature_hpp
#define signature_hpp

#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>
#include "px_global_help.h"
#include "libwallycore.h"
class Signature
{
public:
    Signature();
    void LoadWallyCore();
    void libwally_example();
    char* GetSignature();
private:
    std::string chain_file_path;
    std::string libwally_path;
    std::string signature_result;
    Wally_init wally_init1;
    Bip39_mnemonic_from_bytes bip39_mnemonic_from_bytes1;
    Bip39_mnemonic_to_seed bip39_mnemonic_to_seed1;
    Wally_free_string wally_free_string1;
    Bip32_key_from_seed_alloc bip32_key_from_seed_alloc1;
    Bip32_key_to_base58 bip32_key_to_base581;
    Bip32_key_from_parent_path_alloc bip32_key_from_parent_path_alloc1;
    Wally_bip32_key_to_addr_segwit wally_bip32_key_to_addr_segwit1;
    Wally_bip32_key_to_address wally_bip32_key_to_address1;
    
    Wally_psbt_from_base64 wally_psbt_from_base641;
    Wally_scriptpubkey_p2pkh_from_bytes wally_scriptpubkey_p2pkh_from_bytes1;
    Wally_tx_get_btc_signature_hash wally_tx_get_btc_signature_hash1;
    Wally_ec_sig_from_bytes wally_ec_sig_from_bytes1;
    Bip32_key_free bip32_key_free1;
    Wally_ec_sig_to_der wally_ec_sig_to_der1;
    Wally_psbt_to_base64 wally_psbt_to_base641;
    Wally_psbt_free wally_psbt_free1;
    Wally_cleanup wally_cleanup1;
};
#endif /* signature_hpp */
