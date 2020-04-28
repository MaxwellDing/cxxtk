#ifndef CXXTK_SCHEDULER_H_
#define CXXTK_SCHEDULER_H_

#include <atomic>
#include <condition_variable>
#include <memory>
#include <queue>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "thread/task.h"
#include "thread/worker.h"
#include "thread/spinlock.h"

namespace cxxtk {

class SchedulerInterface {
 public:
  virtual ~SchedulerInterface() {};
  /*
   * @brief Add task to scheduler office
   */
  virtual bool SubmitTask(Task_t task) = 0;

  /*
   * @brief Employ a worker to execute tasks
   */
  virtual bool EmployWorker(Worker_t worker);

  /*
   * @brief Fire a worker
   */
  virtual bool FireWorker(const std::string &name);

  /*
   * @brief Inform workers to go to work
   */
  virtual bool OnDuty() = 0;

  /*
   * @brief Inform workers to get off work
   * @note scheduler reject tasks during closing time
   */
  virtual bool OffDuty() = 0;

  /*
   * @brief Not inform workers to get off work until all the tasks are finished
   * @note scheduler reject tasks during closing time
   */
  virtual bool OverTime() = 0;

  virtual Task_t TakeTask() = 0;

 protected:
  std::unordered_map<std::string, Worker_t> workers_;
  SpinLock worker_list_mutex_;
};  // class SchedulerInterface

template<class Q = std::queue<std::shared_ptr<Task_t>>>
class SchedulerBase: public SchedulerInterface {
 public:
  typedef typename std::enable_if<std::is_same<typename Q::value_type, Task_t>::value, Q>::type queue_type;
  SchedulerBase() {}
  virtual ~SchedulerBase() {};

  /*
   * @brief Add task to scheduler office
   */
  bool SubmitTask(Task_t task);

  /*
   * @brief Employ a worker to execute tasks
   */
  bool EmployWorker(Worker_t worker);

  /*
   * @brief Fire a worker
   */
  bool FireWorker(const std::string &name);

  /*
   * @brief Inform workers to go to work
   */
  bool OnDuty();

  /*
   * @brief Inform workers to get off work
   * @note scheduler reject tasks during closing time
   */
  bool OffDuty();

  /*
   * @brief Not inform workers to get off work until all the tasks are finished
   * @note scheduler reject tasks during closing time
   */
  bool OverTime();

  virtual Task_t TakeTask();

 protected:
  /*
   * @brief Arrange task to task list
   */
  virtual bool ArrangeTask(Task_t task);

  Q task_list_;
  std::mutex task_mutex_;
  std::condition_variable task_cond_;
  std::atomic<bool> accept_task_ = false;

 private:
  std::atomic<bool> work_hours_ = false;
};  // class SchedulerBase

class PriorityScheduler: public SchedulerBase<std::priority_queue<Task_t>> {
};  // class PriorityScheduler

class Scheduler: public SchedulerBase<std::queue<Task_t>> {
};  // class Scheduler

}  // namespace cxxtk

#endif  // SCHEDULER_H_

