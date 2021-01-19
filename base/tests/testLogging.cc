#include "../Thread.h"
#include "../Log.h"
#include <iostream>
#include <unistd.h>
#include <vector>

void ThreadFunc(){
    //std::cout <<"doing:"<< pthread_self() << std::endl;
    for(int i = 0; i < 100; ++i){
        LOG << i;
    }
}
void type_test(){
    std::cout << "------------type-----------" << std::endl; 
    std::cout << "------------type_test--------------" << std::endl;
    LOG << 0;
    
    LOG << 123456789;
    
    LOG << 1.0f;
    
    LOG << 3.141592687456;
    
    LOG << (short) 1;
    
    LOG << (long long) 1;
    
    LOG << (unsigned int) 1;
    
    LOG << (long double) 1;
    
    LOG << (unsigned long long) 1;
   
    LOG << "C";
    
    LOG << "abcdefg";
    
    LOG << std::string("abhjjkll");
    

    
}

void four_thread(){
    std::vector<Thread::ptr> threads;
    //std::cout << pthread_self() << std::endl;
    
    for(int i = 0; i < 4; ++i){
        
        Thread::ptr tmp(new Thread(ThreadFunc, "testthread"));
        threads.push_back(tmp);
    }
    //sleep(10);
    for(int i = 0; i < 4; ++i){
        threads[i]->start();
    }
    
}

void one_thread(){
    std::cout << "-----------------one_thread-----------------"<<std::endl;
    for(int i = 0; i < 100; ++i){
        LOG << i;
    }
}

int main(){
    /*
    type_test();
    sleep(3);
    */
/*
    one_thread();
    sleep(3);
    
*/
/*
    std::cout << "-------------all-----------" << std::endl;
    LOG << "FFFFF" << 'C' << 3.66 << std::string("hhhh");*/


    four_thread();
    sleep(3);
    
    return 1;
}


