#pragma once
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <queue>
#include <string>

class threadpool {
    public:
        threadpool(int numThrd) 
        : numThreads(numThrd) {
            for (int i=0; i<numThrd; ++i) {
                std::thread* th = new std::thread(&threadpool::workFunction, this);
                threads.push_back(th);
            }
        }
        void putWork(std::string);
        std::string getWork();
        void workFunction();
        void join();
    private:
        int numThreads;
        std::mutex mutexL_;
        std::condition_variable condition_;
        std::queue<std::string> workQ_;
        std::vector<std::thread*> threads;
};

void threadpool::workFunction () {
    while(1) {   //Loop function for each thread
		std::string work = getWork();
        cout << "Got the work: " << work << std::endl;
        //static_cast<CallData*>(work)->Proceed();
    }
}

void threadpool::join () {
    for(int i=0; i<numThreads; ++i) {
        threads[i]->join();
    }
}

void threadpool::putWork(std::string work) {
  std::unique_lock<std::mutex> lck(mutexL_);
  workQ_.push(work);
  condition_.notify_all();
}

std::string threadpool::getWork() {
    std::string ret;
    std::unique_lock<std::mutex> lck( mutexL_,std::defer_lock);
    lck.lock();
    while (workQ_.empty()) condition_.wait(lck);
    ret = workQ_.front();
    workQ_.pop();
    lck.unlock();
    return ret;
}


