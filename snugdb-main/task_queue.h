#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include "task.h"

class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();

    void add_task(const Task& task);
    void start();
    void stop();

private:
    void process_tasks();

    std::deque<Task> tasks_;
    std::mutex tasks_mutex_;
    std::condition_variable tasks_condition_;
    std::thread worker_thread_;
    bool running_;
};