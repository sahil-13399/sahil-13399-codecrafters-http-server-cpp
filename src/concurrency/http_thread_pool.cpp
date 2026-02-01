#include "http_thread_pool.hpp"


void HttpThreadPool::enqueue(std::function<void()> func) {
    //Add the function to queue and notify
    std::unique_lock<std::mutex> lock(mutex);
    worker_queue.push(func);
    cv.notify_one();
}


void HttpThreadPool::dequeue() {
    while(true) {
        std::function<void()> func;
        //Waits for new request to be added
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]{
                return !worker_queue.empty() || shutdown_flag;
            });
            func = worker_queue.front();
            worker_queue.pop();
        }
        func();
    }
}


void HttpThreadPool::shutdown() {
    std::unique_lock<std::mutex> lock(mutex);
    shutdown_flag = true;
    cv.notify_all();
    for(auto& t : thread_pool) {
        if(t.joinable()) {
            t.join();
        }
    }
}