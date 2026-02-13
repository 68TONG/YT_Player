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

template <typename T>
class MutexQueue
{
public:
    MutexQueue() {}

    int push(T &val)
    {
        std::lock_guard<std::mutex> l(mutex_);
        que.push(val);
        variable.notify_one();
        return 0;
    }

    int Front(T &val)
    {
        std::lock_guard<std::mutex> l(mutex_);
        if (que.empty())
            return -1;
        val = que.front();
        return 0;
    }

    int pop(T &val, int time = 0)
    {
        std::unique_lock<std::mutex> l(mutex_);
        if (que.empty())
        {
            if (time == 0)
            {
                variable.wait(l);
            }
            else
            {
                variable.wait_for(l, std::chrono::milliseconds(time), []() -> int
                                  { return -1; });
            }
        }
        if (que.empty())
            return -1;
        val = que.front();
        que.pop();
        return 0;
    }

    int pop(std::queue<T> &val_que)
    {
        std::unique_lock<std::mutex> l(mutex_);
        if (que.empty())
            return -1;

        for (; que.empty() == false; que.pop())
        {
            val_que.push(que.front());
        }

        return 0;
    }

    int size()
    {
        std::lock_guard<std::mutex> l(mutex_);
        return que.size();
    }

private:
    std::queue<T> que;
    std::mutex mutex_;
    std::condition_variable variable;
};

#endif // DECODECQUE_H
