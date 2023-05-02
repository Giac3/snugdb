#include "task_queue.h"
#include "file_utils.h"
#include <iostream>

TaskQueue::TaskQueue() : running_(false) {}

TaskQueue::~TaskQueue() {
    stop();
}

void TaskQueue::add_task(const Task& task) {
    std::unique_lock<std::mutex> lock(tasks_mutex_);
    tasks_.push_back(task);
    tasks_condition_.notify_one();
}

void TaskQueue::start() {
    running_ = true;
    worker_thread_ = std::thread(&TaskQueue::process_tasks, this);
}

void TaskQueue::stop() {
    if (running_) {
        running_ = false;
        tasks_condition_.notify_one();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }
}

void TaskQueue::process_tasks() {
    while (running_) {
        std::unique_ptr<Task> task_to_process;
        {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            tasks_condition_.wait(lock, [&]() { return !running_ || !tasks_.empty(); });

            if (!running_) {
                break;
            }

            task_to_process = std::make_unique<Task>(tasks_.front());
            tasks_.pop_front();
        }

        // Task execution
        if (task_to_process->operation() == Task::Operation::CreateDatabase) {
            // Perform disk operations for creating a database
            std::string db_path = "snugdb_data/" + task_to_process->database_name();
            FileUtils::create_directory_if_not_exists(db_path);
        }
        
    }
}