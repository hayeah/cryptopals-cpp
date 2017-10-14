//
// Created by Howard Yeh on 2017-10-13.
//

#include <string>
#include <fstream>
#include <Poco/Base64Decoder.h>
#include <Poco/HexBinaryDecoder.h>
#include <sstream>

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
