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

/**
 *@brief This class provides a thread safe implementation of a queue. It uses std::queue<T> as an internal queue data structure.
 *       Along with that it makes use of the C++ standard threading library to protect the queue and provide synchronization among multiple threads.
 */

template <typename T>
class TSQueue{
private:
    mutable std::mutex _mu;
    std::queue<T> _queue;
    std::condition_variable _queueCondition;
    
public:
    /**
     *@brief Constructor of the TSQueue class.
     */
    TSQueue(){}
    
    /**
     *@brief Copy assignment
     */
    TSQueue& operator=(const TSQueue&) = delete;
    
    /**
     *@brief Copy constructor of the TSQueue class. This enables an instance of this class to be passed around as value.
     *
     *@param other The instance to be copied from
     */
    TSQueue(TSQueue const& other){
        std::lock_guard<std::mutex> lock(other->_mu);
        this->_queue = other._queue;
    }
    
    /**
     *@brief Pushes a value to the queue
     *
     *@param value The item to be pushed to the queue
     */
    void push(T value){
        {
            std::lock_guard<std::mutex> lock(this->_mu);
            this->_queue.push(value);
        }
        this->_queueCondition.notify_one();
    }
    
    /**
     *@brief Checks if the queue is empty
     *
     *@return true if the queue is empty, else false
     */
    bool empty() const{
        std::lock_guard<std::mutex> lock(this->_mu);
        return this->_queue.empty();
    }
    
    /**
     *@brief Tries to pop a value from the queue and copy it to the value passed in as an argument.
     *This is a non blocking call and returns immediately with a status of retrieval.
     *
     *@param value An instance to which the popped item is to be copied to.
     *
     *@return true if there exists an item to be popped, else false.
     */
    bool tryAndPop(T& value){
        std::lock_guard<std::mutex> lock(this->_mu);
        if (this->_queue.empty())
            return false;
        value = this->_queue.front();
        this->_queue.pop();
        return true;
    }
    
    /**
     *@brief Tries to pop an item fron the queue and returns a std::shared_ptr<T>
     *This is a non blocking call and returns immediately with a status of retrieval.
     *
     *@return nullptr if there is no item to be popped, else returns a shared_ptr pointing to the popped item.
     */
    std::shared_ptr<T> tryAndPop(){
        std::lock_guard<std::mutex> lock(this->_mu);
        if (this->_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> value = std::make_shared<T>(this->_queue.front());
        this->_queue.pop();
        return value;
    }
    
    /**
     *@brief Waits and pops an item from the queue and copies it to value passed in as an argument
     *This is a blocking call and it waits until an item is available to be popped.
     *
     *@param value An intance to which the popped item is to be copied to.
     */
    void waitAndPop(T& value){
        std::unique_lock<std::mutex> lock(this->_mu);
        this->_queueCondition.wait(lock, [this]{return !_queue.empty();});
        value = this->_queue.front();
        this->_queue.pop();
    }
    
    /**
     *@brief Waits and pops an item from the queue and returns a shared_ptr<T>
     *This is a blocking call and it waits until an item is available to be popped.
     *
     *@return returns a std::shared_ptr<T> pointing to an instance of the popped item
     */
    std::shared_ptr<T> waitAndPop(){
        std::unique_lock<std::mutex> lock(this->_mu);
        this->_queueCondition.wait(lock, [this]{return !_queue.empty();});
        std::shared_ptr<T> value = std::make_shared<T>(this->_queue.front());
        this->_queue.pop();
        return value;
    }
    
    /**
     *@brief Waits and pops an item from the queue and returns a copy of the item
     *This is a blocking call and it waits until an item is available to be popped.
     *
     *@return returns a copy of the instance of the popped item.
     */
    T waitAndPopValue(){
        std::unique_lock<std::mutex> lock(this->_mu);
        this->_queueCondition.wait(lock, [this]{return !_queue.empty();});
        T value = this->_queue.front();
        this->_queue.pop();
        return value;
    }
    
    /**
     *@brief Waits and pops an item from the queue and copies it to value passed in as an argument.
     *This is a blocking call with a timeout functionality.
     *
     *@param value An instance to which the popped item is to be copied to.
     *@param waitDuration Wait for atmost waitDuration.
     *@return returns false if the wait timedout, returns true if an item was poped and copied.
     */
    bool waitForAndPop(T& value, std::chrono::milliseconds waitDuration){
        std::unique_lock<std::mutex> lock(this->_mu);
        if (this->_queueCondition.wait_for(lock, waitDuration, [this]{return !_queue.empty();})){
            // Got something
            value = this->_queue.front();
            this->_queue.pop();
            return true;
        }
        // Timed out
        return false;
    }
     
    /**
     *@brief Waits and pops an item from the queue and returns a std::shared_ptr<T>.
     *This is a blocking call with a timeout functionality.
     *
     *@param waitDuration Wait for atmost waitDuration
     *@return returns nullptr if it timed out, else returns a std::shared_ptr<T> pointing to an instance of item popped.
     */
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
