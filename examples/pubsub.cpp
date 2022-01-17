//
// Created by ashish on 1/15/22.
//
/* File name: subscriber.c */
#define KXVER 3
#include "time.h"
#include "k.h"
#include "kdb.hpp"
#include <iostream>
#include <thread>
#include <atomic>

void printTime(J t) {
    time_t timval= t / 1000000000;
    struct tm *timeInfo= localtime(&timval);

    printf("%dD%02d:%02d:%02d.%09lld ",
           timeInfo->tm_yday,
           timeInfo->tm_hour,
           timeInfo->tm_min,
           timeInfo->tm_sec,
           t % 1000000000);
}

I handleOk(I handle)
{
    if(handle > 0)
        return 1;
    if(handle == 0)
        std::cout<<"Authentication error \n";
    else if(handle == -1)
        std::cout<<"Connection error\n";
    else if(handle == -2)
        std::cout<<"Timeout error \n";
    return 0;
}

I isRemoteErr(K x) {
    if(!x) {
        std::cout<<"Connection error \n";
        return 1;
    } else if(-128 == x->t) {
        std::cout<<"Error message returned"<< x->s;
        r0(x);
        return 1;
    }
    return 0;
}
struct _k{
    int a;
    int b;
};
struct testKDB{
    kdb::type::list_long timestamp;
    kdb::type::list_long sym;
    kdb::type::list_float capacity;
    kdb::type::list_int rate;
};
KDB_REGISTER(testKDB,timestamp,sym,capacity,rate)

struct testKDBD{
    testKDBD() {

    }

    testKDBD(const testKDBD &kdbd) {
        sym=kdbd.sym;
        capacity=kdbd.capacity;
        rate=kdbd.rate;
    }

    kdb::type::atom_long sym;
    kdb::type::atom_float capacity;
    kdb::type::atom_int rate;
};
KDB_REGISTER(testKDBD,sym,capacity,rate)

testKDB _Test;
testKDBD _TestD;
std::atomic<bool> valid=false;
void testT(I handle){
    K response, table, tableName, columnNames, columnValues;
    while(1) {
        response= k(handle, (S) 0);
        if(!response)
            break;
        table= kK(response)[2]->k;
        columnValues=kK(table)[1];
        columnNames= kK(table)[0];
        kdb::type::list_symbol tk;
        kdb::convert::to_native(columnNames,tk);
        kdb::convert::to_native(columnValues,_Test);
        r0(response);
        valid=true;
    }

}
void testTD(I handle){
    K response, table, tableName, columnNames, columnValues;
    while(1) {
        response= k(handle, (S) 0);
        if(!response)
            break;
        auto start = std::chrono::high_resolution_clock::now();
        table= kK(response)[2];
        columnValues=kK(table)[1];
        columnNames= kK(table)[0];
        kdb::convert::to_native(columnValues,_TestD);
        r0(response);
        valid=true;
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_seconds=std::chrono::duration_cast<std::chrono::nanoseconds>(
                end - start);
        std::cout<<__func__<<","<<elapsed_seconds.count()<<std::endl;
    }

}
void stick_this_thread_to_core(std::thread& _thread,int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    int rc=pthread_setaffinity_np(_thread.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
}
void printTest(){
    while(1){
        auto t=valid.load();
        if(!t)
            continue;
        std::cout<<_Test.sym[0]<<","<<_Test.rate[0]<<std::endl;

    }
}
void printTestD(){
    while(1){
        auto t=valid.load();
        if(!t)
            continue;
        std::cout<<_TestD.sym<<","<<_TestD.rate<<","<<_TestD.capacity<<std::endl;

    }
}
int main() {
    J i;
    I handle;
    I portnumber= 6812;
    S hostname= "localhost";
    S usernamePassword= "kdb:pass";
    K response, table, tableName, columnNames, columnValues;

    handle= khp(hostname, portnumber);
    if(!handleOk(handle))
        return EXIT_FAILURE;
    tableName= ks("grid");
    std::vector<int> t={1,2,3};
    auto kdb_list = kdb::convert::from_native(t);
    //response= k(handle, ".u.sub", r1(tableName),kdb_list, (K)0);
    response= k(handle, ".u.sub", r1(tableName),ks(""), (K)0);
    if(isRemoteErr(response)) {
        r0(tableName);
        kclose(handle);
        return EXIT_FAILURE;
    }
    std::thread test(testTD,handle);
    std::thread testx(printTestD);
    stick_this_thread_to_core(test,1);
    stick_this_thread_to_core(testx,2);
    test.join();
    testx.join();
    r0(response);
    r0(tableName);
    kclose(handle);
    return EXIT_SUCCESS;
}

