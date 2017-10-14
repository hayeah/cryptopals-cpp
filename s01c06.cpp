#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "Poco/Base64Decoder.h"

struct testcase {
    char keychar;
    float score;
    std::string decodedStr;
};


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

std::string xorstringkey(std::string const &input, std::string const &key) {
    std::string output;
    output.resize(input.length());

    for (int i = 0; i < input.length(); i++) {
        output[i] = input[i] ^ key[i % key.length()];
    }

    return output;
}

std::string xorstr(std::string const &a, char key) {
    std::string out;

    for (int i = 0; i < a.length(); i++) {
        out.push_back(a[i] ^ key);
    }

    return out;
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

        cases.push_back(testcase{c, score, str});
    }

    std::sort(cases.begin(), cases.end(), [](const testcase &a, const testcase &b) -> bool {
        return a.score < b.score;
    });

    return cases.front();
}


std::string decodeBase64(std::string const &b64s) {
    std::string out;

    std::istringstream in(b64s);
    Poco::Base64Decoder dec(in);

    int c = dec.get();
    while (c != -1) {
        out += c;
        c = dec.get();
    }

    return out;
}

int hammingWeight8(const char &c) {
    return (c * 0x8040201ULL & 0x11111111) % 0xF;
}

int hammingDistance(std::string const &a, std::string const &b) {
    if (a.length() != b.length()) {
        throw "two strings are not equal length";
    }

    int w = 0;

    for (size_t i = 0; i < a.length(); i++) {
        w += hammingWeight8(a[i] ^ b[i]);
    }

    return w;
}

float keysizeHammingDistance(std::string const &str, size_t keysize) {
    int i = 0;
    int d = 0;

    while ((i + 2) * keysize < str.length()) {
        auto s1 = str.substr(keysize * i, keysize);
        i++;
        auto s2 = str.substr(keysize * i, keysize);

        d += hammingDistance(s1, s2);
    }

    return float(d) / float(keysize * i);
}

float keysizeHammingDistanceOnce(std::string const &str, size_t keysize) {
    auto s1 = str.substr(0, keysize);
    auto s2 = str.substr(keysize, keysize);
    auto d = hammingDistance(s1, s2);

    return float(d) / float(keysize);
}

struct keysizeTest {
    size_t size;
    float score;
};

std::vector<keysizeTest> mostLikelyKeySizes(std::string const &str) {
    std::vector<keysizeTest> tests;
    for (size_t i = 2; i < 40; i++) {
        auto d = keysizeHammingDistance(str, i);
//        auto d = keysizeHammingDistanceOnce(str, i);

        tests.push_back(keysizeTest{i, d});
    }

    std::sort(tests.begin(), tests.end(), [](keysizeTest const &a, keysizeTest const &b) -> bool {
        return a.score < b.score;
    });

    return tests;
}

std::vector<std::string> transposeBlocks(std::string const &str, size_t keysize) {
    std::vector<std::string> blocks;
    for (int i = 0; i < keysize; i++) {
        std::string block;
        int j = 0;
        while (j * keysize + i < str.length()) {
            block += str[j * keysize + i];
            j++;
        }

        blocks.push_back(block);
    }

    return blocks;
}

std::string guessKey(std::string const &str, size_t keysize) {
    std::string key;
    auto blocks = transposeBlocks(str, keysize);

    for (auto const &block : blocks) {
        auto guess = bestcase(block);
        key += guess.keychar;
    }

    return key;
}

std::string readBase64File(std::string const &filepath) {
    std::fstream f(filepath);

    std::string data;
    std::string line;
    while (std::getline(f, line)) {
        data.append(line);
    }

    f.close();

    return decodeBase64(data);
}

int main() {
    std::fstream f("./6.txt");

    std::string data;
    std::string line;
    while (std::getline(f, line)) {
        data.append(line);
    }

    std::cout << data << std::endl;

    data = decodeBase64(data);

//    std::cout << "len: " << data.length() << std::endl;
//    std::cout << "h: " << hammingDistance("this is a test", "wokka wokka!!!") << std::endl;

    auto keysizeTests = mostLikelyKeySizes(data);

    for (auto const &t : keysizeTests) {
        std::cout << "keysize: " << t.size << std::endl;
        std::cout << "d: " << t.score << std::endl;
    }

    for (int i = 0; i < 3 && i < keysizeTests.size(); i++) {
        auto test = keysizeTests[i];
        auto key = guessKey(data, test.size);

        auto result = xorstringkey(data, key);

        std::cout << "key: " << key.length() << std::endl;
        std::cout << "key size: " << key << std::endl;
        std::cout << "res:\n" << result << std::endl;
    }
}