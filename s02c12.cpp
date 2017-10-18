//
// Created by Howard Yeh on 2017-10-14.
//
#include <iostream>
#include <map>
#include "utils.h"
#include "AESBlockCipher.h"

auto key = randstr(16);
auto cipher = AESBlockCipher(key);

std::string encoderWithRandPadding(std::string const &ptext) {
//    auto padbefore = rand() % 5 + 5;
//    auto padafter = rand() % 5 + 5;
//
//    auto text = padstr(randstr(padbefore) + ptext + randstr(padafter), '\0', 16);

    // no random padding for this exercise
    auto text = padstr(ptext, '\0', 16);

    return cipher.encryptECB(text);
}

bool hasRepeatBlock(std::string ctext, size_t bsize) {
    std::map<std::string, bool> m;

    for (auto it = std::begin(ctext); it + bsize < std::end(ctext); it += bsize) {
        auto block = std::string(it, it + bsize);
        if (m[block]) {
            return true;
        }

        m[block] = true;
    }

    return false;
}

// returns block size. 0 if cannot find a block size.
int detectBlocksize(std::function<std::string(std::string)> crypt) {

    for (size_t bsize = 2; bsize < 1024; bsize++) {
        // assuming that left and right paddings are less than block size, genearting 3 blocks will gurantee that the same
        // bytes straddle 2 blocks. Then we detect repeat.
        auto s = std::string(bsize * 3, '0');
        auto ctext = crypt(s);
        if (hasRepeatBlock(ctext, bsize)) {
            return bsize;
        }
    }

    return 0;
}


class Oracle {
public:
    Oracle(std::string key, std::string _secret): cipher{AESBlockCipher(key)}, secret(_secret) {
//        auto cipher = ;
//        this->cipher = cipher;
//        secret = secret;
    }

    std::string encrypt(std::string prefix) const {
        auto ptext = prefix + this->secret;
        return cipher.encryptECB(padstr(ptext, '\0', 16));
    }

private:
    AESBlockCipher cipher;
    std::string secret;
};


std::string recoverSecret(Oracle const &o, size_t bsize) {
    std::string emptyBlock = std::string(bsize, '\0');
    std::string decodedBlock;
    std::string out;

    auto prefix = std::string(bsize - 1, '\0');

    for (int j = 0; j < bsize * 100; j++) {
        auto nthblock = j / bsize;
        auto byteoffset = nthblock * bsize;

        auto ctext = o.encrypt(prefix);
        auto block = std::string(std::begin(ctext)+byteoffset, std::begin(ctext)+byteoffset+bsize);

        auto guessText = prefix + out + decodedBlock + '\0';
        for (int i = 0; i < 128; i++) {
            guessText[guessText.length() - 1] = i;
            auto ctext2 = o.encrypt(guessText);
            auto block2 = std::string(std::begin(ctext2)+byteoffset, std::begin(ctext2)+byteoffset+bsize);

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
        }
    }


    return out;
}


int main() {
    auto unknownStr = readBase64File("./12.txt");

    auto key = randstr(16);
    auto cipher = AESBlockCipher(key);

    auto oracle = Oracle(key, unknownStr);


    std::string myprefix = "hello ecb";

    auto ctext = oracle.encrypt(myprefix);

//    auto ctext = encoderWithRandPadding(myprefix + unknownStr);

    auto bsize = detectBlocksize(encoderWithRandPadding);

    std::cout << "ctext:\n" << hexencode(ctext) << std::endl;
    std::cout << "bsize:\n" << bsize << std::endl;

    auto secret = recoverSecret(oracle, bsize);
    std::cout << "secret:\n" << secret << std::endl;
}