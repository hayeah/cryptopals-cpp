#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include "Poco/HexBinaryDecoder.h"
#include "Poco/HexBinaryEncoder.h"

struct testcase {
    char keychar;
    float score;
    std::string decodedStr;
};


std::string xorstr(std::string const &a, char key) {
    std::string out;

    for (int i = 0; i < a.length(); i++) {
        out.push_back(a[i] ^ key);
    }

    return out;
}

std::string hexencode(std::string const &str) {
    std::string out;
    std::ostringstream ss(out);
    Poco::HexBinaryEncoder encoder(ss);

    encoder << str;
    encoder.close();

    return ss.str();
}

std::string hexdecode(std::string const &str) {
    std::string out;
    std::istringstream ss(str);
    Poco::HexBinaryDecoder decoder(ss);

    char c;
    while (decoder.get(c)) {
        out += c;
    }

    return out;
}

auto efreqs = std::map<char, float>{
        {'a',  0.08167},
        {'b',  0.01492},
        {'c',  0.02782},
        {'d',  0.04253},
        {'e',  0.12702},
        {'f',  0.02228},
        {'g',  0.02015},
        {'h',  0.06094},
        {'i',  0.06966},
        {'j',  0.00153},
        {'k',  0.00772},
        {'l',  0.04025},
        {'m',  0.02406},
        {'n',  0.06749},
        {'o',  0.07507},
        {'p',  0.01929},
        {'q',  0.00095},
        {'r',  0.05987},
        {'s',  0.06327},
        {'t',  0.09056},
        {'u',  0.02758},
        {'v',  0.00978},
        {'w',  0.02360},
        {'x',  0.00150},
        {'y',  0.01974},
        {'z',  0.00074},

        // penalty for punctuations
        {'.',  1},
        {',',  1},
        {' ',  1},
        {'\'', 1},
};

float englishScore(std::string const &str) {
    float score = 0;
    std::map<char, int> charCounts;

    for (auto c : str) {
        charCounts[c] += 1;
    }


    auto len = str.length();

    for (char i; i < 127; i++) {
        float freq = float(charCounts[i]) / float(len);

        if (freq == 0) {
            continue;
        }

        float efreq;
        if (efreqs.find(i) == efreqs.end()) {
            efreq = 0;
        } else {
            efreq = efreqs.at(i);
        }

        if (efreq == 0) {
            // penalty for non-english punctuations
            score += 5;
        } else {
            auto d = (efreq - freq);
            score += d * d;
        }
    }

    return score;
}

std::string decode(std::string const &str, char c) {
    auto strxored = xorstr(str, c);
    std::string ldstr;
    ldstr.resize(strxored.size());
    std::transform(strxored.begin(), strxored.end(), ldstr.begin(), ::tolower);

    return ldstr;
}

const testcase &bestcase(std::string const &input) {
    auto cases = std::vector<testcase>();

    for (char c = 0; c < 127; c++) {
        auto str = decode(input, c);
        auto score = englishScore(str);

//        cases[c] = testcase{c, score, str};
        cases.push_back(testcase{c, score, str});
    }

    std::sort(cases.begin(), cases.end(), [](const testcase &a, const testcase &b) -> bool {
        return a.score < b.score;
    });

    return cases.front();
}


int main() {
    auto f = std::fstream("./4.txt");

    auto line = std::string();

    while (std::getline(f, line)) {
//        std::cout << line << std::endl;

        auto input = hexdecode(line);
        auto c = bestcase(input);

        if (c.score < 10) {
            std::cout << "line: " << line << std::endl;
            std::cout << c.keychar << ": " << c.decodedStr << std::endl;
            std::cout << "score: " << c.score << std::endl;
        }
    }

    return 0;
}