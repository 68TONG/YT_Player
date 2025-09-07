#ifndef DECODECQUE_H
#define DECODECQUE_H

#include <map>
#include <set>
#include <queue>
#include <vector>

#include <mutex>
#include <atomic>
#include <condition_variable>

#include <thread>
#include <iostream>

template<typename T>
class MutexQueue{
public:

    MutexQueue()
    {

    }

    int push(T &val)
    {
        std::lock_guard<std::mutex> l(mutex_);
        if(abort) return -1;
        que.push(val);
        variable.notify_one();
        return 0;
    }

    int Front(T &val)
    {
        std::lock_guard<std::mutex> l(mutex_);
        if(que.empty()) return -1;
        val = que.front();
        return 0;
    }

    int pop(T &val, int time = 0)
    {
        std::unique_lock<std::mutex> l(mutex_);
        if(que.empty()){
            if(time == 0){
                variable.wait(l);
            } else{
                variable.wait_for(l, std::chrono::milliseconds(time), []()->int{
                                      return -1;
                });
            }
        }
        if(que.empty()) return -1;
        val = que.front();
        que.pop();
        return 0;
    }

    int pop(std::queue<T> &val_que)
    {
        std::unique_lock<std::mutex> l(mutex_);
        if(que.empty()) return -1;

        for(;que.empty() == false;que.pop()){
            val_que.push(que.front());
        }

        return 0;
    }

    int size()
    {
        std::lock_guard<std::mutex> l(mutex_);
        return que.size();
    }

    bool isAbort()
    {
        return abort;
    }

    void Abort(bool is)
    {
        std::lock_guard<std::mutex> l(mutex_);
        abort = is;
        variable.notify_all();
    }

private:
    bool abort = false;
    std::queue<T> que;
    std::mutex mutex_;
    std::condition_variable variable;
};

// template<typename T>
// class Vector{
// public:

//     int push_back(T &val){
//         std::lock_guard<std::mutex> l(mutex_);
//         if(abort) return -1;
//         vec.push_back(val);
//         variable.notify_one();
//         return 0;
//     }

//     int pop_back(){
//         std::unique_lock<std::mutex> l(mutex_);
//         if(vec.empty()) return -1;
//         vec.pop_back();
//         return 0;
//     }

//     int erase(T &val){
//         std::unique_lock<std::mutex> l(mutex_);
//         for(auto it = vec.begin();it != vec.end();it++){
//             if(val == (*it)){
//                 vec.erase(it);
//                 return 0;
//             }
//         }
//         return -1;
//     }

//     int clear(){
//         std::lock_guard<std::mutex> l(mutex_);
//         if(vec.empty() == false) vec.clear();
//         return 0;
//     }

//     int size(){
//         std::lock_guard<std::mutex> l(mutex_);
//         return vec.size();
//     }

//     bool isAbort(){
//         return abort;
//     }

//     void Abort(bool is){
//         std::lock_guard<std::mutex> l(mutex_);
//         abort = is;
//         variable.notify_all();
//     }

//     T& operator[](int index){
//         std::unique_lock<std::mutex> l(mutex_);
//         if(vec.empty() || index >= vec.size() || index < 0){
//             throw std::out_of_range("Index out of range");
//         }
//         return vec[index];
//     }

// private:
//     bool abort = false;
//     std::vector<T> vec;
//     std::mutex mutex_;
//     std::condition_variable variable;
// };

#endif // DECODECQUE_H
