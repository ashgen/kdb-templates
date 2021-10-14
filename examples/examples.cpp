//
// Created by ashish on 10/3/21.
//

#define KXVER 3
#include "k.h"
#include "markrooney/kdb.hpp"

using namespace kdb;

#include <unordered_map>
#include <random>
#include <thread>

static constexpr int N = 1e6;
struct test{
    std::vector<bool> cp;
    std::vector<int> strike;
    std::vector<double> iv;
};
KDB_REGISTER(test,cp,strike,iv);
struct simpleDict {
    int cnt;
    std::string sym;
    double fairsynp;
    test vol;
};
KDB_REGISTER(simpleDict,cnt,sym,fairsynp,vol);
struct simpleTest {
    int cnt;
    std::string sym;
    double fairsynp;
};
KDB_REGISTER(simpleTest,cnt,sym,fairsynp);



int main(){
    auto t= khp("localhost",5052);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(1.0, 10.0);
    auto tableName=ks((S) "ivolt");
    for(int i = 0;i<N;i++){
        simpleDict exp = {i,"NIFTY",dist(mt),{{true,false},{10*i,10*i+1},{dist(mt),dist(mt)}}};
        //simpleTest exp = {i,"NIFTY",dist(mt)};
        auto x = convert::from_native(exp);
        k(-t,"insert",r1(tableName),x,K(0));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return 0;
}