
#include <string>
#include <iostream>
#include "AESBlockCipher.h"
#include "utils.h"

// encrypt ptext = aes_ecb_encrypt(ptext ^ ctext_prev)
// decrypt ctext = aes_ecb_decrypt(ctext) ^ ctext_prev

std::string decryptCBC(AESBlockCipher const &c, std::string const &ctext, std::string const &iv) {
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

        auto block = c.decryptBlock(std::begin(ctext));
        out += xorstr(block, ctext_prev);
        ctext_prev = ctext;
    }

    return out;
}

std::string encryptCBC(AESBlockCipher const &c, std::string const &ptext, std::string const &iv) {
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
        ctext_prev = c.encryptBlock(std::begin(block));
        out += ctext_prev;
    }

    return out;
}

int main() {
    std::string key = "YELLOW SUBMARINE";

    std::string iv;
    iv.resize(16);

    auto cipher = AESBlockCipher(key);

    auto ctext = readBase64File("./10.txt");

//    std::string plaintext = "hello world";
//    padstr(plaintext, '0', 16);
//
//    auto block = cipher.encryptBlock(std::begin(plaintext));
//    auto text = cipher.decryptBlock(std::begin(block));
//
//    std::cout << "block:" << hexencode(block) << std::endl;
//    std::cout << "text:" << text << std::endl;


    auto ptext = decryptCBC(cipher, ctext, iv);
    std::cout << "AES CBC decoded text:\n" << ptext << std::endl;

    ctext = encryptCBC(cipher, "yellow submarine", iv);
    ptext = decryptCBC(cipher, ctext, iv);

    std::cout << "ctext:" << hexencode(ctext) << std::endl;
    std::cout << "ptext:" << ptext << std::endl;

    // key "0 (\x00\x00\x00 &c)"



//    cipher.decryptBlock()
}
