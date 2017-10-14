#include <string>
#include <iostream>

std::string xorstring(std::string const& input, std::string const& key) {
    std::string output;
    output.resize(input.length());

    for (int i = 0; i < input.length(); i++) {
        output[i] = input[i] ^ key[i % key.length()];
    }

    return output;
}

std::string str2hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

int main() {
    std::string key = "ICE";
    std::string s1 = "Burning 'em, if you ain't quick and nimble I go crazy when I hear a cymbal";

    auto o1 = str2hex(xorstring(s1, key));

    std::cout << o1 << std::endl;
}