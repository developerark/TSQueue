//
//  main.cpp
//  TSQueue
//
//  Created by Aswin Raj Kharel on 12/26/19.
//  Copyright © 2019 Aswin Raj Kharel. All rights reserved.
//

#include <iostream>
#include "TSQueue.hpp"
#include <string>
#include <thread>
class Item{
public:
    int id;
    std::string name;
};

std::atomic<bool> running(true);

void consumer(TSQueue<Item>& queue){
    while (running){
        std::shared_ptr<Item> item = queue.waitForAndPop(std::chrono::seconds(10));
        if (item == nullptr){
            std::cout << "Timed out, contunuing..." << std::endl;
            continue;
        }
        // Got an Item
        std::cout << "Got an item: " << item.get()->id << " " << item.get()->name;
    }
}

int main(int argc, const char * argv[]) {
    TSQueue<Item> queue;
    std::thread consumerThread(consumer, std::ref(queue));
    
    // get input loop
    int count = 0;
    while (true){
        char input = std::getchar();
        if (input == 'q'){
            std::cout << "Received q" << std::endl;
            running.exchange(false);
            break;
        }
        if (input == 'i'){
            Item item;
            item.id = count++;
            item.name = "Pen";
            queue.push(item);
        }
        if (input == 'b'){
            for (int i = 0; i < 1000; i++){
                Item item;
                item.id = count++;
                item.name = "Pen";
                queue.push(item);
            }
        }
    }
    //
    
    consumerThread.join();
    std::cout << "Ending program..." << std::endl;
    
    return 0;
}
