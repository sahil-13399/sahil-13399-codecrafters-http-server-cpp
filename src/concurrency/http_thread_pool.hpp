#pragma once
#include<string>
#include<vector>
#include<thread>
#include <iostream>
#include <queue>
#include <functional>
#include<mutex>
#include<condition_variable>

/*
    Aim is to build a ThreadPool for handling HTTP requests
    So we need pool of threads, mutex(lock), condition_variable(to signal a thread)


*/
class HttpThreadPool{

    private:
        int max_worker_threads;
        std::vector<std::thread> thread_pool;
        std::queue<std::function<void()>> worker_queue;
        std::mutex mutex;
        std::condition_variable cv;
        bool shutdown_flag;

    public:
        
        HttpThreadPool(int n) : max_worker_threads(n) {
            shutdown_flag = false;
            for(int i = 0; i < n; i++) {
                thread_pool.push_back(std::thread(&HttpThreadPool::dequeue, this));
            }
        }

        void enqueue(std::function<void()> func);
        void dequeue();
        void shutdown();
        ~HttpThreadPool() {
            shutdown();
        }
};