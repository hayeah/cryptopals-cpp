//
// Created by Howard Yeh on 2017-10-14.
//
#pragma once

#include "utils.h"
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

    std::string decryptBlock(std::string::const_iterator it) const {
        AES_ecb_encrypt((const unsigned char *) it.base(), (unsigned char *) buf.c_str(), &decryptKey,
                        AES_DECRYPT);
        return buf;
    }

    std::string encryptBlock(std::string::const_iterator it) const {
        AES_ecb_encrypt((const unsigned char *) it.base(), (unsigned char *) buf.c_str(), &encryptKey,
                        AES_ENCRYPT);
        return buf;
    }

    std::string encryptECB(std::string const &ptext) const {
        if (ptext.length() % 16 != 0) {
            throw "AES ECB plain text must be blocks of 16 bytes";
        }

        std::string out;
        for (auto it = std::begin(ptext); it < std::end(ptext); it += 16) {
            out += encryptBlock(it);
        }

        return out;
    }

    std::string decryptECB(std::string const &ctext) const {
        if (ctext.length() % 16 != 0) {
            throw "AES ECB cipher text must be blocks of 16 bytes";
        }

        std::string out;
        for (auto it = std::begin(ctext); it < std::end(ctext); it += 16) {
            out += decryptBlock(it);
        }

        return out;
    }

    std::string encryptCBC(std::string const &ptext, std::string const &iv) {
        if (iv.length() % 16 != 0) {
            throw "AES CBC IV must be 16 bytes";
        }

        if (ptext.length() % 16 != 0) {
            throw "AES CBC plain text must be blocks of 16 bytes";
        }

        std::string out;
        std::string ctext_prev = iv;

        for (auto it = std::begin(ptext); it < std::end(ptext); it += 16) {
            auto block = xorstr(std::string(it, it + 16), ctext_prev);
            ctext_prev = encryptBlock(std::begin(block));
            out += ctext_prev;
        }

        return out;
    }

    std::string decryptCBC(std::string const &ctext, std::string const &iv) {
        if (iv.length() % 16 != 0) {
            throw "AES CBC IV must be 16 bytes";
        }

        if (ctext.length() % 16 != 0) {
            throw "AES CBC cipher text must be blocks of 16 bytes";
        }

        std::string out;

        std::string ctext_prev = iv;

        for (auto it = std::begin(ctext); it < std::end(ctext); it += 16) {
            auto ctext = std::string(it, it + 16);

            auto block = decryptBlock(std::begin(ctext));
            out += xorstr(block, ctext_prev);
            ctext_prev = ctext;
        }

        return out;
    }

private:

    AES_KEY decryptKey;
    AES_KEY encryptKey;

    std::string buf;
};
