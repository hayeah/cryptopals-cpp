//
// Created by Howard Yeh on 2017-10-14.
//
#include <iostream>
#include <map>
#include <thread>
#include "utils.h"
#include "AESBlockCipher.h"


class Oracle {
public:
    Oracle(std::string const &key, const std::string &_secret) : secret(_secret), cipher(AESBlockCipher(key)) {
    }

    std::string encrypt(std::string prefix) const {
        auto rlen = rand() % 5 + 5;
        auto rprefix = randstr(rlen);
        auto ptext = rprefix + prefix + secret;
        return cipher.encryptECB(padstr(ptext, '\0', 16));
    }

private:
    AESBlockCipher cipher;
    std::string secret;
};



std::string alignEncrypt(Oracle const &o, std::string const &guessText, size_t bsize) {
    // X = unknown target text
    // R = unknown random padding
    //
    // string to construct (assuming bsize = 4 for illustration):
    //
    // [R0 R1 RP RP] [DP DP DP DP] [DP DP DP DP] [GP GP GP X1] [X2 X3 X4]
    //
    // RP = random padding to align the random prefix
    // DP = duplicate blocks to detect alignment success
    // GP = padding attack prefix
    //
    // G = guess text = GP + X
    //
    // @returns encrypt(G)

    static auto MAX_TRIES = size_t {1000};

    // DP should use a different padding char from GP
    auto DP = std::string(bsize * 2, '\1');

    for (size_t i = 0; i < MAX_TRIES; i++) {
        auto RPlen = rand() % bsize;
        auto RP = randstr(RPlen);

        // alignment success if 2nd and 3rd blocks are the same

        auto str = RP + DP + guessText;
        auto ctext = o.encrypt(str);

        auto block2 = ctext.substr(bsize, bsize);
        auto block3 = ctext.substr(bsize*2, bsize);

        if (block2 == block3) {
            auto alignedCtext = ctext.substr(bsize*3, ctext.length() - bsize*3);
            return alignedCtext;
        }
    }

    throw "Failed to align encryption blocks";
}

std::string recoverSecret(Oracle const &o, size_t bsize) {
    std::string emptyBlock = std::string(bsize, '\0');
    std::string decodedBlock;
    std::string out;

    auto prefix = std::string(bsize - 1, '\0');

    for (size_t j = 0; j < bsize * 100; j++) {
        auto nthblock = j / bsize;
        auto byteoffset = nthblock * bsize;

        auto ctext = alignEncrypt(o, prefix, bsize);
        auto block = std::string(std::begin(ctext) + byteoffset, std::begin(ctext) + byteoffset + bsize);

        auto guessText = prefix + out + decodedBlock + '\0';

        std::cout << "decode nth byte:" << j << std::endl;
//        std::cout << "decoded:" << out + decodedBlock << std::endl;
//        for (int i = 0; i <= 128; i++) {
        // test only printable characters (missing \n)
//        for (int i = 0x20; i <= 0x7e + 1; i++) {
        for (int i = 0; i <= 128; i++) {
            std::cout << "guessing:" << out + decodedBlock + char(i) << std::endl;
            guessText[guessText.length() - 1] = i;
            auto ctext2 = alignEncrypt(o, guessText, bsize);
            auto block2 = std::string(std::begin(ctext2) + byteoffset, std::begin(ctext2) + byteoffset + bsize);

            if (block2 == block) {
                decodedBlock += i;
                // shift prefix to the left, and set the newly detected char as the last char
                prefix = prefix.substr(0, prefix.length() - 1);

                if (j > 0 && decodedBlock.length() % bsize == 0) {
                    if (decodedBlock == emptyBlock) {
                        return out;
                    }
                    out += decodedBlock;
                    decodedBlock = "";
                    prefix = std::string(bsize - 1, '\0');
                }
                break;
            }

            if (i == 128) {
                throw "failed to decode byte";
            }
        }
    }


    return out;
}


int main() {
    auto unknownStr = readBase64File("./12.txt");

    auto key = randstr(16);
    std::cout << "key:\n" << hexencode(key) << std::endl;
    key = randstr(16);
    std::cout << "key:\n" << hexencode(key) << std::endl;
    key = randstr(16);
    std::cout << "key:\n" << hexencode(key) << std::endl;
    key = randstr(16);
    std::cout << "key:\n" << hexencode(key) << std::endl;

    auto cipher = AESBlockCipher(key);

    auto oracle = Oracle(key, unknownStr);


    std::string myprefix = "hello ecb";

    auto ctext = oracle.encrypt(myprefix);


    auto bsize = size_t {16};

    std::cout << "ctext:\n" << hexencode(ctext) << std::endl;
    std::cout << "bsize:\n" << bsize << std::endl;



    auto secret = recoverSecret(oracle, bsize);

    std::cout << "secret:\n" << secret << std::endl;
}