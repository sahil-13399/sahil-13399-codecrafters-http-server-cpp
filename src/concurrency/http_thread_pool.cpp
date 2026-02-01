#include "http_thread_pool.hpp"


void HttpThreadPool::enqueue(std::function<void()> func) {
    {
        std::unique_lock<std::mutex> lock(mutex);
        worker_queue.push(func);
    }
    cv.notify_one();
}

void HttpThreadPool::dequeue() {
    while(true) {
        std::function<void()> func;
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]{
                return !worker_queue.empty() || shutdown_flag;
            });
            
            if(worker_queue.empty()) {
                if(shutdown_flag) {
                    return;
                }
                continue;
            }
            
            func = worker_queue.front();
            worker_queue.pop();
        }
        
        func();
    }
}

void HttpThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        shutdown_flag = true;
    }
    cv.notify_all();
    
    for(auto& t : thread_pool) {
        if(t.joinable()) {
            t.join();
        }
    }
}