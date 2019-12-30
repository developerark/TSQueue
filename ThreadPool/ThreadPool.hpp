//
//  ThreadPool.hpp
//  TSQueue
//
//  Created by Aswin Raj Kharel on 12/29/19.
//  Copyright © 2019 Aswin Raj Kharel. All rights reserved.
//

#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include <stdio.h>
#include <vector>
#include <thread>
#include "TSQueue.hpp"
#include "ThreadJoiner.hpp"

class ThreadPool{
private:
    std::atomic<bool> _done;
    std::atomic<bool> _waitToEmpty;
    
    TSQueue<std::function<void()>> _workQueue;
    std::vector<std::thread> _threads;
    ThreadJoiner _threadJoiner;
    
    bool _shouldRun(){
        /*
        // Done         // _waitToEmpty         // _empty           // shouldRun
            T                   T                   T                   F
            T                   T                   F                   T
            T                   F                   T                   F
            T                   F                   F                   F
            F                   T                   T                   T
            F                   T                   F                   T
            F                   F                   T                   T
            F                   F                   F                   T
        */
        
        if (!this->_done){
            return true;
        }else{
            if (!this->_waitToEmpty){
                return false;
            }else{
                if(this->_workQueue.empty()){
                    return false;
                }
                return true;
            }
        }
    }
    
    void _startWorker(){
        while(this->_shouldRun()){
            std::function<void()> work;
            if (this->_workQueue.tryAndPop(work)){
                work();
            }else{
                std::this_thread::yield();
            }
        }
    }
    
public:
    ThreadPool():
        _done(false), _threadJoiner(_threads), _waitToEmpty(false){
            unsigned const maxThreads = std::thread::hardware_concurrency();
            try{
                for (unsigned int i = 0; i < maxThreads; i++){
                    this->_threads.push_back(std::thread(&ThreadPool::_startWorker, this));
                }
            }catch(...){
                this->_done = true;
                throw;
            }
    }
    
    ~ThreadPool(){
        this->_done = true;
    }
    
    
    void submit(std::function<void()> work){
        this->_workQueue.push(work);
    }
    
    void setDone(bool value){
        this->_done = value;
    }
    
    void setWaitToEmpty(bool value){
        this->_waitToEmpty = value;
    }
};

#endif /* ThreadPool_hpp */
