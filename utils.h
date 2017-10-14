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

std::string xorstr(std::string const &input, std::string const &key) {
    std::string output;
    output.resize(input.length());

    for (int i = 0; i < input.length(); i++) {
        output[i] = input[i] ^ key[i % key.length()];
    }

    return output;
}

void padstr(std::string &str, char paddingByte, size_t blockSize) {
    std::string out;

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
