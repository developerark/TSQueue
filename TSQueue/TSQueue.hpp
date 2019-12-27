//
//  TSQueue.hpp
//  TSQueue
//
//  Created by Aswin Raj Kharel on 12/26/19.
//  Copyright © 2019 Aswin Raj Kharel. All rights reserved.
//

#ifndef TSQueue_hpp
#define TSQueue_hpp

#include <stdio.h>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
class TSQueue{
private:
    mutable std::mutex _mu;
    std::queue<T> _queue;
    std::condition_variable _queueCondition;
    
public:
    TSQueue(){}
    
    TSQueue(TSQueue const& other){
        std::lock_guard<std::mutex> lock(this->_mu);
        this->_queue = other._queue;
    }
    
    void push(T value){
        {
            std::lock_guard<std::mutex> lock(this->_mu);
            this->_queue.push(value);
        }
        this->_queueCondition.notify_one();
    }
    
    bool empty() const{
        std::lock_guard<std::mutex> lock(this->_mu);
        return this->_queue.empty();
    }
    
    bool tryAndPop(T& value){
        std::lock_guard<std::mutex> lock(this->_mu);
        if (this->_queue.empty())
            return false;
        value = this->_queue.front();
        this->_queue.pop();
        return true;
    }
    
    std::shared_ptr<T> tryAndPop(){
        std::lock_guard<std::mutex> lock(this->_mu);
        if (this->_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> value = std::make_shared<T>(this->_queue.front());
        this->_queue.pop();
        return value;
    }
    
    void waitAndPop(T& value){
        std::unique_lock<std::mutex> lock(this->_mu);
        this->_queueCondition.wait(lock, [this]{return !_queue.empty();});
        value = this->_queue.front();
        this->_queue.pop();
    }
    
    std::shared_ptr<T> waitAndPop(){
        std::unique_lock<std::mutex> lock(this->_mu);
        this->_queueCondition.wait(lock, [this]{return !_queue.empty();});
        std::shared_ptr<T> value = std::make_shared<T>(this->_queue.front());
        this->_queue.pop();
        return value;
    }
    
    // *
    T waitAndPopValue(){
        std::unique_lock<std::mutex> lock(this->_mu);
        this->_queueCondition.wait(lock, [this]{return !_queue.empty();});
        T value = this->_queue.front();
        this->_queue.pop();
        return value;
    }
    
    // *
    bool waitForAndPop(T& value, std::chrono::milliseconds waitDuration){
        std::unique_lock<std::mutex> lock(this->_mu);
        if (this->_queueCondition.wait_for(lock, waitDuration, [this]{return !_queue.empty();})){
            // Got something
            value = this->_queueCondition.front();
            this->_queueCondition.pop();
            return true;
        }
        // Timed out
        return false;
    }
    
    // *
    std::shared_ptr<T> waitForAndPop(std::chrono::milliseconds waitDuration){
        std::unique_lock<std::mutex> lock(this->_mu);
        if (this->_queueCondition.wait_for(lock, waitDuration, [this]{return !_queue.empty();})){
            // Got something
            std::shared_ptr<T> value = std::make_shared<T>(this->_queue.front());
            this->_queue.pop();
            return value;
        }
        // Timed out
        return std::shared_ptr<T>();
    }
};



#endif /* TSQueue_hpp */
