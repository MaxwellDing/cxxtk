#ifndef CXXTK_WORKER_H_
#define CXXTK_WORKER_H_

#include <future>
#include <memory>
#include <string>

namespace cxxtk {

class SchedulerInterface;

class Worker {
 public:
  Worker(const std::string &name): name_(name) {}
  virtual ~Worker() {}

  inline bool OnDuty(std::shared_ptr<SchedulerInterface> office) {
    report_ = std::async(std::launch::async, &Worker::Work, this, office);
    return true;
  }
  inline bool OffDuty() {
    return report_.get();
  }
  inline bool Working() {
    return report_.valid();
  }
  inline const std::string GetErrorReport() const {
    return error_report_;
  }
  inline const std::string GetName() const {
    return name_;
  }

 private:
  virtual bool Work(std::shared_ptr<SchedulerInterface> office);
  std::string name_;
  std::string error_report_;
  std::future<bool> report_;
};  // class Worker

using Worker_t = std::shared_ptr<Worker>;

}  // namespace cxxtk

#endif  // CXXTK_WORKER_H_

