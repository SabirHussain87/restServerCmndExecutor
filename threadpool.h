#pragma once
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <queue>
#include <string>
#include <vector>

#define MAX_WORK 500
struct work {
    std::string task;
    int id;
};

class threadpool {
    public:
        threadpool(int numThrd) 
        : numThreads(numThrd) {
            for (int i=0; i<numThrd; ++i) {
                std::thread* th = new std::thread(&threadpool::workFunction, this);
                threads.push_back(th);
            }
            runningWorkId=0;
            for(int i=0; i < MAX_WORK; ++i) commandOutput.push_back("");
        }
        int putWork(std::string);
        work getWork();
        void workFunction();
        void join();
        std::string exec(std::string cmd);
        std::string getOutput(int id) {
            return commandOutput[id];
        }
    private:
        int numThreads;
        std::mutex mutexL_;
        std::condition_variable condition_;
        //std::queue<std::string> workQ_;
        std::queue<work> workQ_;
        std::vector<std::thread*> threads;
        std::vector<string> commandOutput;
        int runningWorkId;
};

void threadpool::workFunction () {
    while(1) {   //Loop function for each thread
		struct work wrk;
        wrk = getWork();
        //cout << "Got the work: " << wrk.task << std::endl;
        std::string res = exec(wrk.task);
        //cout << "Result:" << res << endl; 
        commandOutput[wrk.id] = res;
    }
}

std::string threadpool::exec(std::string cmd) {
    std::array<char, 512> buf;
    std::string res;
    std::unique_ptr<FILE, decltype(&pclose)> exepipe(popen(cmd.c_str(), "r"), pclose);
    if (!exepipe) {
        std::cout<< "popen() failed!: Errno" << errno << std::endl ;
    }
    while (fgets(buf.data(), buf.size(), exepipe.get()) != nullptr) {
        res += buf.data();
    }
    return res;
}

void threadpool::join () {
    for(int i=0; i<numThreads; ++i) {
        threads[i]->join();
    }
}

//Returns uniq id for every work put in the queue
int threadpool::putWork(std::string task) {
  struct work wrk;
  wrk.task = task;
  std::unique_lock<std::mutex> lck(mutexL_);
  wrk.id = ++runningWorkId;
  if(runningWorkId == 500) runningWorkId=0;
  workQ_.push(wrk);
  condition_.notify_all();
  return wrk.id;
}

work threadpool::getWork() {
    struct work ret;
    std::unique_lock<std::mutex> lck( mutexL_,std::defer_lock);
    lck.lock();
    while (workQ_.empty()) condition_.wait(lck);
    ret = workQ_.front();
    workQ_.pop();
    lck.unlock();
    return ret;
}


