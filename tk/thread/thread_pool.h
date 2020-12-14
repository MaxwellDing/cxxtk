/*********************************************************************************************************
 * All modification made by [2020] Maxwell Ding
 * All rights reserved.
 * All other contributions:
 * Copyright (C) 2014 by Vitaliy Vitsentiy
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Intel Corporation nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************************************************/

#ifndef CXXTK_THREAD_POOL_H_
#define CXXTK_THREAD_POOL_H_

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "threadsafe_queue.h"

// thread pool to run user's functors with signature
//      ret func(int id, other_params)
// where id is the index of the thread that runs the functor
// ret is some return type

namespace cxxtk {

struct Task {
  void operator()() {
    if (func) {
      (*func)();
    } else {
      printf("No task function\n");
    }
  }
  std::shared_ptr<std::function<void()>> func = nullptr;
  int priority = 0;
  Task() = default;
};

template <typename Q = queue_ts<Task>>
class ThreadPool {
 public:
  using queue_type = typename std::enable_if<Q::thread_safe, Q>::type;
  using task_type = typename std::enable_if<std::is_same<typename queue_type::value_type, Task>::value, Task>::type;
  ThreadPool() = default;
  ThreadPool(int n_threads) { Resize(n_threads); }

  // the destructor waits for all the functions in the queue to be finished
  ~ThreadPool() { Stop(true); }

  // get the number of running threads in the pool
  int Size() { return static_cast<int>(threads_.size()); }

  // number of idle threads
  int IdleNumber() { return n_waiting_; }
  std::thread &GetThread(int i) { return *threads_[i]; }

  // change the number of threads in the pool
  // should be called from one thread, otherwise be careful to not interleave, also with this->stop()
  // n_threads must be >= 0
  void Resize(int n_threads) {
    if (!is_stop_ && !is_done_) {
      int old_n_threads = static_cast<int>(threads_.size());
      if (old_n_threads <= n_threads) {
        // if the number of threads is increased
        threads_.resize(n_threads);
        flags_.resize(n_threads);

        for (int i = old_n_threads; i < n_threads; ++i) {
          flags_[i] = std::make_shared<std::atomic<bool>>(false);
          SetThread(i);
        }
      } else {
        // the number of threads is decreased
        for (int i = old_n_threads - 1; i >= n_threads; --i) {
          // this thread will finish
          *flags_[i] = true;
          threads_[i]->detach();
        }

        {
          // stop the detached threads that were waiting
          std::unique_lock<std::mutex> lock(mutex_);
          cv_.notify_all();
        }

        // safe to delete because the threads are detached
        threads_.resize(n_threads);
        // safe to delete because the threads have copies of shared_ptr of the flags, not originals
        flags_.resize(n_threads);
      }
    }
  }

  // empty the queue
  void ClearQueue() {
    task_type t;
    // empty the queue
    while (task_q_.pop(t))
      ;
  }

  // pops a functional wrapper to the original function
  task_type Pop() {
    task_type t;
    task_q_.pop(t);
    return t;
  }

  // wait for all computing threads to finish and stop all threads
  // may be called asynchronously to not pause the calling thread while waiting
  // if wait_all_task_done == true, all the functions in the queue are run, otherwise the queue is cleared without
  // running the functions
  void Stop(bool wait_all_task_done = false) {
    if (!wait_all_task_done) {
      if (is_stop_) return;
      is_stop_ = true;
      for (int i = 0, n = this->Size(); i < n; ++i) {
        // command the threads to stop
        flags_[i]->store(true);
      }

      // empty the queue
      this->ClearQueue();
    } else {
      if (is_done_ || is_stop_) return;
      // give the waiting threads a command to finish
      is_done_ = true;
    }

    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.notify_all();  // stop all waiting threads
    }

    // wait for the computing threads to finish
    for (int i = 0; i < static_cast<int>(threads_.size()); ++i) {
      if (threads_[i]->joinable()) threads_[i]->join();
    }

    // if there were no threads in the pool but some functors in the queue, the functors are not deleted by the threads
    // therefore delete them here
    this->ClearQueue();
    threads_.clear();
    flags_.clear();
  }

  // run the user's function that excepts argument int - id of the running thread. returned value is templatized
  // operator returns std::future, where the user can get the result and rethrow the catched exceptins
  template <typename callable, typename... arguments>
  auto Push(int priority, callable &&f, arguments &&... args) -> std::future<decltype(f(args...))> {
    auto pck = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
    task_type t;
    t.func.reset(new std::function<void()>([pck]() { (*pck)(); }));
    t.priority = priority;
    task_q_.push(t);
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.notify_one();
    return pck->get_future();
  }

  template <typename callable, typename... arguments>
  void VoidPush(int priority, callable &&f, arguments &&... args) {
    task_type t;
    t.func.reset(new std::function<void()>(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...)));
    t.priority = priority;
    task_q_.push(t);
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.notify_one();
  }

 private:
  // deleted
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;

  void SetThread(int i) {
    std::shared_ptr<std::atomic<bool>> tmp_flag(flags_[i]);
    auto f = [this, tmp_flag]() {
      std::atomic<bool> &flag = *tmp_flag;
      task_type t;
      bool have_task = task_q_.pop(t);
      while (true) {
        // if there is anything in the queue
        while (have_task) {
          t();
          if (flag) {
            // the thread is wanted to stop, return even if the queue is not empty yet
            return;
          } else {
            have_task = task_q_.pop(t);
          }
        }

        // the queue is empty here, wait for the next command
        std::unique_lock<std::mutex> lock(mutex_);
        ++n_waiting_;
        cv_.wait(lock, [this, &t, &have_task, &flag]() {
          have_task = task_q_.pop(t);
          return have_task || is_done_ || flag;
        });
        --n_waiting_;

        // if the queue is empty and is_done_ == true or *flag then return
        if (!have_task) return;
      }
    };

    threads_[i].reset(new std::thread(f));
  }

  std::vector<std::unique_ptr<std::thread>> threads_;
  std::vector<std::shared_ptr<std::atomic<bool>>> flags_;
  queue_type task_q_;
  std::atomic<bool> is_done_{false};
  std::atomic<bool> is_stop_{false};
  // how many threads are waiting
  std::atomic<int> n_waiting_{0};

  std::mutex mutex_;
  std::condition_variable cv_;
};  // class ThreadPool

using EqualityThreadPool = ThreadPool<queue_ts<Task>>;
using PriorityThreadPool = ThreadPool<priority_queue_ts<Task>>;

bool operator<(const Task &lhs, const Task &rhs) { return lhs.priority < rhs.priority; }

}  // namespace cxxtk

#endif  // CXXTK_THREAD_POOL_H_
