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
    std::array<bool,2> cp;
    std::array<int,2> strike;
    std::array<double,2> iv;
};
KDB_REGISTER(simpleDict,cnt,sym,fairsynp,cp,strike,iv);
struct simpleTest {
    int cnt;
    std::string sym;
    double fairsynp;
};
KDB_REGISTER(simpleTest,cnt,sym,fairsynp);

struct data {
    std::vector<double> oiv;
    std::vector<double> om;
    std::vector<double> oweight;

};
KDB_REGISTER(data,oiv,om,oweight);

void testInsert(){
    auto t= khp("localhost",6055);
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(1.0, 10.0);
    auto tableName=ks((S) "ivol");
    for(int i = 0;i<N;i++){
        simpleDict exp = {i,"NIFTY",dist(mt),{true,false},{10*i,10*i+1},{dist(mt),dist(mt)}};
        //simpleTest exp = {i,"NIFTY",dist(mt)};
        auto x = convert::from_native(exp);
        k(-t,".u.upd",r1(tableName),x,K(0));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

const data& testSelect(){
    auto t= khp("localhost",6055);
    K _data=k(t,"exec from 1#select iv,m,weight from curve where date=min date,minute > 09:20 ",(K)0);
    K rows = kK(_data)[1];
    data test;
    convert::to_native(rows,test);
    return std::move(test);
}


int main(){
    testInsert();
    //testSelect();
    return 0;
}