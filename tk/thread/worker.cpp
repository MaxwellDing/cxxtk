#include "log/logger.h"
#include "thread/scheduler.h"
#include "thread/task.h"
#include "thread/worker.h"

namespace cxxtk {

bool Worker::Work(std::shared_ptr<SchedulerInterface> office) {
  Task_t t;
  while(t = office->TakeTask()) {
    if (!t->Execute()) {
      error_report_ = t->GetReport();
      LOG(INFO, "Worker %p get off work, since execute task failed, report:%s", this, error_report_.c_str());
      return false;
    }
  }
  return true;
}

}  // namespace cxxtk

