//
//  libwallycore.h
//  all
//
//  Created by 007 on 2022/1/21.
//

#ifndef libwallycore_h
#define libwallycore_h
//#include <_size_t.h>
//#include <_uint32_t.h>
typedef int (*Wally_init)(uint32_t flags);
typedef int (*Bip39_mnemonic_from_bytes)(const struct words *w,
                                         const unsigned char *bytes,
                                         size_t bytes_len,
                                         char **output);
typedef int (*Bip39_mnemonic_to_seed)( const char *mnemonic,
                                      const char *passphrase,
                                      unsigned char *bytes_out,
                                      size_t len,
                                      size_t *written);
typedef int (*Wally_free_string)(char *str);
typedef int (*Bip32_key_from_seed_alloc)(const unsigned char *bytes,
                                         size_t bytes_len,
                                         uint32_t version,
                                         uint32_t flags,
                                         struct ext_key **output);
typedef int (*Bip32_key_to_base58)(const struct ext_key *hdkey,
                                   uint32_t flags,
                                   char **output);
typedef int (*Bip32_key_from_parent_path_alloc)(const struct ext_key                                               *hdkey,
                                                const uint32_t *child_path,
                                                size_t child_path_len,
                                                uint32_t flags,
                                                struct ext_key **output);
typedef int (*Bip32_key_to_base58)(const struct ext_key *hdkey,
                                   uint32_t flags,
                                   char **output);
typedef int (*Wally_bip32_key_to_addr_segwit)(const struct ext_key                                              *hdkey,
                                              const char *addr_family,
                                              uint32_t flags,
                                              char **output);
typedef int (*Wally_bip32_key_to_address)(const struct ext_key *hdkey,
                                          uint32_t flags,
                                          uint32_t version,
                                          char **output);
typedef int (*Wally_psbt_from_base64)(
                                   const char *base64,
                                   struct wally_psbt **output);
typedef int (*Wally_scriptpubkey_p2pkh_from_bytes)(
                                                const unsigned char *bytes,
                                                size_t bytes_len,
                                                uint32_t flags,
                                                unsigned char *bytes_out,
                                                size_t len,
                                                size_t *written);
typedef int (*Wally_tx_get_btc_signature_hash)(
                                            const struct wally_tx *tx,
                                            size_t index,
                                            const unsigned char *script,
                                            size_t script_len,
                                            uint64_t satoshi,
                                            uint32_t sighash,
                                            uint32_t flags,
                                            unsigned char *bytes_out,
                                            size_t len);
typedef  int (*Wally_ec_sig_from_bytes)(
                                     const unsigned char *priv_key,
                                     size_t priv_key_len,
                                     const unsigned char *bytes,
                                     size_t bytes_len,
                                     uint32_t flags,
                                     unsigned char *bytes_out,
                                     size_t len);
typedef int (*Bip32_key_free)(
                           const struct ext_key *hdkey);
typedef int (*Wally_ec_sig_to_der)(
                                const unsigned char *sig,
                                size_t sig_len,
                                unsigned char *bytes_out,
                                size_t len,
                                size_t *written);
typedef int (*Wally_psbt_to_base64)(
                                 const struct wally_psbt *psbt,
                                 uint32_t flags,
                                 char **output);
typedef int (*Wally_psbt_free)(
                            struct wally_psbt *psbt);
typedef int (*Wally_cleanup)(uint32_t flags);
#endif /* libwallycore_h */
