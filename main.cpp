#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include "Poco/HexBinaryDecoder.h"
#include "Poco/HexBinaryEncoder.h"

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
        {'a', 0.08167},
        {'b', 0.01492},
        {'c', 0.02782},
        {'d', 0.04253},
        {'e', 0.12702},
        {'f', 0.02228},
        {'g', 0.02015},
        {'h', 0.06094},
        {'i', 0.06966},
        {'j', 0.00153},
        {'k', 0.00772},
        {'l', 0.04025},
        {'m', 0.02406},
        {'n', 0.06749},
        {'o', 0.07507},
        {'p', 0.01929},
        {'q', 0.00095},
        {'r', 0.05987},
        {'s', 0.06327},
        {'t', 0.09056},
        {'u', 0.02758},
        {'v', 0.00978},
        {'w', 0.02360},
        {'x', 0.00150},
        {'y', 0.01974},
        {'z', 0.00074},

        // penalty for punctuations
        {'.', 1},
        {',', 1},
        {' ', 1},
        {'\'', 1},
};

float englishScore(std::string const &str) {
    std::vector<int> charCounts(127);

    for (auto c : str) {
        charCounts[c] += 1;
    }

    float score = 0;
    auto len = str.length();

    for (char i; i < 127; i++) {
        float freq = float(charCounts[i]) / float(len);

        if (freq == 0) {
            continue;
        }

        auto efreq = efreqs[i];
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

int main() {

    auto input = hexdecode("1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736");

    char c = 'X';

    struct testcase {
        char keychar;
        float score;
        std::string decodedStr;
    };


    auto cases = std::vector<testcase>{};

    for (char c = 0; c < 127; c++) {
        auto str = decode(input, c);
        auto score = englishScore(str);

        cases.push_back(testcase{c, score, str});
    }

    std::sort(cases.begin(), cases.end(), [](const testcase &a, const testcase &b) -> bool {
        return a.score > b.score;
    });


    std::vector<testcase> bestCases(cases.end()-10, cases.end());


    for (auto c : bestCases) {
        std::cout << c.keychar << ": " << c.decodedStr << std::endl;
        std::cout << "score: " << c.score << std::endl;
    }

    return 0;
}