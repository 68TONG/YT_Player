#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <map>
#include <set>
#include <queue>
#include <mutex>
#include <future>
#include <thread>
#include <functional>
#include <condition_variable>

class ThreadPool
{
protected:
    class ThreadWorker
    {
    public:
        ThreadWorker(ThreadPool *par, int id);

        void operator()();

        int thread_id = -1;
        ThreadPool *parent = NULL;
    };

    class TaskQueue
    {
    public:
        int task_id = 0;
        int task_add_count = 0;
        int task_need_count = 0;
        int task_finish_count = 0;
        std::function<void()> task_release = nullptr;
        std::queue<std::function<void()>> task_que;
    };

    bool abort = false;
    std::vector<std::thread *> threads;

    int alloc_task = 0;
    int64_t alloc_task_id = 0;
    std::mutex set_value_mutex;
    std::condition_variable run_condition;
    std::set<int> task_run_list;
    std::set<int> task_wait_list;
    std::map<int, TaskQueue> taskqueue_list;

public:
    ThreadPool();
    ~ThreadPool();
    static ThreadPool &getObject();

    void setThreads(int count);

    void updateTaskQueue();

    void waitTask(int task_id);

    int getTaskID(int task_need = 1);

    template <typename F, typename... Args>
    auto addTask(int task_id, F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        if (taskqueue_list.find(task_id) == taskqueue_list.end())
        {
            throw std::out_of_range("addTask of task_id error");
        }
        if (taskqueue_list[task_id].task_need_count == taskqueue_list[task_id].task_add_count)
        {
            throw std::out_of_range("addTask of task_need error");
        }

        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> worker_task = [task_ptr]()
        {
            (*task_ptr)();
        };

        std::unique_lock<std::mutex> l(set_value_mutex);

        taskqueue_list[task_id].task_add_count++;
        taskqueue_list[task_id].task_que.push(worker_task);

        l.unlock();
        updateTaskQueue();
        return task_ptr->get_future();
    }

    template <typename F, typename... Args>
    auto addTask_Release(int task_id, F &&f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        if (taskqueue_list.find(task_id) == taskqueue_list.end())
        {
            throw std::out_of_range("addTask of task_id error");
        }

        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        std::function<void()> worker_task = [task_ptr]()
        {
            (*task_ptr)();
        };

        std::unique_lock<std::mutex> l(set_value_mutex);

        taskqueue_list[task_id].task_release = worker_task;

        return task_ptr->get_future();
    }
};

#endif // THREADPOOL_H
