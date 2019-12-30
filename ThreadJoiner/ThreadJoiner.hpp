//
//  ThreadJoiner.hpp
//  TSQueue
//
//  Created by Aswin Raj Kharel on 12/29/19.
//  Copyright © 2019 Aswin Raj Kharel. All rights reserved.
//

#ifndef ThreadJoiner_hpp
#define ThreadJoiner_hpp

#include <stdio.h>
#include <stdio.h>
#include <vector>
#include <thread>
class ThreadJoiner{
private:
    std::vector<std::thread>& _threads;

public:
    explicit ThreadJoiner(std::vector<std::thread> &threads) : _threads(threads){}
    ~ThreadJoiner(){
        for (int i = 0; i < this->_threads.size(); i++){
            if (this->_threads[i].joinable()){
                this->_threads[i].join();
            }
        }
    }
};
#endif /* ThreadJoiner_hpp */
