#ifndef CXXTK_TASK_H_
#define CXXTK_TASK_H_

#include <functional>
#include <memory>

namespace cxxtk {

class Task;
using Task_t = std::shared_ptr<Task>;

/**
 *  @brief notify once when task is done
 */
using Notifier = std::function<void(Task*)>;

/**
 *  @brief A task class
 */
class Task {
 public:
  Task(const Notifier &n = nullptr, int priority = 0): notifier_(n), priority_(priority) {}
  virtual ~Task() {}
  inline bool Execute() {
    bool ret = Do();
    if (ret && notifier_) notifier_(this);
    return ret;
  }
  inline int GetPriority() const { return priority_; }
  inline const std::string GetReport() const { return error_report; }

 protected:
  virtual bool Do() = 0;
  Notifier notifier_;
  int priority_;
  std::string error_report;
};

inline bool operator<(const Task_t lhs, const Task_t rhs) {
  return lhs->GetPriority() < rhs->GetPriority();
}

}  // namespace cxxtk

#endif  // CXXTK_TASK_H_

