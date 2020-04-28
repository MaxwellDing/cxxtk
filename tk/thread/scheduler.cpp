#include "thread/scheduler.h"

#include <chrono>
#include <thread>

#include "log/logger.h"

namespace cxxtk {

bool SchedulerInterface::EmployWorker(Worker_t worker) {
  SpinLockGuard lk(worker_list_mutex_);
  if (worker->Working() || !workers_.insert({worker->GetName(), worker}).second) {
    LOG(ERROR, "Worker %s has been employed", worker->GetName().c_str());
    return false;
  }
  return true;
}

bool SchedulerInterface::FireWorker(const std::string &name) {
  bool ret = true;
  
  SpinLockGuard lk(worker_list_mutex_);
  if (workers_.find(name) == workers_.end()) {
    LOG(WARNING, "Cannot fire worker %s that is not in this office %p", name, this);
    ret = false;
  } else {
    auto worker = workers_.at(name);
    if (worker->Working()) worker->OffDuty();
    LOG(INFO, "Fire worker %s", name);
    workers_.erase(name);
    ret = true;
  }
  return ret;
}

template <class Q>
bool SchedulerBase<Q>::SubmitTask(Task_t task) {
  if (!accept_task_.load()) {
    LOG(WARNING, "Reject task");
    return false;
  }
  LOG(TRACE, "submit task%p", task.get());
  return ArrangeTask(task);
}

template <class Q>
bool SchedulerBase<Q>::OnDuty() {
  if (work_hours_.load()) {
    LOG(WARNING, "Scheduler is already on duty");
    return false;
  }
  work_hours_.store(true);
  accept_task_.store(true);
  SpinLockGuard lk(worker_list_mutex_);
  for (auto &p : workers_) {
    if (!p.second->OnDuty(this)) {
      LOG(WARNING, "Worker %p do not want to work");
    }
  }

  return true;
}

template <class Q>
bool SchedulerBase<Q>::OffDuty() {
  if (!work_hours_.load()) {
    LOG(WARNING, "Scheduler is not on duty");
    return false;
  }
  accept_task_.store(false);
  work_hours_.store(false);
  SpinLockGuard lk(worker_list_mutex_);
  for (auto &p : workers_) {
    if (!p.second->OffDuty(this)) {
      LOG(ERROR, "Report from worker %p: %s", p.second->GetErrorReport().c_str());
    }
  }
  return true;
}

template <class Q>
bool SchedulerBase<Q>::ArrangeTask(Task_t task) {
  std::lock_guard<std::mutex> lk(task_mutex_);
  task_list_.push(task);
  task_cond_.notify_one();
  return true;
}

template<class Q>
bool SchedulerBase<Q>::OverTime() {
  if (!work_hours_.load()) {
    LOG(WARNING, "Scheduler is not on duty");
    return false;
  }
  accept_task_.store(false);
  std::unique_lock<std::mutex> task_lk(task_mutex_);
  while (!task_list_.empty()) {
    task_cond_.notify_all();
    task_lk.unlock();
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100));
    task_lk.lock();
  }
  task_lk.unlock();

  return OffDuty();
}

inline Task_t GetFrontAndPop(std::queue<Task_t> q) {
  auto tmp = q.front();
  q.pop();
  return tmp;
}

inline Task_t GetFrontAndPop(std::priority_queue<Task_t> q) {
  auto tmp = q.top();
  q.pop();
  return tmp;
}

template <class Q>
Task_t SchedulerBase<Q>::TakeTask() {
  std::unique_lock<std::mutex> lk(task_mutex_);
  while (work_hours_.load() && task_list_.empty()) {
    task_cond_.wait_for(lk, std::chrono::duration<double, std::milli>(50),
                        [this] { return !this->task_list_.empty(); });
  }
  return GetFrontAndPop(task_list_);
}

}  // namespace cxxtk

