#include <iostream>
#include <fstream>

#import "utils.h"

#include "AESBlockCipher.h"

int main() {

    AESBlockCipher cipher("YELLOW SUBMARINE");

    auto data = readBase64File("./7.txt");

    std::string out;

    for (int i = 0; i < data.length(); i += 16) {
        auto it = data.begin() + i;
        auto decryptedBlock = cipher.decryptBlock(it);
        out += decryptedBlock;
    }

    std::cout << out << std::endl;
    std::cout << "ok" << std::endl;
}