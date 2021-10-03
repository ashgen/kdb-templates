//
// Created by ashish on 10/3/21.
//

#define KXVER 3
#include "k.h"
#include "markrooney/kdb.hpp"

using namespace kdb;

#include <unordered_map>
#include <random>
static constexpr int N = 1e6;
struct simpleDict {
    int time;
    std::string sym;
    std::vector<float> iv;
};

KDB_REGISTER(simpleDict,time,sym,iv);


int main(){
    auto t= khp("localhost",5052);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(1.0, 10.0);
    for(int i = 0;i<N;i++){
        auto exp=simpleDict{i,"NIFTY",{dist(mt),dist(mt)}};
        auto x = convert::from_native(exp);
        k(-t,"`ivol insert",x,K(0));
    }
    return 0;
}