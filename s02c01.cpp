
#include <string>
#include <iostream>

void padBlock(std::string &str, char paddingByte, size_t blockSize) {
    if (str.length() > blockSize) {
        throw "string cannot be longer than block size";
    }

    if (str.length() == blockSize) {
        return;
    }

    auto padN = blockSize - str.length();

    for (size_t i = 0; i < padN; i++) {
        str.push_back(paddingByte);
    }
}

int main() {
    std::string s = "YELLOW SUBMARINE";
    padBlock(s, 'a', 20);

    std::cout << s << std::endl;
}
