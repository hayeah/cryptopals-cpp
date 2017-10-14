//
// Created by Howard Yeh on 2017-10-14.
//

#include <fstream>
#include <map>
#include <iostream>

#import "utils.cpp"

bool analyzeRepeatBlock(std::string const &str) {
    std::map<std::string, bool> m;


    for (size_t i = 0; i < str.length(); i += 16) {
        auto n = std::min(str.length() - i, 16ul);
        auto block = str.substr(i, n);

        if (m[block] == true) {
            return true;
        } else {
            m[block] = true;
        }
    }

    return false;
}

int main() {
    std::fstream f("./8.txt");

    std::string line;
    while (std::getline(f, line)) {
        auto data = hexdecode(line);
        auto repeat = analyzeRepeatBlock(data);
        if (repeat) {
            std::cout << "found: " << line << std::endl;
            break;
        }
    }
}