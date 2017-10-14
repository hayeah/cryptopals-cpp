//
// Created by Howard Yeh on 2017-10-14.
//

#include <string>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>

#include "utils.h"
#include "AESBlockCipher.h"

std::string randstr(size_t n) {
    // take advantage of buffering
    static std::fstream rf("/dev/urandom");

    std::string out;
    out.resize(n);
    rf.get(std::begin(out).base(), n);
    return out;
}

std::string rand_encode(std::string const &ptext) {

    auto padbefore = rand() % 5 + 5;
    auto padafter = rand() % 5 + 5;

    auto text = padstr(randstr(padbefore) + ptext + randstr(padafter), '\0', 16);

    auto key = randstr(16);
    auto cipher = AESBlockCipher(key);

    auto iv = randstr(16);

    if (rand() % 2) {
        std::cout << "cbc ";
        return cipher.encryptCBC(text, iv);
    } else {
        std::cout << "ecb ";
        return cipher.encryptECB(text);
    }
}

enum EncodingMode {
    ECBEncryption,
    CBCEncryption,
};

EncodingMode encoderOracle(std::function<std::string(std::string)> encoder) {
    auto ptext = std::string(64, '0');
    auto ctext = encoder(ptext);

    std::map<std::string, bool> m;

    for (auto it = std::begin(ctext); it < std::end(ctext); it += 16) {
        auto block = std::string(it, it + 16);
        if (m[block]) {
            return ECBEncryption;
        }

        m[block] = true;
    }

    return CBCEncryption;
};

int main() {
    srand(time(NULL));

    for (int i = 0; i < 300; i++) {
        auto mode = encoderOracle(rand_encode);
        if (mode == ECBEncryption) {
            std::cout << "detected ECB" << std::endl;
        } else {
            std::cout << "detected CBC" << std::endl;
        }
    }
//    std::string text = "yellow submarineyellow submarine";
//
//    auto key = randstr(16);
//    auto cipher = AESBlockCipher(key);
//    auto ctext = cipher.encryptECB(text);
//
//    std::cout << "ctext:" << hexencode(ctext) << std::endl;
//    std::cout << "ptext:" << cipher.decryptECB(ctext) << std::endl;
}