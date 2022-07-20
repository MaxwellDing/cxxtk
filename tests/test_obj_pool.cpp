#include "tk/util/obj_pool.hpp"
#include "tk/util/object.hpp"
#include "tk/util/scope.hpp"

class TestObj : public tk::util::Object<TestObj> {};

int main() {
  auto f_a = tk::util::finally([]() { tk::util::Object<TestObj>::Print(); });

  tk::util::ObjectPool pool;
  printf("pointer\n");
  TestObj* po = pool.requireObj<TestObj>();
  printf("ref\n");
  TestObj& ro = *pool.requireObj<TestObj>();
  printf("value\n");
  TestObj vo = *pool.requireObj<TestObj>();
  printf("clear\n");
  pool.releaseAll();
  printf("end\n");
  return 0;
}
