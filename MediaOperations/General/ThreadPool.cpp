#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    setThreads(6);
}

ThreadPool::~ThreadPool()
{
    abort = true;
    {
        std::lock_guard<std::mutex> l(set_value_mutex);
        run_condition.notify_all();
    }

    for (auto &&it : threads)
    {
        if (it->joinable())
        {
            it->join();
            delete it;
        }
    }
    threads.clear();
}

ThreadPool &ThreadPool::getObject()
{
    static ThreadPool obj;
    return obj;
}

void ThreadPool::setThreads(int count)
{
    for (int i = threads.size(); i < count; i++)
    {
        threads.push_back(new std::thread(ThreadWorker(this, i)));
    }
}

void ThreadPool::updateTaskQueue()
{
    std::lock_guard<std::mutex> l(set_value_mutex);

    for (auto task_id = task_wait_list.begin(); task_id != task_wait_list.end();)
    {
        TaskQueue &task_queue = taskqueue_list[*task_id];
        if (task_queue.task_need_count > (threads.size() - alloc_task))
        {
            task_id++;
            continue;
        }

        alloc_task += task_queue.task_need_count;
        task_run_list.insert(*task_id);
        task_id = task_wait_list.erase(task_id);
    }
    run_condition.notify_all();
    // std::cout << "updateTaskQueue " << std::this_thread::get_id() << ' ' << abort << std::endl;
}

#pragma GCC push_options
#pragma GCC optimize("00")
void ThreadPool::waitTask(int task_id)
{
    int sic = true;
    while (sic)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::lock_guard<std::mutex> l(set_value_mutex);
        sic = taskqueue_list.find(task_id) != taskqueue_list.end();
    }
}
#pragma GCC pop_options

int ThreadPool::getTaskID(int task_need)
{
    std::lock_guard<std::mutex> l(set_value_mutex);

    int task_id = alloc_task_id++;

    TaskQueue task_queue;
    task_queue.task_id = task_id;
    task_queue.task_need_count = task_need;

    task_wait_list.insert(task_id);
    taskqueue_list.insert(std::pair<int, TaskQueue>(task_id, task_queue));

    return task_id;
}

ThreadPool::ThreadWorker::ThreadWorker(ThreadPool *par, int id)
{
    parent = par;
    thread_id = id;
}

void ThreadPool::ThreadWorker::operator()()
{
    std::function<void()> func;
    while (parent->abort == false)
    {
        std::unique_lock<std::mutex> l(parent->set_value_mutex);
        int task_queue_index = -1;
        for (auto task_id = parent->task_run_list.begin(); task_id != parent->task_run_list.end(); task_id++)
        {
            TaskQueue &task_queue = parent->taskqueue_list[*task_id];
            if (task_queue.task_que.empty())
                continue;

            task_queue_index = *task_id;
            break;
        }
        if (task_queue_index == -1)
        {
            // std::cout << "previous " << std::this_thread::get_id() << ' ' << parent->abort << std::endl;
            parent->run_condition.wait(l);
            // std::cout << "next " << std::this_thread::get_id() << ' ' << parent->abort << std::endl;
            continue;
        }

        TaskQueue &task_queue = parent->taskqueue_list[task_queue_index];
        func = task_queue.task_que.front();
        task_queue.task_que.pop();
        l.unlock();

        func();

        l.lock();
        parent->alloc_task--;
        task_queue.task_finish_count++;
        if (task_queue.task_que.empty() && task_queue.task_need_count == task_queue.task_finish_count)
        {
            if (task_queue.task_release != NULL)
                task_queue.task_release();
            parent->task_run_list.erase(task_queue.task_id);
            parent->taskqueue_list.erase(task_queue.task_id);
        }

        l.unlock();
        parent->updateTaskQueue();
    }
}
