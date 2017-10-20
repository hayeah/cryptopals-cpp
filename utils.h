//
// Created by Howard Yeh on 2017-10-13.
//

#pragma once

#include <string>
#include <fstream>
#include <Poco/Base64Decoder.h>
#include <Poco/HexBinaryDecoder.h>
#include <sstream>
#include <Poco/HexBinaryEncoder.h>
#include <map>

std::string
randstr(size_t n) {
    // take advantage of buffering
    static std::fstream rf("/dev/urandom");

    std::string out;
    out.resize(n);
    rf.get(std::begin(out).base(), n);
    return out;
}

std::string
padstr(std::string const &str, char padchar, unsigned int mod) {
    auto olen = str.length();
    if (olen % mod == 0) {
        return str;
    }

    std::string out = str;

    auto padn = mod - olen % mod;
    out.resize(olen + padn);

    for (size_t i = 0; i < padn; i++) {
        out[olen + i] = padchar;
    }

    return out;
}

std::string xorstr(std::string const &input, std::string const &key) {
    std::string output;
    output.resize(input.length());

    for (int i = 0; i < input.length(); i++) {
        output[i] = input[i] ^ key[i % key.length()];
    }

    return output;
}

//void padstr(std::string &str, char paddingByte, size_t blockSize) {
//    std::string out;
//
//    if (str.length() > blockSize) {
//        throw "string cannot be longer than block size";
//    }
//
//    if (str.length() == blockSize) {
//        return;
//    }
//
//    auto padN = blockSize - str.length();
//
//    for (size_t i = 0; i < padN; i++) {
//        str.push_back(paddingByte);
//    }
//}

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

std::string hexencode(std::string const &str) {
    std::string out;
    std::ostringstream ss(out);
    Poco::HexBinaryEncoder encoder(ss);

    encoder << str;
    encoder.close();

    return ss.str();
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

