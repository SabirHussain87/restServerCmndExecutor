#pragma once
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <queue>
#include <string>

class threadpool {
    public:
        threadpool(int numThrd, std::vector<std::string> vendorAddresses) 
        : numThreads(numThrd), vendorAddresses_(vendorAddresses) {
            for (int i=0; i<numThrd; ++i) {
                std::thread* th = new std::thread(&threadpool::workFunction, this);
                threads.push_back(th);
            }
        }
        void putWork(void *);
        void* getWork();
        void workFunction();
        void join();
    private:
        int numThreads;
        std::mutex mutexL_;
        std::condition_variable condition_;
        std::queue<void*> workQ_;
        std::vector<std::thread*> threads;
        std::vector<std::string> vendorAddresses_;
};

void threadpool::workFunction () {
    while(1) {   //Loop function for each thread
		void* work = getWork();
        static_cast<CallData*>(work)->Proceed();
    }
}

void threadpool::join () {
    for(int i=0; i<numThreads; ++i) {
        threads[i]->join();
    }
}

void threadpool::putWork(void * work) {
  std::unique_lock<std::mutex> lck(mutexL_);
  workQ_.push(work);
  condition_.notify_all();
}

void* threadpool::getWork() {
    void* ret;
    std::unique_lock<std::mutex> lck( mutexL_,std::defer_lock);
    lck.lock();
    while (workQ_.empty()) condition_.wait(lck);
    ret = workQ_.front();
    workQ_.pop();
    lck.unlock();
    return ret;
}


