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
    TSQueue<std::function<void()>> _workQueue;
    std::vector<std::thread> _threads;
    ThreadJoiner _threadJoiner;
    
    void _startWorker(){
        while(!this->_done){
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
        _done(false), _threadJoiner(_threads){
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
    
    template <typename T>
    void submit(T work){
        this->_workQueue.push(work);
    }
};

#endif /* ThreadPool_hpp */
