#include <iostream>
#include "Poco/URI.h"
#include "AESBlockCipher.h"
#include "utils.h"

class User {
public:
    User(const std::string &email, int uid, const std::string &role) : email(email), uid(uid), role(role) {}

    std::string rawQuery() const {
        Poco::URI uri;
        uri.addQueryParameter("email", email);
        uri.addQueryParameter("uid", std::to_string(uid));
        uri.addQueryParameter("role", role);

        return uri.getQuery();
//        return uri.getRawQuery();
    }
private:
    std::string role;
    std::string email;
    int uid;
};

class Encryptor {
public:
    Encryptor(const std::string &key): cipher{AESBlockCipher(key)} {
    }

    std::string encrypt(std::string ptext) const {
        return cipher.encryptECB(padstr(ptext, '\4', 16));
    }

    std::string decrypt(std::string ctext) const {
        return cipher.decryptECB(ctext);
    }

private:
    AESBlockCipher cipher;
    std::string secret;
};

auto key = randstr(16);
auto encryptor = Encryptor { key };

std::string profile_for(const std::string &email) {
    auto user = User { email, 10, "user" };
    auto qstr = user.rawQuery();

    return encryptor.encrypt(qstr);
}

// returns "" if failed to detect a dup block
std::string findDupBlock(std::string ctext, size_t bsize) {
    std::map<std::string, bool> m;

    std::string* dup = nullptr;

    for (auto it = std::begin(ctext); it + bsize < std::end(ctext); it += bsize) {
        auto block = std::string(it, it + bsize);
        if (m[block]) {
            return block;
        }

        m[block] = true;
    }

    return "";
}


std::string detectTextCipherBlock(std::string const &targetStr, size_t bsize) {
    if (targetStr.length() > bsize) {
        throw "targetStr needs to be shorter than block size";
    }

    // we expect this block cause a dup block in cipher text, given the right prefix length.
    auto blocks = padstr(targetStr, '\4', bsize) + padstr(targetStr, '\4', bsize);
    auto prefix = std::string { "" };
    prefix.reserve(bsize);

    // try different prefix lengths to align the blocks
    for (int i = 0; i < bsize; i++) {
        auto ptext = prefix + blocks;
        auto ctext = profile_for(ptext);
        auto dup = findDupBlock(ctext, bsize);
        if (dup != "") {
            return dup;
        }

        prefix += '0';
    }

    throw "cannot find dup block for target string";
}

std::string ecbCopyAndPaste(std::string const &fromBlock, std::string const &toBlock, std::function<std::string(std::string)> crypt) {
    auto bsize = fromBlock.length();

    std::string input;

    // keep padding input until we find fromBlock at the end of cipher text
    for (int i = 0; i < bsize; i++) {
        auto ctext = crypt(input);
        auto lastBlock = ctext.substr(ctext.length()-bsize, bsize);
        if (lastBlock == fromBlock) {
            return ctext.substr(0, ctext.length()-bsize) + toBlock;
        }
        input += "0";
    }

    throw "cannot find fromBlock";

}


int main() {
    auto ctext = profile_for("hayeah@gmail.com");
    auto qstr = encryptor.decrypt(ctext);

    auto bsize = detectBlocksize(profile_for);
    std::cout << "bsize: " << bsize << std::endl;

    auto adminCipherBlock = detectTextCipherBlock("admin", bsize);
    std::cout << "targetStrBlock: " << hexencode(adminCipherBlock) << std::endl;

    auto userCipherBlock = detectTextCipherBlock("user", bsize);
    std::cout << "userCipherBlock: " << hexencode(userCipherBlock) << std::endl;


    std::cout << "e(qstr): " << hexencode(ctext) << std::endl;
    std::cout << "d(qstr): " << qstr << std::endl;

    auto ctextAdmin = ecbCopyAndPaste(userCipherBlock, adminCipherBlock, profile_for);
    std::cout << "ctextAdmin: " << hexencode(ctextAdmin) << std::endl;
    std::cout << "d(ctextAdmin): " << encryptor.decrypt(ctextAdmin) << std::endl;
}