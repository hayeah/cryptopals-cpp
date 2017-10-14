//
// Created by Howard Yeh on 2017-10-14.
//
#pragma once
#include "openssl/aes.h"

class AESBlockCipher {
public:
    std::string key;

    AESBlockCipher(std::string key) {
        this->key = key;
        this->buf.resize(16);

        AES_set_decrypt_key((const unsigned char *) key.c_str(), 16 * 8, &this->decryptKey);
        AES_set_encrypt_key((const unsigned char *) key.c_str(), 16 * 8, &this->encryptKey);
    }

    std::string decryptBlock(std::string::iterator it) const {
        AES_ecb_encrypt((const unsigned char *) it.base(), (unsigned char *) buf.c_str(), &this->decryptKey, AES_DECRYPT);
        return buf;
    }

    std::string encryptBlock(std::string::iterator it) const {
        AES_ecb_encrypt((const unsigned char *) it.base(), (unsigned char *) buf.c_str(), &this->encryptKey, AES_ENCRYPT);
        return buf;
    }
private:

    AES_KEY decryptKey;
    AES_KEY encryptKey;

    std::string buf;
};
