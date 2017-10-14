//
// Created by Howard Yeh on 2017-10-13.
//
#include <vector>
#include <iostream>
#include <map>

int vecfault() {
    std::vector<int> is(10, 1);

//    is[100] = 20;

    std::cout << "[5]:" << is[5] << std::endl;
    std::cout << "[100]:" << is[100] << std::endl;
//    std::cout << "[200]:" << is[100] << std::endl;
//
//    std::cout << "[200]:" << is.at(200) << std::endl;

    int score = 0;
    for (int i = 0; i < 1000000; i++) {
        score += is[i];
    }

    std::cout << "size: " << is.size() << std::endl;
    std::cout << "score: " << score << std::endl;
//    std::cout << is[100] << std::endl;

//    for (auto it = is.begin(); it < is.end(); it++) {
//        auto i = *it;
//
//
//    }

    return 0;
}

int mapgrow() {
    std::map<int,int> is;

//    is[100] = 20;

    std::cout << "[5]:" << is[5] << std::endl;
    std::cout << "[100]:" << is[100] << std::endl;
//    std::cout << "[200]:" << is[100] << std::endl;
//
//    std::cout << "[200]:" << is.at(200) << std::endl;

    int score = 0;
    for (int i = 0; i < 1000; i++) {
        score += is[i];
    }

    std::cout << "size: " << is.size() << std::endl;
    std::cout << "score: " << score << std::endl;
//    std::cout << is[100] << std::endl;

//    for (auto it = is.begin(); it < is.end(); it++) {
//        auto i = *it;
//
//
//    }

    return 0;
}

int main() {
    mapgrow();
}