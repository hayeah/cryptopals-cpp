//#include <openssl/conf.h>
#include <iostream>
#include <fstream>

#include "openssl/aes.h"

#import "utils.cpp"

int main() {
    auto key = AES_KEY();

    AES_set_decrypt_key((const unsigned char *) "YELLOW SUBMARINE", 16 * 8, &key);

    auto data = readBase64File("./7.txt");

    std::string out;
    std::string outbuf;
    outbuf.resize(16);

    for (int i = 0; i < data.length(); i += 16) {
        auto it = data.begin() + i;
        AES_ecb_encrypt((const unsigned char *) it.base(), (unsigned char *) outbuf.c_str(), &key, AES_DECRYPT);
        out += outbuf;
    }


//    std::cout << "data:" << data << std::endl;
    std::cout << out << std::endl;
    std::cout << "ok" << std::endl;

}